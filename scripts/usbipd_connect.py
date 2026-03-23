#!/usr/bin/env python3
"""
scripts/usbipd_connect.py

Auto-detect the NXP S32K144EVB OpenSDA/CMSIS-DAP debug probe via usbipd,
bind it (if not already shared), and attach it to WSL2.

Run from WSL:  python3 scripts/usbipd_connect.py
Or via make:   make usbipd-connect
"""

import re
import subprocess
import sys

# Identifiers for the S32K144EVB OpenSDA probe (PEMicro CMSIS-DAP)
PROBE_PATTERNS = [
    "OpenSDA",
    "1357:0089",  # PEMicro VID:PID
]


def run(cmd: list[str], check: bool = False) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, capture_output=True, text=True, check=check)


def find_probe() -> tuple[str | None, str | None]:
    """Return (busid, description) of the first matching OpenSDA device."""
    try:
        result = run(["usbipd.exe", "list"])
    except FileNotFoundError:
        print("ERROR: usbipd.exe not found.")
        print("       Install usbipd-win on Windows: https://github.com/dorssel/usbipd-win")
        sys.exit(1)

    for line in result.stdout.splitlines():
        if any(p in line for p in PROBE_PATTERNS):
            match = re.match(r"^\s*(\d+-\d+)", line)
            if match:
                return match.group(1), line.strip()

    return None, None


def bind(busid: str) -> None:
    """Bind the device so usbipd can share it.

    Tries unprivileged first. On access-denied (exit 3) re-runs via
    PowerShell Start-Process -Verb RunAs which triggers a Windows UAC prompt.
    Idempotent — already-shared is not an error.
    """
    print(f"  Binding {busid}...")
    result = run(["usbipd.exe", "bind", "--busid", busid])
    stderr = result.stderr.strip().lower()

    if result.returncode == 0 or "already" in stderr or "shared" in stderr:
        print("  OK (device is shared)")
        return

    if result.returncode == 3 or "access denied" in stderr or "administrator" in stderr:
        print("  Requires admin — launching elevated bind via UAC prompt...")
        elevated = run([
            "powershell.exe", "-Command",
            f"Start-Process -FilePath usbipd -ArgumentList 'bind --busid {busid}' -Verb RunAs -Wait",
        ])
        if elevated.returncode == 0:
            print("  OK (elevated bind succeeded)")
        else:
            print(f"  WARNING: elevated bind returned exit {elevated.returncode}")
            print("  Continuing — attach may still work if device was already shared.")
    else:
        print(f"  WARNING: bind returned exit {result.returncode}: {result.stderr.strip()}")


def attach(busid: str) -> None:
    """Attach the device to WSL2."""
    print(f"  Attaching {busid} to WSL2...")
    result = run(["usbipd.exe", "attach", "--wsl", "--busid", busid])

    if result.returncode != 0:
        print(f"ERROR: attach failed (exit {result.returncode})")
        if result.stderr.strip():
            print(f"       {result.stderr.strip()}")
        sys.exit(1)

    print("  OK")


def main() -> None:
    print("=" * 60)
    print("  usbipd-connect — S32K144EVB OpenSDA probe")
    print("=" * 60)

    print("\nScanning for OpenSDA probe...")
    busid, desc = find_probe()

    if not busid:
        print("\nERROR: OpenSDA device not found.")
        print("  1. Is the S32K144EVB plugged in via USB?")
        print("  2. Confirm it appears in PowerShell:  usbipd list")
        sys.exit(1)

    print(f"  Found : {desc}")
    print(f"  busid : {busid}\n")

    bind(busid)
    attach(busid)

    print("\nDone. Verify in WSL with:  lsusb")
    print("=" * 60)


if __name__ == "__main__":
    main()
