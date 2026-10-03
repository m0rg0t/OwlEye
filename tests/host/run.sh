#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")/../.."
build="$(mktemp -d)"
trap 'rm -rf "$build"' EXIT HUP INT TERM
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -pedantic   -fsanitize=undefined -fno-sanitize-recover=all   -I tests/host tests/host/sketch_test.cpp -o "$build/sketch-test"
"$build/sketch-test"
