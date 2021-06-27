#!/usr/bin/env bash
set -Eeuo pipefail

cmake --build build --target jpeg2000 -- -j "$(nproc)"
./build/bin/jpeg2000 -dq --opencv ./img/lena.png
