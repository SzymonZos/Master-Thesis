#!/usr/bin/env bash
set -Eeuo pipefail

case "$1" in
  *ubuntu*)
    sudo apt-get install -y libopencv-dev
    ;;
  *macos*)
    brew install tbb opencv
    ;;
esac
