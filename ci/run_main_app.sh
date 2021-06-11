#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build --target jpeg2000 -- -j "$(nproc)"
./build/src/jpeg2000 -dq
