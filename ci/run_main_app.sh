#!/usr/bin/env bash
set -Eeuo pipefail

IMG="./img/lena.png"
cmake --build build --target jpeg2000 -- -j "$(nproc)"
./build/bin/jpeg2000 -dq --opencv "$IMG"
./build/bin/jpeg2000 --opencv-queue "$IMG"
./build/bin/jpeg2000 --opencv-queue-parallel "$IMG"
./build/bin/jpeg2000 --opencv-queue-entropy "$IMG"
./build/bin/jpeg2000 --opencv-queue-parallel-rgb "$IMG"
