#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build -- -j "$(nproc)"
./build/src/jpeg2000 -dq
