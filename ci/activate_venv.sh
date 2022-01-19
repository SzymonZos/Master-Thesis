#!/usr/bin/env bash
set -Eeuo pipefail

pip3 install --upgrade pip
python3 -m venv venv
. ./venv/bin/activate
pip3 install -r requirements.txt

# https://github.com/travis-ci/travis-ci/issues/5434
set +u
