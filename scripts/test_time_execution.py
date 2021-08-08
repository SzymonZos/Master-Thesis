import os
import re
import subprocess
from pathlib import Path


BASE = Path("./cmake-build-release-gcc/bin/jpeg2000.exe")


def run_cmd(cmd):
    print(f"{BASE} {cmd}")
    res = subprocess.run(f"{BASE} {cmd}", stdout=subprocess.PIPE, shell=True)


def test_time_exec(path):
    run_cmd(f"--check-entropy-exec-policies {path}")
    for i in range(1, 21):
        run_cmd(f"--threads {i} --check-entropy-threads {path}")


def main():
    for file in os.scandir('./img/FINAL'):
        print(file.path)
        test_time_exec(file.path)
        print("")


if __name__ == '__main__':
    main()
