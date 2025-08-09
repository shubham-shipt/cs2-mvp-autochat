# bin/

This directory holds launchers and (optionally) compiled binaries.

Included files:
- `cs2-autochat-cli` — Unix launcher for the CLI (calls `scripts/run-cli.sh`)
- `cs2-autochat-ui` — Unix launcher for the GTK UI (calls `scripts/start-ui.sh`)
- `cs2-autochat-cli.bat` — Windows launcher for the CLI (uses Git Bash/WSL)
- `cs2-autochat-ui.bat` — Windows launcher for the GTK UI (uses Git Bash/WSL)
- `.gitkeep` — keeps this directory tracked when empty

Make the Unix launchers executable:
```bash
chmod +x bin/cs2-autochat-cli bin/cs2-autochat-ui
```

Run:
```bash
# CLI (uses default config/config.json unless a path is passed)
./bin/cs2-autochat-cli
./bin/cs2-autochat-cli path/to/your.json

# GTK UI
./bin/cs2-autochat-ui
```

Windows (PowerShell or cmd):
```bat
bin\cs2-autochat-cli.bat
bin\cs2-autochat-ui.bat
```