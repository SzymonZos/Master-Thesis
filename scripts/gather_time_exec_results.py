import os
import re
from pathlib import Path
from functools import cmp_to_key
import matplotlib.pyplot as plt
import matplotlib
import numpy as np


def n_thread(file: str):
    try:
        return int(re.search(R"\d+", file).group(0))
    except AttributeError:
        return 100


def comp_threads(t1, t2):
    return n_thread(t1) - n_thread(t2)


def len_dict_values(d):
    return len(next(iter(d.values())))


def save_all_plots(img_wise, thread_names):
    for i, img in enumerate(img_wise):
        fig = plt.figure()
        plt.plot(img, "x", markeredgewidth=3, markersize=10)
        plt.xticks(range(len(img)), thread_names, rotation=45)
        plt.title('Elapsed time comparison of different execution methods', fontsize=16)
        plt.grid()
        plt.xlabel('Type of execution', fontsize=16)
        plt.ylabel('Elapsed time (s)', fontsize=16)
        plt.savefig(f"../plots/img_{i}.png", bbox_inches="tight")
        plt.close(fig)


def normalize_results(results):
    for i, img in enumerate(results):
        norm = img[0]
        for j, val in enumerate(img):
            results[i][j] = val / norm


def means(results):
    return [*map(np.mean, np.array(results).T), ]


def plot_means(means, names, which):
    fig = plt.figure()
    plt.plot(means, "x", markeredgewidth=3, markersize=10)
    plt.xticks(range(len(means)), names, rotation=45)
    plt.title('Improvement over single thread comparison of different execution methods', fontsize=16)
    plt.grid()
    plt.xlabel('Type of execution', fontsize=16)
    plt.ylabel('Relation to single thread time execution', fontsize=16)
    plt.savefig(f"../plots/means_{which}.png", bbox_inches="tight")
    plt.close(fig)


def main():
    comp = dict()
    for file in os.scandir('../results_time_exec'):
        print(file.path)
        with open(file.path, "r") as f:
            results = f.read()
            key = Path(file.path).stem
            comp[key] = [*map(float, re.findall(R"(?<=Elapsed time: )[\d.]+", results)), ]
        print("")
    img_wise = [[] for _ in range(len_dict_values(comp))]
    # img_wise = []
    thread_names = []
    for key in sorted(comp, key=cmp_to_key(comp_threads)):
        thread_names.append(key)
        for i, img in enumerate(comp[key]):
            img_wise[i].append(img)
    matplotlib.rc('figure', figsize=(12, 9))
    # save_all_plots(img_wise, thread_names)
    img_wise_base = img_wise.copy()
    normalize_results(img_wise_base)
    base_means = means(img_wise_base)
    img_wise.remove(max(img_wise, key=max))
    plot_means(base_means, thread_names, "base")
    normalize_results(img_wise)
    mod_means = means(img_wise)
    plot_means(mod_means, thread_names, "mod")
    print("DUPA")


if __name__ == '__main__':
    main()
