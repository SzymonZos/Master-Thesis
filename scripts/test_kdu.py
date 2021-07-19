import os
import re
import subprocess
from itertools import product


BASE = "./bin/Linux-x86-64-gcc/kdu_compress"
OPTS = "Creversible=yes"


def run_cmd(path, cmd):
    imgs = f"-i {path} -o ./image.jpx"
    cmd_base = f"{BASE} {imgs} {OPTS}"
    res = subprocess.run(f"{cmd_base} {cmd}",
                         stdout=subprocess.PIPE, shell=True)
    out = res.stdout.decode('utf-8')
    try:
        out = re.search(R"(Layer.+\n\D+)([\d\.]+)", out, re.M).group(2)
    except AttributeError:
        out = "nil"
    return f"Bitrate: {out}"


def test_kdu(path):
    vertical = ['V(-)']
    horizontal = ['H(-)']
    both = ['B(-:-:-)']
    comb = [vertical, horizontal, both]
    prod = [[] for _ in range(5)]
    for i in range(5):
        prod[i] = [*product(comb, repeat=i+1), ]
    cmds = []
    for i, comps in enumerate(prod):
        level = f"Clevels={i+1}"
        for comp in comps:
            comp = ','.join(x[0] for x in comp)
            cmds.append(f'{level} Cdecomp="{comp}"')
    for cmd in cmds:
        print(f"{cmd}: {run_cmd(path, cmd)}")


def main():
    for file in os.scandir('./images_ppm'):
        print(file.path)
        test_kdu(file.path)
        break


if __name__ == '__main__':
    main()
