#!/usr/bin/env bash
set -Eeuo pipefail

python3.8 -m venv venv
. ./venv/bin/activate
pip install -r requirements.txt

# https://github.com/travis-ci/travis-ci/issues/5434
set +u
