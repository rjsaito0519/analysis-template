#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${project_dir}"

mkdir -p data results param scratch

if [[ ! -e config/project.env ]]; then
    cp config/project.env.example config/project.env
    echo "Created config/project.env; edit it for this machine."
else
    echo "config/project.env already exists; leaving it unchanged."
fi

if [[ ! -e config/site.sh ]]; then
    cp config/site.sh.example config/site.sh
    echo "Created config/site.sh; set ROOT/Garfield paths if needed."
else
    echo "config/site.sh already exists; leaving it unchanged."
fi

echo "Project directories are ready."
