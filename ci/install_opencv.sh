#!/usr/bin/env bash
set -Eeuo pipefail

case "$1" in
  *ubuntu*)
    sudo apt-get install -y libopencv-dev
    ;;
  *macos*)
    brew install opencv
    ln -s /usr/local/Cellar/tbb/2021.5.0/include/tbb /usr/local/include/tbb
    ;;
esac
