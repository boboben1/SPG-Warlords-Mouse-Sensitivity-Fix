# SPG Warlords Mouse Sensitivity Fix
Prevents SPG from changing your mouse sensitivity.
Camera sensitivity still works as intended.

# Usage
1. Start steam.
2. Run SPGNoMouseChange.exe

# How it works
This tool prevents the launcher and game from calling SystemParametersInfoW with SPI_SETMOUSESPEED (0x71)