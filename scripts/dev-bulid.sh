#!/usr/bin/env bash
set -euo pipefail

# Quick developer build helper.
# - Detects Makefile and builds
# - Shows where binaries landed
# - Prints hints for running scripts

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

if [[ ! -f Makefile ]]; then
  echo "[dev-build] No Makefile found at $ROOT."
  echo "            Please add build rules or integrate CMake/Make."
  exit 1
fi

echo "[dev-build] Building with Makefile..."
make -j"$(nproc)"

echo
echo "[dev-build] Build complete. Executables found:"
if [[ -d bin ]]; then
  find bin -maxdepth 1 -type f -perm -111 -printf "  %p\n" || true
else
  echo "  (no ./bin directory)"
fi

echo
echo "[dev-build] Tips:"
echo "  - Run CLI:  scripts/run-cli.sh [config/config.json]"
echo "  - Start UI: scripts/start-ui.sh"