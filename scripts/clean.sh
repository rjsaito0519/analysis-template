#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Only remove out-of-source CMake products. Data, parameters, and results are
# deliberately preserved.
for build_dir in "${project_dir}/.build" "${project_dir}/.build-debug"; do
    if [[ -d "${build_dir}" ]]; then
        cmake -E remove_directory "${build_dir}"
        echo "Removed ${build_dir}"
    fi
done

echo "Build products are clean."

