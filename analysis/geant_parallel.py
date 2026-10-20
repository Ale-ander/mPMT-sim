from multiprocessing import Pool, Manager
from pathlib import Path
import random
import subprocess
import time
import argparse
import json

try:
    import psutil
except ImportError:
    psutil = None

# ----------
# BASE PATH
# ----------
BASE_DIR = Path("/home/sansone/multiPMTsim")
GEANT_PATH = Path("/home/sansone/multiPMTsim/multipmt-simulation/build/task")


# ----------------------
def load_macro(path):
    with open(path) as f:
        return f.readlines()


def make_macro(template_lines, suffix, outdatadir, particle_info):
    """
    particle_info: dict con chiavi tipo:
      'particle', 'energy', 'posX', 'posY', 'posZ', 'dirX', 'dirY', 'dirZ'
    """
    seed = random.randrange(1, 2**31 - 1)
    out = []

    for L in template_lines:
        line = L.strip()
        if line.startswith("/output/setSuffix"):
            out.append(f"/output/setSuffix {suffix}\n")
        elif line.startswith("/output/dataPath"):
            out.append(f"/output/dataPath {outdatadir}\n")
        elif line.startswith("/gps/particle"):
            out.append(f"/gps/particle {particle_info['particle']}\n")
        elif line.startswith("/gps/ene/mono"):
            out.append(f"/gps/ene/mono {particle_info['energy']} GeV\n")
        elif line.startswith("/gps/pos/centre"):
            out.append(f"/gps/pos/centre {particle_info['posX']} {particle_info['posY']} {particle_info['posZ']} cm\n")
        elif line.startswith("/gps/direction"):
            out.append(f"/gps/direction {particle_info['dirX']} {particle_info['dirY']} {particle_info['dirZ']}\n")
        elif line.startswith("/random/setSeeds"):
            out.append(f"/random/setSeeds {seed} {seed+1}\n")
        else:
            out.append(L)

    return "".join(out)


def run_single(args):
    template_lines, run_id, outdatadir, particle_info, geant_bin, errlog_path, progress = args

    suffix = f"run_{run_id:05d}"

    macro_text = make_macro(
        template_lines=template_lines,
        suffix=suffix,
        outdatadir=outdatadir,
        particle_info=particle_info
    )

    macro_file = outdatadir / f"{suffix}.mac"
    macro_file.write_text(macro_text)

    subprocess.run(
        [str(geant_bin), str(macro_file)],
        stdout=subprocess.DEVNULL,
        stderr=open(errlog_path, "a"),
        cwd=geant_bin.parent
    )

    macro_file.unlink(missing_ok=True)
    progress["done"] += 1
    return True


def monitor(total, progress):
    while progress["done"] < total:
        done = progress["done"]
        pct = 100 * done / total
        if psutil:
            cpu = psutil.cpu_percent()
            ram = psutil.virtual_memory().percent
            print(f"[{done}/{total}] {pct:.1f}% | CPU {cpu:.1f}% | RAM {ram:.1f}%")
        else:
            print(f"[{done}/{total}] {pct:.1f}%")
        time.sleep(5)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--template", required=True, help="Macro template GEANT4")
    parser.add_argument("--input", required=True, help="JSON con parametri particella")
    parser.add_argument("--simulations", type=int, default=100)
    parser.add_argument("--cores", type=int, default=4)
    parser.add_argument("--tag", required=True, help="Cartella di run")
    args = parser.parse_args()

    template_lines = load_macro(args.template)

    # Carica info particella
    with open(args.input) as f:
        particle_info = json.load(f)

    # Crea cartella per le run
    run_dir = BASE_DIR / f"geant_runs_{args.tag}"
    run_dir.mkdir(parents=True, exist_ok=True)

    # Logs
    log_dir = BASE_DIR / "geant_logs"
    log_dir.mkdir(exist_ok=True)
    errlog_path = log_dir / "geant_errors.log"

    # Progress
    manager = Manager()
    progress = manager.dict()
    progress["done"] = 0

    # Monitor
    from threading import Thread
    mon = Thread(target=monitor, args=(args.simulations, progress), daemon=True)
    mon.start()

    # Pool
    pool = Pool(args.cores)
    tasks = [
        (template_lines, run_id, run_dir, particle_info, GEANT_PATH, errlog_path, progress)
        for run_id in range(1, args.simulations + 1)
    ]

    pool.map(run_single, tasks)
    pool.close()
    pool.join()

    print("\nTutte le simulazioni sono state generate.")
    print(f"I file sono in: {run_dir}")
    print(f"Error log: {errlog_path}")


if __name__ == "__main__":
    main()
