import csv
import ast
import random
import numpy as np
import pandas as pd
from PIL import Image
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import matplotlib.patheffects as pe
from matplotlib.cm import ScalarMappable
from matplotlib.patches import Circle
from matplotlib.lines import Line2D

def fast_parse_times(s):
    s = s.strip('[]"')
    return [float(x) for x in s.split(",")]

def fast_first_hit(s):
    s = s.strip('[]"')
    i = s.find(',')
    return float(s if i == -1 else s[:i])

def clean_folder(folder_path: Path):
    for file in folder_path.glob("*.png"):
        file.unlink()

def convert_geant_to_analysis(geant_file_path: str, output_filepath: str):
    df = pd.read_csv(geant_file_path)
    event_list = []
    for event_id, group in df.groupby('EventID'):
        row = {
            'ChNumber': len(group),
            'Channels': group['PmtID'].tolist(),
            'Energies': group['Hits'].tolist(),
            'TotalPE': group['Hits'].sum(),
            'Times': group['TimeHits'].tolist()
        }
        event_list.append(row)

    df_out = pd.DataFrame(event_list)
    df_out.to_csv(output_filepath, index=False)


def convert_geant_to_mean(geant_file_path: str, mean_file_path: str):
    df = pd.read_csv(geant_file_path)

    def parse_min_time(x):
        if isinstance(x, str):
            x = ast.literal_eval(x)
        return np.min(x)

    df['MinTime'] = df['TimeHits'].apply(parse_min_time)
    df_filtered = df[df['MinTime'] <= 300].copy()

    if df_filtered.empty:
        print("Nessun dato soddisfa la condizione Time <= 300.")
        return

    event_min_times = df_filtered.groupby('EventID')['MinTime'].transform('min')
    df_filtered['TimeRef'] = df_filtered['MinTime'] - event_min_times
    res = df_filtered.groupby('PmtID').agg(
        Hits=('Hits', 'mean'),
        TimeHits=('TimeRef', lambda x: f"[{x.mean()}]")  # Formattato come stringa con parentesi quadre
    ).reset_index()

    res['EventID'] = 0
    res = res[['EventID', 'PmtID', 'Hits', 'TimeHits']]
    res.to_csv(mean_file_path, index=False)


def canale_to_xy(canale):
    if canale == 18:
        return np.array([0.0, 0.0])

    elif 12 <= canale <= 17:
        idx = canale - 6
        angolo = idx * 2 * np.pi / 6
        r = 1.0
        return np.array([r * np.cos(angolo), r * np.sin(angolo)])

    elif 0 <= canale <= 11:
        idx = canale
        angolo = idx * 2 * np.pi / 12
        r = 2.0
        return np.array([r * np.cos(angolo), r * np.sin(angolo)])

    else:
        raise ValueError(f"Canale PMT non valido: {canale}")


def charge_centroid(csv_file, multiplicity):
    df = pd.read_csv(csv_file)
    df.columns = ['ChNumber', 'Channels', 'Energies', 'TotalPE', 'Times']
    df_filtered = df[df['ChNumber'] >= multiplicity].copy()

    if df_filtered.empty:
        print("Nessun evento trovato con la molteplicità richiesta.")
        return

    def parse_if_str(val):
        return ast.literal_eval(val) if isinstance(val, str) else val

    df_filtered['Channels'] = df_filtered['Channels'].apply(parse_if_str)
    df_filtered['Energies'] = df_filtered['Energies'].apply(parse_if_str)
    df_filtered['TotalPE'] = df_filtered['TotalPE'].apply(parse_if_str)

    impatti = []
    for _, row in df_filtered.iterrows():
        canali = row['Channels']
        energie = np.array(row['Energies'])
        energia_tot = row['TotalPE']
        coords = np.array([canale_to_xy(c) for c in canali])  # Shape: (N_canali, 2)
        punto = np.dot(energie, coords) / energia_tot
        impatti.append(punto)

    impatti = np.array(impatti)

    fig, ax = plt.subplots(figsize=(10, 8))

    h2d = ax.hist2d(
        impatti[:, 0],
        impatti[:, 1],
        bins=100,
        cmap='plasma',
        range=[[-2.5, 2.5], [-2.5, 2.5]],
        vmin=1e-5
    )

    for c in range(19):
        x, y = canale_to_xy(c)
        ax.plot(x, y, 'o', markersize=10, color="deepskyblue")
        ax.text(
            x,
            y + 0.1,
            str(c + 1),
            color='deepskyblue',
            ha='center',
            va='center',
            fontweight='bold',
            fontsize=14
        )

    ax.set_title(
        f"Charge Centroid - multiplicity >= {multiplicity}",
        fontsize=20
    )

    cbar = plt.colorbar(h2d[3], ax=ax)
    cbar.set_label('Events', fontsize=26)
    cbar.ax.tick_params(labelsize=20)

    ax.set_xticks([])
    ax.set_yticks([])
    ax.tick_params(axis='both', which='both', labelbottom=False, labelleft=False)

    ax.set_aspect('equal', adjustable='box')
    plt.tight_layout()
    plt.show()


def leggi_evento_csv(filename, event_index: int = 1):
    df = pd.read_csv(filename)

    blocco = df[df['EventID'] == event_index-1]

    pmtID = blocco['PmtID'].to_list()
    energies = blocco['Hits'].to_numpy()
    times = blocco['TimeHits'].apply(lambda x: min(ast.literal_eval(x))).tolist()

    return pmtID, energies, times


def plot_event(idx, pmtID, all_energies, all_times, min_multiplicity=1, time_window=10.0, SCALE=45, save=False, filename=''):
    pmtID = np.array(pmtID)
    all_energies = np.array(all_energies)
    all_times = np.array(all_times)

    if len(all_times) == 0:
        print("Nessun segnale nell'evento.")
        return

    # 1. Trova il primo tempo (t_0) e applica la finestra temporale
    first_time = np.min(all_times)
    valid_mask = (all_times >= first_time) & (all_times <= first_time + time_window)

    # 2. Filtra i dati entro la finestra temporale
    valid_pmtID = pmtID[valid_mask]
    valid_energies = all_energies[valid_mask]

    # --- NORMALIZZAZIONE TEMPI: Delta t rispetto a t_0 ---
    valid_times = all_times[valid_mask] - first_time  # Il primo PMT diventa 0.0 ns

    # 3. Controllo Molteplicità
    multiplicity = len(valid_pmtID)
    if multiplicity < min_multiplicity:
        print(f"Event discarded: multiplicity ({multiplicity}) under value required ({min_multiplicity}).")
        return

    # Troviamo l'ID del primo PMT (quello che ora ha tempo 0.0)
    first_pmt_id = valid_pmtID[np.argmin(valid_times)]

    # Colormap e Normalizzazione sui tempi normalizzati
    norm = mcolors.Normalize(
        vmin=0.0,
        vmax=time_window
    )
    colormap = plt.colormaps.get("RdYlBu")
    sm = ScalarMappable(cmap=colormap, norm=norm)

    fig, ax = plt.subplots(figsize=(10, 8))

    text_effect = [pe.withStroke(linewidth=3, foreground='white')]
    text_offset = 0.12
    max_radius_for_outside_text = 0.5
    inner_text_distance = 0.25

    def get_text_position(y, radius):
        if radius < max_radius_for_outside_text:
            return y + radius + text_offset, y - radius - text_offset
        else:
            return y + inner_text_distance, y - inner_text_distance

    coords = [canale_to_xy(i) for i in range(19)]
    x_positions = [c[0] for c in coords]
    y_positions = [c[1] for c in coords]

    valid_event_map = {p: (e, t) for p, e, t in zip(valid_pmtID, valid_energies, valid_times)}

    for i in range(19):
        x, y = x_positions[i], y_positions[i]

        if i in valid_event_map:
            energy, dt = valid_event_map[i]
            radius = (energy / SCALE) + 0.05

            # Selezione colore
            if i == first_pmt_id:
                color = 'limegreen'
            else:
                color = colormap(norm(dt))

            # Cerchio PMT
            circle = Circle((x, y), radius, color=color, ec='k', lw=1.5 if i == first_pmt_id else 1)
            ax.add_patch(circle)

            # Numero canale
            ax.text(x, y, str(i + 1), ha='center', va='center', fontweight='bold' if i == first_pmt_id else 'normal')

            # Testo energia e Delta t
            y_top, y_bottom = get_text_position(y, radius)
            ax.text(x, y_top, f"{round(energy)} pe", ha='center', va='bottom', fontsize=11, color='black',
                    path_effects=text_effect)
            ax.text(x, y_bottom, '' if i == first_pmt_id else f"{round(dt, 3)} ns", ha='center', va='top', fontsize=11, color='black',
                    path_effects=text_effect)
        else:
            ax.text(x, y, str(i + 1), ha='center', va='center')
            circle = Circle((x, y), 0.25, color="white", linestyle="--", ec='k', lw=0.8)
            ax.add_patch(circle)

    # --- Colorbar ---
    cbar = fig.colorbar(sm, ax=ax, orientation='vertical', fraction=0.1, pad=0.055)
    cbar.ax.tick_params(labelsize=18)
    cbar.set_label(r"$\Delta t$ (ns)", fontsize=18)  # Indicatore Delta t

    # --- Legenda Primo Trigger ---
    legend_element = [
        Line2D([0], [0], marker='o', color='w', label=r'First PMT triggered',
               markerfacecolor='limegreen', markeredgecolor='k', markersize=10)
    ]
    ax.legend(
        handles=legend_element,
        loc='upper left',
        bbox_to_anchor=(0.0, -0.02),
        frameon=False,
        fontsize=11
    )

    # --- Layout ---
    ax.set_title(f'Event: {idx} -- Multiplicity: {multiplicity} -- Time Window: {time_window}ns', fontsize=18)
    ax.set_aspect('equal', adjustable='box')
    ax.set_xlim(-2.5, 2.5)
    ax.set_ylim(-2.5, 2.5)
    ax.axhline(0, color='gray', linewidth=0.5, linestyle='--')
    ax.axvline(0, color='gray', linewidth=0.5, linestyle='--')

    ax.set_xticks([])
    ax.set_yticks([])

    if save:
        plt.savefig(filename, bbox_inches='tight')
        print(f"File saved: {filename}")
        plt.close(fig)
    else:
        plt.show()


def plot_multiple_events(geant_file_dir: Path, n_events: int = 5, randomize: bool = False, SCALE: int = 45, normalize_times: bool = True):

    all_files = sorted(geant_file_dir.rglob("csv/*.csv"))
    
    if randomize:
        chosen_files = random.sample(all_files, min(n_events, len(all_files)))
    else:
        chosen_files = all_files[:n_events]
    
    for i, file in enumerate(chosen_files):
        pmtid, energies, times = leggi_evento_csv(file)

        if normalize_times:
            valid_times = times[times > 0]
            if len(valid_times) > 0:
                t_min = np.min(valid_times)
                times = times - t_min
        
        plot_event(
            pmtid,
            energies,
            times,
            SCALE=SCALE
        )


def plot_trace_signal(geant_file_dir: Path, n_events: int = 5, randomize: bool = False, time_bin: float = 0.1, xlim: int = 30):

    all_files = sorted(geant_file_dir.rglob("csv/*.csv"))
    
    if randomize:
        chosen_files = random.sample(all_files, min(n_events, len(all_files)))
    else:
        chosen_files = all_files[:n_events]

    for file in chosen_files:
        with open(file, 'r', newline='') as trace_file:

            reader = csv.reader(trace_file)
            next(reader)

            channel_times = {ch: [] for ch in range(7)}

            for row in reader:
                ch = int(row[1])
                time_hits = fast_parse_times(row[3])
                if not time_hits:
                    continue

                time_start = time_hits[0]
                time_sample = [time_start]

                for t in time_hits[1:]:
                    if abs(t - time_start) <= time_bin:
                        time_sample.append(t)
                    else:
                        channel_times[ch].extend(time_sample)
                        time_sample = [t]
                        time_start = t

                if time_sample:
                    channel_times[ch].extend(time_sample)

            for ch, times in channel_times.items():
                if times:
                    bins = np.arange(0, max(times) + time_bin, time_bin)
                    plt.figure(figsize=(12, 4))
                    plt.hist(times, bins=bins, color='deepskyblue', edgecolor='k', alpha=0.7, density=True)
                    plt.xlim(0, xlim)
                    plt.xlabel("Time [ns]", fontsize=14)
                    plt.ylabel("Hits per bin", fontsize=14)
                    plt.title(f"Trace PMT {ch} - {file.name} (bin = {time_bin} ns)", fontsize=16)
                    plt.tight_layout()
                    plt.show()

if __name__ == "__main__":
    data_file = "//wsl.localhost/Ubuntu/home/alecs/multipmt-simulation-main/data/2026-10-14/csv/HitsData_PMT_test.csv"
    output_file = "outputs/output.csv"
    convert_geant_to_analysis(data_file, output_file)
    # charge_centroid(output_file, multiplicity=2)
    clean_folder(Path("./plots/"))
    for idx in range(100):
        pmtID, energy, times = leggi_evento_csv(data_file, event_index=idx+1)
        if not pmtID:
            continue
        plot_event(idx, pmtID, energy, times, min_multiplicity=5, time_window=3, SCALE=40, save=True, filename=f'./plots/2Gev_muneg_{idx}.png')
    # create GIF
    image_path = Path("./plots/")
    files = list(image_path.glob("2Gev_muneg_*.png"))
    files_sorted = sorted(files, key=lambda x: int(x.stem.split("_")[-1]))
    first_img = Image.open(files_sorted[0])
    images = [Image.open(f) for f in files_sorted[1:]]
    first_img.save("output.gif",save_all=True,append_images=images,duration=1000,loop=0)