import os
import re
import subprocess
from itertools import product
from pathlib import Path


BASE = "./bin/Linux-x86-64-gcc/kdu_compress"
FILTERS = {
    'bior2.2': 'Creversible=yes',
    'haar': 'Catk=2 Kkernels:I2=R2X2',
    'bior2.6': 'Catk=2 Kextension:I2=SYM Kreversible:I2=yes "Ksteps:I2={4,-1,4,8},{4,-2,4,8}" '
               'Kcoeffs:I2=0.0625,-0.5625,-0.5625,0.0625,-0.0625,0.3125,0.3125,-0.0625'
}


def run_cmd(path, cmd):
    imgs = f"-i {path} -o ./image.jpx"
    cmd_base = f"{BASE} {imgs}"
    res = subprocess.run(f"{cmd_base} {cmd}",
                         stdout=subprocess.PIPE, shell=True)
    out = res.stdout.decode('utf-8')
    try:
        out = re.search(R"(Layer.+\n\D+)([\d\.]+)", out, re.M).group(2)
    except AttributeError:
        out = "inf"
    return float(out)


def test_kdu_dwt_comp(path, dwt_filter):
    vertical = ['V(-)']
    horizontal = ['H(-)']
    both = ['B(-:-:-)']
    comb = [vertical, horizontal, both]
    comps = [*product(comb, repeat=5), ]
    cmds = []
    level = "Clevels=5"
    for comp in comps:
        comp = ','.join(x[0] for x in comp)
        cmds.append(f'{FILTERS[dwt_filter]} {level} Cdecomp="{comp}"')
    results = dict()
    for cmd in cmds:
        results[cmd] = run_cmd(path, cmd)
    min_bitrate = min(results, key=results.get)
    with open(f"./ref_results/{Path(path).stem}.txt", "a") as f:
        f.write(f"best: {results[min_bitrate]}\n")


def test_kdu_filter(path):
    results = dict()
    for dwt_filter, cmd in FILTERS.items():
        results[dwt_filter] = run_cmd(path, cmd)
    with open(f"./ref_results/{Path(path).stem}.txt", "a") as f:
        f.write(f"ref: {results['bior2.2']}\n")
    return min(results, key=results.get)


def main():
    for file in os.scandir('../img/FINAL'):
        print(file.path)
        dwt_filter = test_kdu_filter(file.path)
        test_kdu_dwt_comp(file.path, dwt_filter)
        print("")


if __name__ == '__main__':
    main()
