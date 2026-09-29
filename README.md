# Analysis template

C++17、CERN ROOT、Pythonを使う解析のためのシンプルなテンプレートです。

## 構成

```text
analysis/src/       物理解析プログラム（1ファイル = 1実行ファイルを推奨）
calibration/src/    較正プログラム
common/include/     解析・較正で共有するヘッダー
common/src/         解析・較正で共有する実装
python/src/         再利用可能なPythonパッケージ（srcレイアウト）
python/tests/       Python側の単体テスト
config/             ローカル環境設定のひな形
scripts/            初期化、ビルド、実行の補助
tests/              ROOTファイルを必要としない小さなテスト
data/               入力データやそのシンボリックリンク（Git管理外）
results/            ROOT/PDF/画像などの成果物（Git管理外）
scratch/            一時ファイル（Git管理外）
```

## 必要なもの

- Linux または WSL
- CMake 3.22 以上
- C++17 対応コンパイラ
- CERN ROOT 6.24 以上（`thisroot.sh` 適用済み、または `ROOT_DIR` 設定済み）
- Python 3.9 以上

## 最初の実行

```bash
bash scripts/bootstrap.sh
${EDITOR:-vi} config/project.env
bash scripts/build.sh
ctest --preset default
python3 scripts/run_analysis.py 42 --entries 20000
```

結果は `results/run00042/example_analysis.root` に作られます。入力なしで乱数からヒストグラムを生成するため、環境確認にも使えます。

ビルドを最初からやり直す場合は、データや結果を残したままCMake生成物だけを削除できます。

```bash
bash scripts/clean.sh
bash scripts/build.sh
```

較正側の例は次のように実行します。

```bash
source config/project.env
./.build/bin/example_calibration 42 1.025
```

実 ROOT ファイルの TTree を `RDataFrame` で読む例もあります。

```bash
./.build/bin/rdf_analysis 42 input.root tree_name branch_name results/run00042/rdf.root
```

Pythonは現在のシェルで利用できる `python3` とインストール済みパッケージをそのまま使います。

```bash
python3 python/src/my_analysis/plot_result.py \
  results/run00042/example_analysis.root \
  --output results/run00042/example_analysis.png
PYTHONPATH=python/src python3 -m pytest
```

## 新しい解析を追加する

1. `analysis/src/example_analysis.cpp` をコピーして解析を書く。
2. `CMakeLists.txt` に `add_analysis_executable(実行名 ソース)` を1行追加する。
3. `bash scripts/build.sh` で再ビルドする。

共有処理は `common/include/analysis/` と `common/src/` に置き、実行ファイルの `main` は引数処理と処理手順の組み立てに留めると保守しやすくなります。

## パス設定

`config/project.env` は各マシン専用でGitには入りません。利用可能な変数は次の4つです。

- `ANALYSIS_DATA_DIR`: 入力データ置き場
- `ANALYSIS_OUTPUT_DIR`: 永続的な解析結果
- `ANALYSIS_PARAM_DIR`: パラメータ・較正定数
- `ANALYSIS_SCRATCH_DIR`: 大きな一時ファイル

C++ 側はこの環境変数を読み、未設定ならプロジェクト直下の `data / results / param / scratch` を使います。実行時引数で出力先を明示すれば、その値が最優先です。

## 実データへつなぐときの目安

- 実験のデコーダや DST ライブラリは `analysis_core` とは別の CMake target にする。
- run ごとの入力解決は `RunPaths` に集約し、個々の解析に絶対パスを書かない。
- 較正定数はソースコードではなく `param/` の版管理可能なテキストへ置く。
- 大きな ROOT ファイルは Git に入れず、`data/` から共有ストレージへリンクする。
- 解析条件、入力ファイル、Git commit、ROOT version を出力 ROOT の metadata に残す。

設計上の簡単な指針は [docs/best-practices.md](docs/best-practices.md) にまとめています。
