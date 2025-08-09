#!/usr/bin/env bash
set -euo pipefail

# Resolve repo root
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
exec "$ROOT/scripts/start-ui.sh"