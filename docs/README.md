# Documentation

このディレクトリには、READMEだけでは収まらない解析の前提、設計判断、データ形式、手順、検証結果を置きます。

## 文書の整理

文書は追加した時点から、実際の解析テーマや対象に沿って整理します。`project/`や`development/`のような汎用カテゴリを先に固定するのではなく、そのプロジェクトに存在する検出器、物理解析、較正、geometry、simulation、manualなどを分類の単位にします。

新しいトピックを追加するときは次の形にします。

```text
docs/
├── README.md
└── <topic>/
    ├── README.md
    └── ...
```

- `docs/README.md`は全体の索引とする。
- 各トピックの`README.md`には、現在の状態、最初に読む文書、古い文書の扱いを書く。
- 新しい文書を追加、移動、archiveしたら、トピックと全体の両方の`README.md`を更新する。
- 同じ内容の文書を増やさず、既存文書を更新できないか先に確認する。
- 時点に依存する結果や状況には日付を書く。
- 古い記録は黙って書き換えず、現状との差分を追記するか、置き換え先を示してarchiveする。

このリポジトリに具体的な解析テーマの文書がまだない間は、空のトピックディレクトリを作りません。最初の文書を置く時点から、その内容に合う分類を用意します。

## プロジェクト構成の方針

この構成は、C++17、CERN ROOT、Pythonを使う解析を対象にします。特定の実験、検出器、run番号、計算機、共有ストレージは共通の前提にしません。

- C++の実行プログラムと小さな共有処理は`src/`に置く。
- 実行補助、確認、描画などのPythonスクリプトは`scripts/`に置く。
- 入力データや共有データへのリンクは`data/`、生成物は`results/`に置き、中身をGit管理しない。
- `include/`、共有ライブラリ、`analysis/`、`calibration/`などは、実際に分割する理由が生じたときに追加する。
- run番号は必要な解析でだけ導入し、ファイル名やCLIの共通前提にしない。

## 解析を始めるときに記録すること

- 入力ROOTファイル、TTree、必要なbranch
- 数値の単位と座標系
- 選別条件と、その物理的な理由
- 出力するTTree、histogram、表、図の名前と意味
- 小さいサンプルでの検証方法
- 本番結果を再現するために残すmetadata

ROOTのbranch名、histogram名、object名は、下流のmacroや過去データとのインターフェースです。変更が必要なら、新旧形式の扱いを決めてから実施します。

## 実装上の指針

### C++とCMake

- build要件はグローバルなflagではなくCMake targetに設定する。
- 実行プログラムは明示的に登録し、試作中の`.cpp`をglobで自動追加しない。
- RAII、値、`std::filesystem`、smart pointerを優先し、所有するraw pointerを避ける。
- 生成ファイルは`.build*`以下に置き、sourceと混ぜない。
- 特定環境の正確なversionではなく最低versionを指定し、本番結果には実際のversionを記録する。

### ROOT

- 入出力`TFile`、TTree、必要なbranchをevent loop前に確認する。
- 新しいcolumnar analysisでは`ROOT::RDataFrame`を検討し、actionをまとめてbookする。
- ROOT objectのlifetimeとdirectory ownershipを明示的に扱う。
- implicit multithreadingは、callbackと外部libraryのthread safetyを確認してから有効にする。
- plottingとevent processingを分け、batch処理をdisplay serverに依存させない。

### Python

- 小さい補助scriptは`scripts/`に置き、複数箇所で共有する必要が生じてからpackage化する。
- subprocessはcommand文字列ではなく引数listで呼び出し、`shell=True`を避ける。
- `pathlib.Path`、context manager、type hint、小さな関数を適切に使う。
- 同じ物理selectionをC++とPythonへ別々に実装しない。

## 再現性と物理結果

- raw data、生成ROOT file、credential、machine固有の絶対pathをcommitしない。
- 小さいcalibration/configuration入力はversion管理し、使用したfileまたはhashを記録する。
- 本番結果にはGit commit、dirty状態、command line、ROOT version、入力、主要cut、event数を残す。
- cut、較正値、単位、座標変換、再構成、random seedを黙って変更しない。
- 物理結果へ影響する変更は、影響を説明し、内容の分かった小さいsampleで検証する。
- TTree branch、histogram、ROOT objectの既存名は、移行方法なしに変更しない。

## AIツール

環境に依存しないAI向けの共通ルールは、リポジトリ直下の[AGENTS.md](../AGENTS.md)に置きます。

`.cursor/`などはAI環境によって形式と必要性が変わるため、共通設定には含めません。必要な場合は`AGENTS.md`を原典として各環境で用意し、その管理方法も各利用者が決めます。複数環境で共通に必要になった知識は、環境固有設定ではなく`AGENTS.md`または適切なトピックの文書へ戻します。
