import os
import re
import subprocess
from pathlib import Path


def run_cmd(cmd):
    res = subprocess.run(cmd, stdout=subprocess.PIPE, shell=True)
    out = res.stdout.decode('utf-8')
    try:
        out = re.search(R"(Layer.+\n\D+)([\d\.]+)", out, re.M).group(2)
    except AttributeError:
        out = "inf"
    return float(out)


def test_kdu(commands):
    for file, cmds in commands.items():
        with open(f"./results/{Path(file).stem}.txt", "a") as f:
            for cmd in cmds:
                f.write(f"{file}: {run_cmd(cmd)}")


def read_dwts_dict():
    dwts_dict = dict()
    with open("../img/results_processed.txt", "r") as f:
        for i, line in enumerate(f.readlines()):
            if i % 3 == 0:
                file = line.rstrip()
            if i % 3 == 1:
                dwts_dict[file] = eval(line)
    return dwts_dict


BASE = "./bin/Linux-x86-64-gcc/kdu_compress"
OUT = "-o ./image.jpx"
FILTERS = {
    'bior2.2': 'Creversible=yes',
    'haar': 'Catk=2 Kkernels:I2=R2X2',
    'bior2.6': 'Catk=2 Kextension:I2=SYM Kreversible:I2=yes "Ksteps:I2={4,-1,4,8},{4,-2,4,8}" '
    'Kcoeffs:I2=0.0625,-0.5625,-0.5625,0.0625,-0.0625,0.3125,0.3125,-0.0625'
}
DWT_COMPS = ['', 'H(-)', 'V(-)', 'B(-:-:-)']


def make_decomp(decomp):
    return f"Clevels=5 Cdecomp={','.join([*map(lambda x: DWT_COMPS[x], decomp),])}"


def make_commands(dwts_dict):
    commands = dict()
    for file, dwts in dwts_dict.items():
        cmds = []
        for filt, decomp in dwts.items():
            cmd = f"{BASE} -i {file} {OUT} {FILTERS[filt]}"
            cmds.append(f"{cmd} {make_decomp(decomp)}")
            if decomp != [3, 3, 3, 3, 3]:
                cmds.append(f"{cmd}")
        commands[file] = cmds
    return commands


def main():
    dwts_dict = read_dwts_dict()
    commands = make_commands(dwts_dict)
    test_kdu(commands)
    print("")


if __name__ == '__main__':
    main()
