#!/usr/bin/env bash
set -Eeuo pipefail

case "$1" in
  *gcc*)
    CC="$(which gcc-11)" || CC="$(which gcc-10)"
    CXX="$(which g++-11)" || CXX="$(which g++-10)"
    ;;
  *clang*)
    CC="$(which clang-12)" || CC="$(brew --prefix llvm)/bin/clang"
    CXX="$(which clang++-12)" || CXX="$(brew --prefix llvm)/bin/clang++"
    ;;
esac
echo "CC=$CC" >> "$GITHUB_ENV"
echo "CXX=$CXX" >> "$GITHUB_ENV"
