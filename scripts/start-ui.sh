#!/usr/bin/env bash
set -euo pipefail

# Resolve repo root
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

# Build with Makefile if present
if [[ -f Makefile ]]; then
  echo "[start-ui] Building via Makefile..."
  make -j"$(nproc)"
fi

# Try to find a UI binary (GTK)
find_ui_bin() {
  # Prefer bin/ first
  if [[ -d bin ]]; then
    BIN="$(find bin -maxdepth 1 -type f -perm -111 2>/dev/null | grep -Ei '/(ui|main_ui|gtk|cs2.*ui|autochat.*ui)$' || true)"
    if [[ -n "${BIN:-}" ]]; then
      echo "$BIN" | head -n1
      return 0
    fi
  fi

  # Fallback: search repo for an executable with ui/gtk in name
  BIN="$(find . -type f -perm -111 2>/dev/null | grep -Ei '/(ui|main_ui|gtk)(\\.exe)?$' || true)"
  if [[ -n "${BIN:-}" ]]; then
    echo "$BIN" | head -n1
    return 0
  fi

  return 1
}

UI_BIN="$(find_ui_bin || true)"
if [[ -z "${UI_BIN:-}" ]]; then
  echo "[start-ui] ERROR: Could not find UI binary. Ensure your Makefile builds a UI target into ./bin/"
  echo "            Expected something like: ./bin/cs2-autochat-ui or ./bin/main_ui"
  exit 1
fi

echo "[start-ui] Running: $UI_BIN"
exec "$UI_BIN"