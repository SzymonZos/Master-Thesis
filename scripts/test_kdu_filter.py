import os
import re
import subprocess


BASE = "./bin/Linux-x86-64-gcc/kdu_compress"


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


def test_kdu(path):
    cmds = [
        'Creversible=yes',
        'Catk=2 Kkernels:I2=R2X2',
        'Catk=2 Kextension:I2=SYM Kreversible:I2=yes "Ksteps:I2={4,-1,4,8},{4,-2,4,8}" '
        'Kcoeffs:I2=0.0625,-0.5625,-0.5625,0.0625,-0.0625,0.3125,0.3125,-0.0625'
    ]

    results = dict()
    for cmd in cmds:
        results[cmd] = run_cmd(path, cmd)
    with open('kdu_logs.txt', 'a') as f:
        f.write(f"{path}: {results}\n")
    min_bitrate = min(results, key=results.get)
    print(f"{min_bitrate}: {results[min_bitrate]}")


def main():
    for file in os.scandir('./images_ppm'):
        print(file.path)
        test_kdu(file.path)
        print("")


if __name__ == '__main__':
    main()
