#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build --target install -- -j "$(nproc)"
pytest ./test/test.py
