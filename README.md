# Analysis template

C++17、CERN ROOT、Pythonを使う解析のためのシンプルなテンプレートです。

## 構成

```text
src/                C++の解析プログラムと共有実装
include/            C++の共有ヘッダー
scripts/            ビルド補助とPythonスクリプト
tests/              C++の小さなテスト
data/               入力データやそのシンボリックリンク（Git管理外）
results/            ROOT/PDF/画像などの成果物（Git管理外）
```

## 必要なもの

- Linux または WSL
- CMake 3.22 以上
- C++17 対応コンパイラ
- CERN ROOT 6.24 以上（`thisroot.sh` 適用済み、または `ROOT_DIR` 設定済み）
- Python 3.9 以上

## 最初の実行

```bash
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

実 ROOT ファイルの TTree を `RDataFrame` で読む例もあります。

```bash
./.build/bin/rdf_analysis 42 input.root tree_name branch_name results/run00042/rdf.root
```

Pythonは現在のシェルで利用できる `python3` とインストール済みパッケージをそのまま使います。

```bash
python3 scripts/plot_result.py \
  results/run00042/example_analysis.root \
  --output results/run00042/example_analysis.png
```

## 新しい解析を追加する

1. `src/example_analysis.cpp` をコピーして解析を書く。
2. `CMakeLists.txt` に `add_analysis_executable(実行名 ソース)` を1行追加する。
3. `bash scripts/build.sh` で再ビルドする。

共有処理は `include/` と `src/` に置きます。規模が大きくなった場合にだけ、`src/analysis`、`src/calibration`などへ分割してください。

入力と出力はプロジェクト直下の `data / results` を使います。別の出力先が必要な場合は実行時引数で指定します。

## 実データへつなぐときの目安

- 実験のデコーダやDSTライブラリが大きくなったら、`analysis_core`とは別のCMake targetにする。
- run ごとの入力解決は `RunPaths` に集約し、個々の解析に絶対パスを書かない。
- 大きな ROOT ファイルは Git に入れず、`data/` から共有ストレージへリンクする。
- 解析条件、入力ファイル、Git commit、ROOT version を出力 ROOT の metadata に残す。

設計上の簡単な指針は [docs/best-practices.md](docs/best-practices.md) にまとめています。
