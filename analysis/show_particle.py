import pandas as pd
import argparse
from particle import Particle, PDGID

def print_showers(IDs):
    df = pd.read_csv("../corsika_showers/shower7.csv")
    for ID in IDs:
        df_filter = df[df["event"] == int(ID)]
        for _, event in df_filter.iterrows():
            x = event["x_cm"]
            y = event["y_cm"]
            if (x**2 + y**2)**0.5 <= 300: # 3m in cm
                if PDGID(int(event['pdg'])).is_lepton:
                    energy = (event['px_GeV']**2 + event['py_GeV']**2 + event['pz_GeV']**2)**0.5
                    print(f"Event: {ID}, {Particle.from_pdgid(int(event['pdg']))} at: {x} cm, {y} cm with energy {energy:.2f} GeV")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Show particle data from CSV")
    parser.add_argument("-n", type=str, default="1", help="Shower ID")
    args = parser.parse_args()
    IDs = args.n.split(",")
    print_showers(IDs)