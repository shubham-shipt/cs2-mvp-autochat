#!/usr/bin/env bash
set -euo pipefail

# Resolve repo root
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

CONFIG="${1:-config/config.json}"

echo "[run-cli] Using config: $CONFIG"

# Build with Makefile if present
if [[ -f Makefile ]]; then
  echo "[run-cli] Building via Makefile..."
  make -j"$(nproc)"
fi

# Try to find a CLI binary
find_cli_bin() {
  # Prefer bin/ first
  if [[ -d bin ]]; then
    BIN="$(find bin -maxdepth 1 -type f -perm -111 2>/dev/null | grep -Ei '/(cli|main_cli|cs2.*cli|autochat.*cli)$' || true)"
    if [[ -n "${BIN:-}" ]]; then
      echo "$BIN" | head -n1
      return 0
    fi
  fi

  # Fallback: search repo for an executable with cli in name
  BIN="$(find . -type f -perm -111 2>/dev/null | grep -Ei '/(cli|main_cli)(\\.exe)?$' || true)"
  if [[ -n "${BIN:-}" ]]; then
    echo "$BIN" | head -n1
    return 0
  fi

  return 1
}

CLI_BIN="$(find_cli_bin || true)"
if [[ -z "${CLI_BIN:-}" ]]; then
  echo "[run-cli] ERROR: Could not find CLI binary. Ensure your Makefile builds a CLI target into ./bin/"
  echo "           Expected something like: ./bin/cs2-autochat-cli or ./bin/main_cli"
  exit 1
fi

echo "[run-cli] Running: $CLI_BIN $CONFIG"
exec "$CLI_BIN" "$CONFIG"