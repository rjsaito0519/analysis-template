#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${project_dir}"

if [[ -f config/project.env ]]; then
    # shellcheck disable=SC1091
    source config/project.env
fi

if ! command -v root-config >/dev/null 2>&1; then
    echo "error: ROOT is not active. Source thisroot.sh or set ROOT_DIR." >&2
    exit 1
fi

cmake --preset default
cmake --build --preset default --parallel

