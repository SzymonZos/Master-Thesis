#!/usr/bin/env bash
set -Eeuo pipefail

IMG="./img/lena.png"
./build/bin/jpeg2000 -dq --opencv "$IMG"
./build/bin/jpeg2000 --opencv-queue "$IMG"
./build/bin/jpeg2000 --opencv-queue-parallel "$IMG"
./build/bin/jpeg2000 --opencv-queue-entropy "$IMG"
./build/bin/jpeg2000 --opencv-queue-parallel-rgb "$IMG"
