#!/usr/bin/env python3
"""Run one analysis target with predictable paths and no shell expansion."""

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the example ROOT analysis")
    parser.add_argument("run", type=int, help="non-negative run number")
    parser.add_argument("--entries", type=int, default=10_000)
    parser.add_argument("--target", default="example_analysis")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()

    executable = PROJECT_ROOT / ".build" / "bin" / args.target
    if not executable.is_file():
        parser.error(f"{executable} does not exist; run build.sh first")

    command = [str(executable), str(args.run), "--entries", str(args.entries)]
    if args.output is not None:
        command.extend(["--output", str(args.output.resolve())])

    completed = subprocess.run(
        command,
        cwd=PROJECT_ROOT,
        check=False,
    )
    return completed.returncode


if __name__ == "__main__":
    sys.exit(main())
