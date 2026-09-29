# Analysis template

C++17、CERN ROOT、Pythonを使う解析のためのシンプルなテンプレートです。

## このテンプレートから新しいリポジトリを作る

### 1. GitHub上で作成する

1. GitHubでこの`analysis-template`リポジトリを開く。
2. ファイル一覧の上にある **Use this template** を押す。
3. **Create a new repository** を選ぶ。
4. **Owner**、新しいリポジトリ名、必要ならdescriptionを入力する。
5. **Public**または**Private**を選ぶ。
6. **Include all branches**は、通常は選択せずdefault branchだけを使う。
7. **Create repository from template**を押す。

これで、templateのファイルを初期状態として持つ、独立した新しいリポジトリが作られます。forkではないため、templateへ変更を返すためのリポジトリではなく、新しい解析の履歴として管理できます。

**Use this template**が表示されない場合は、このリポジトリの **Settings** → **General** を開き、**Template repository**を有効にします。

GitHub公式の説明: [Creating a repository from a template](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template)

### 2. 利用する環境へcloneして初期化する

作成したリポジトリの **Code** からSSHまたはHTTPSのURLをコピーし、解析に使用する環境でcloneします。

```bash
git clone REPOSITORY_URL
cd YOUR_REPOSITORY
./init-project.sh PROJECT_NAME
```

たとえばSSHを使う場合、`REPOSITORY_URL`は`git@github.com:YOUR_ACCOUNT/YOUR_REPOSITORY.git`のようになります。

`PROJECT_NAME`はCMakeのproject名にも使われるため、英字で始まり、英数字・`_`・`-`だけを使います。

初期化前に、スクリプトが行う変更の一覧と確認が表示されます。初期化すると次が行われます。

- このREADMEを、新しいプロジェクト用の最小READMEへ置き換える
- CMakeのproject名を変更する
- 動作確認用のC++・PythonサンプルとCMake targetを削除する
- `src/.gitkeep`を残し、空の`src/`を維持する
- CMake生成物を削除する
- 初期化後は不要になる`init-project.sh`自身を削除する

`data/`、`results/`、Gitのstage・commit・remoteには触れません。実行後に`git status`と`git diff`で内容を確認し、問題がなければ自分でcommit・pushします。

サンプルコードを参考として残したい場合は、初期化前に内容を確認し、必要なファイルを別名で保存してから実行してください。

## 構成

```text
src/                C++の解析プログラムと共有実装
scripts/            Pythonスクリプト
data/               入力データやそのシンボリックリンク（Git管理外）
results/            ROOT/PDF/画像などの成果物（Git管理外）
docs/               解析テーマごとに整理する文書
```

## 必要なもの

- Bashが使えるUnix系環境（Linux、macOS、WSLなど）
- CMake 3.22 以上
- C++17 対応コンパイラ
- CERN ROOT 6.24 以上（`thisroot.sh` 適用済み、または `ROOT_DIR` 設定済み）
- Python 3.9 以上

## 最初の実行

```bash
./build.sh
python3 scripts/run_analysis.py --entries 20000
```

結果は `results/example_analysis.root` に作られます。入力なしで乱数からヒストグラムを生成するため、環境確認にも使えます。

ビルドを最初からやり直す場合は、データや結果を残したままCMake生成物だけを削除できます。

```bash
./clean.sh
./build.sh
```

実 ROOT ファイルの TTree を `RDataFrame` で読む例もあります。

```bash
./.build/bin/rdf_analysis input.root tree_name branch_name results/rdf.root
```

Pythonは現在のシェルで利用できる `python3` とインストール済みパッケージをそのまま使います。

```bash
python3 scripts/plot_result.py \
  results/example_analysis.root \
  --output results/example_analysis.png
```

## 新しい解析を追加する

1. `src/example_analysis.cpp` をコピーして解析を書く。
2. `CMakeLists.txt` に `add_analysis_executable(実行名 ソース)` を1行追加する。
3. `./build.sh` で再ビルドする。

共有処理も `src/` に置きます。規模が大きくなった場合にだけ、`include/`や`src/analysis`、`src/calibration`などへ分割してください。

入力と出力はプロジェクト直下の `data / results` を使います。別の出力先が必要な場合は実行時引数で指定します。

## 実データへつなぐときの目安

- 実験のデコーダやDSTライブラリが大きくなったら、`analysis_core`とは別のCMake targetにする。
- 大きな ROOT ファイルは Git に入れず、`data/` から共有ストレージへリンクする。
- 解析条件、入力ファイル、Git commit、ROOT version を出力 ROOT の metadata に残す。

詳しい構成と設計上の指針は [docs/README.md](docs/README.md) にまとめています。AIツール向けの共通ルールは [AGENTS.md](AGENTS.md) を入口にしています。
