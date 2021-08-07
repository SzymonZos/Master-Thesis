import os
import re
import numpy as np


def find_res(res, results):
    return float(re.search(Rf"(?<={res}: )[\d.]+", results).group(0))


def main():
    refs = []
    outs = []
    for file in os.scandir('../ref_results'):
        # print(file.path)
        with open(file.path, "r") as f:
            results = f.read()
            refs.append(find_res("ref", results))
            outs.append(find_res("best", results))
    delta = []
    for ref, out in zip(refs, outs):
        delta.append((out - ref) / ref * 100)
    print(*filter(lambda x: x >= 0., delta),)
    print(f"mean: {np.mean(delta)}")
    print(f"stddev: {np.std(delta)}")
    print(f"min: {np.min(delta)}")
    print(f"max: {np.max(delta)}")


if __name__ == '__main__':
    main()
