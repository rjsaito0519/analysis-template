#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${project_dir}"

usage() {
    echo "Usage: ./init-project.sh PROJECT_NAME [--yes]"
    echo "PROJECT_NAME must start with a letter and contain only letters, numbers, _ or -."
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
    usage
    exit 2
fi

project_name="$1"
auto_yes="${2:-}"

if [[ ! "${project_name}" =~ ^[A-Za-z][A-Za-z0-9_-]*$ ]]; then
    echo "error: invalid project name: ${project_name}" >&2
    usage >&2
    exit 2
fi

if [[ -n "${auto_yes}" && "${auto_yes}" != "--yes" ]]; then
    usage >&2
    exit 2
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "error: python3 is required to initialize the project" >&2
    exit 1
fi

for required_file in \
    CMakeLists.txt \
    README.md \
    AGENTS.md \
    src/example_analysis.cpp \
    src/rdf_analysis.cpp \
    scripts/run_analysis.py \
    scripts/plot_result.py
do
    if [[ ! -f "${required_file}" ]]; then
        echo "error: expected template file is missing: ${required_file}" >&2
        echo "The repository may already be initialized or modified; no files were changed." >&2
        exit 1
    fi
done

python3 - <<'PY'
from pathlib import Path

cmake = Path("CMakeLists.txt").read_text(encoding="utf-8")
expected_lines = (
    "project(MyAnalysis VERSION 0.1.0 LANGUAGES CXX)",
    "add_analysis_executable(example_analysis src/example_analysis.cpp)",
    "add_analysis_executable(rdf_analysis src/rdf_analysis.cpp)",
)

missing = [line for line in expected_lines if cmake.count(line) != 1]
if missing:
    details = "\n  - ".join(missing)
    raise SystemExit(
        "error: CMakeLists.txt does not match the uninitialized template:\n"
        f"  - {details}\n"
        "The repository may already be initialized or modified; no files were changed."
    )
PY

cat <<EOF
This will initialize the repository as '${project_name}'.

Changes:
  - replace the template README with a minimal project README
  - set the CMake project name to ${project_name}
  - remove the sample C++ programs and their CMake targets
  - remove the sample Python runner and plotting script
  - remove this initialization script
  - remove only CMake build products via ./clean.sh

It will not modify data/, results/, Git staging, commits, or remotes.
EOF

if [[ "${auto_yes}" != "--yes" ]]; then
    read -r -p "Continue? [y/N] " reply
    case "${reply}" in
        y|Y|yes|YES) ;;
        *)
            echo "Cancelled."
            exit 0
            ;;
    esac
fi

python3 - "${project_name}" <<'PY'
from pathlib import Path
import sys

project_name = sys.argv[1]
path = Path("CMakeLists.txt")
cmake = path.read_text(encoding="utf-8")
cmake = cmake.replace(
    "project(MyAnalysis VERSION 0.1.0 LANGUAGES CXX)",
    f"project({project_name} VERSION 0.1.0 LANGUAGES CXX)",
)
for target in (
    "add_analysis_executable(example_analysis src/example_analysis.cpp)\n",
    "add_analysis_executable(rdf_analysis src/rdf_analysis.cpp)\n",
):
    cmake = cmake.replace(target, "")
path.write_text(cmake, encoding="utf-8")
PY

rm -f \
    src/example_analysis.cpp \
    src/rdf_analysis.cpp \
    scripts/run_analysis.py \
    scripts/plot_result.py

mkdir -p src
touch src/.gitkeep
rmdir scripts 2>/dev/null || true

cat > README.md <<EOF
# ${project_name}

C++17、CERN ROOT、Pythonを使う解析プロジェクトです。

## 必要なもの

- Bashが使えるUnix系環境（Linux、macOS、WSLなど）
- CMake 3.22 以上
- C++17対応コンパイラ
- CERN ROOT 6.24 以上
- Python 3.9 以上

## ビルド

\`src/\`にC++ソースを追加し、\`CMakeLists.txt\`へtargetを登録してから実行します。

\`\`\`bash
./build.sh
\`\`\`

CMake生成物だけを削除する場合は次を実行します。

\`\`\`bash
./clean.sh
\`\`\`

プロジェクトの構成、解析上の注意、文書管理の方針は[docs/README.md](docs/README.md)を参照してください。AI向けの共通ルールは[AGENTS.md](AGENTS.md)にあります。
EOF

./clean.sh
rm -f "${project_dir}/init-project.sh"

cat <<EOF

Initialized '${project_name}'.
Review the changes with 'git status' and 'git diff'.
Stage, commit, and push only after you are satisfied with the result.
EOF
