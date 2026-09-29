#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${project_dir}"

if [[ -f config/site.sh ]]; then
    # shellcheck disable=SC1091
    source config/site.sh
fi

require_command() {
    local name="$1"
    if ! command -v "${name}" >/dev/null 2>&1; then
        echo "missing: ${name}" >&2
        return 1
    fi
    printf '%-12s %s\n' "${name}" "$(command -v "${name}")"
}

require_command git
require_command cmake
require_command c++
require_command python3
require_command root-config

echo
printf '%-12s %s\n' "ROOT" "$(root-config --version)"
printf '%-12s %s\n' "ROOT prefix" "$(root-config --prefix)"
printf '%-12s %s\n' "CMake" "$(cmake --version | head -n 1)"
printf '%-12s %s\n' "C++" "$(c++ --version | head -n 1)"
printf '%-12s %s\n' "Python" "$(python3 --version)"

if [[ -n "${GARFIELD_HOME:-}" ]]; then
    echo
    printf '%-12s %s\n' "Garfield++" "${GARFIELD_HOME}"
    if [[ ! -e "${GARFIELD_HOME}" ]]; then
        echo "warning: GARFIELD_HOME does not exist" >&2
        exit 1
    fi
fi

echo
echo "Environment check passed."
