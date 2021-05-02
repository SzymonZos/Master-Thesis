#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build -- -j "$(nproc)"
./build/jpeg2000
