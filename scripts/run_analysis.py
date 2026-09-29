#!/usr/bin/env python3
"""Run one analysis target with predictable paths and no shell expansion."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import subprocess
import sys


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def load_env_file(path: Path) -> dict[str, str]:
    environment = os.environ.copy()
    if not path.exists():
        return environment

    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if line.startswith("export "):
            line = line[7:]
        key, separator, value = line.partition("=")
        if not separator:
            raise ValueError(f"invalid environment line: {raw_line}")
        value = value.strip().strip('"').strip("'")
        value = value.replace("${PWD}", str(PROJECT_ROOT))
        environment[key.strip()] = value
    return environment


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the example ROOT analysis")
    parser.add_argument("run", type=int, help="non-negative run number")
    parser.add_argument("--entries", type=int, default=10_000)
    parser.add_argument("--target", default="example_analysis")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()

    executable = PROJECT_ROOT / ".build" / "bin" / args.target
    if not executable.is_file():
        parser.error(f"{executable} does not exist; run scripts/build.sh first")

    command = [str(executable), str(args.run), "--entries", str(args.entries)]
    if args.output is not None:
        command.extend(["--output", str(args.output.resolve())])

    environment = load_env_file(PROJECT_ROOT / "config" / "project.env")
    completed = subprocess.run(
        command,
        cwd=PROJECT_ROOT,
        env=environment,
        check=False,
    )
    return completed.returncode


if __name__ == "__main__":
    sys.exit(main())
