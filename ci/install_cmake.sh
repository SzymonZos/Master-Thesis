#!/usr/bin/env bash
set -Eeuo pipefail

case "$MATRIX_EVAL" in
  *gcc*)
    python3 -m pip install -U --user pip
    ;;
  *clang*)
    wget https://bootstrap.pypa.io/get-pip.py
    sudo python3.8 get-pip.py
    ;;
esac
sudo apt purge --auto-remove cmake
pip3.8 install -U cmake
