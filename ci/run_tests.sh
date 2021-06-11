#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build --target jpeg2000_test install -- -j "$(nproc)"
pytest ./test/test.py
