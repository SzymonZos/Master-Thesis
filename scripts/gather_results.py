import os
import re
import numpy as np


def main():
    refs = []
    outs = []
    for file in os.scandir('../results'):
        with open(file.path, "r") as f:
            results = f.read()
            try:
                ref = re.search(R"(?<=Creversible=yes Clevels=5 Cdecomp=\"B\(-:-:-\),B\(-:-:-\),B\(-:-:-\),B\(-:-:-\),B\(-:-:-\)\": )[\d.]+", results).group(0)
            except AttributeError:
                ref = re.search(R"(?<=Creversible=yes: )[\d.]+", results).group(0)
            refs.append(float(ref))
            out = re.findall(R"(?<=: )[\d.]+", results)
            outs.append(min(map(float, out)))
    delta = []
    for ref, out in zip(refs, outs):
        delta.append((out - ref) / ref * 100)
    print(f"mean: {np.mean(delta)}")
    print(f"stddev: {np.std(delta)}")
    print(f"min: {np.min(delta)}")
    print(f"max: {np.max(delta)}")


if __name__ == '__main__':
    main()
