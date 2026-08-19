# Radinue

[English](README.md)

Radinue は、ディレクトリに保存したラジオ録音を聴くための、Windows・macOS
向けの小さなデスクトップオーディオプレイヤーです。再生中のファイルと位置を記録し、
前回停止した場所から正確に再開できます。

> [!NOTE]
> Radinue は開発初期段階です。開発ビルドでは再生操作を利用できますが、
> 安定版のプレイヤーはまだリリースされていません。

## 現在実装済みの機能

- ディレクトリを選択し、直下にあるオーディオファイルを一覧表示する
- 明示的な拡張子許可リストで、大文字・小文字を区別せずファイルを抽出する
- サブディレクトリ、シンボリックリンク、状態ファイル、未対応ファイルを除外する
- ファイル名を基準に、昇順・降順で決定的に並べ替える
- アイコン付きボタンで、前後のトラック移動、10秒戻る・進む、再生・一時停止を行う
- シークバーで再生位置を確認・変更する
- ピッチを補正しながら、再生速度を50%から200%まで10%刻みで変更する
- 音量を0%から200%まで調整する（100%超はソフトウェア増幅）
- 現在のファイルと再生位置をプレイリスト内の `.radinue-state.json` へ原子的に保存し、
  同じディレクトリを開いたときに復元する

対応拡張子は `.aac`、`.flac`、`.m4a`、`.mka`、`.mp3`、`.ogg`、`.opus`、
`.wav`、`.wma` です。

## 実装予定の再生機能

- プレイリストの末尾から先頭へ戻ることなく、次のファイルを自動再生する
- 壊れたファイルや未対応のファイルがあっても、アプリを終了せず安全に処理する

Radinue は、ローカルファイルを安定して再生することに集中します。ライブラリ管理、
ストリーミング、ポッドキャストフィード、シャッフル、リピート、イコライザー、
クラウド同期、テレメトリ、自動アップデートは対象外です。

## キーボードショートカット

ショートカットは Radinue にフォーカスがある間だけ有効です。

| キー | 操作 |
| --- | --- |
| `z` | 10秒戻る |
| `x` | 10秒進む |
| `s` | 再生速度を10%下げる |
| `d` | 再生速度を10%上げる |
| `g` | 再生速度を100%に戻す |

## 技術構成

Radinue は、意図的に小さく保った次のネイティブ技術構成を採用する予定です。

- C++20
- Qt 6 Widgets
- libmpv
- CMake

## ビルド

### 必要な環境

- CMake 3.25 以降
- C++20 対応コンパイラ（macOS は Apple Clang、Windows は MSVC）
- Widgets・Test コンポーネントを含む Qt 6.8 以降
- libmpv の開発用ヘッダーとライブラリ
- 開発者向けプリセットで使用する Ninja

macOS では、Homebrew を使って必要なパッケージをインストールできます。

```sh
brew install cmake ninja qt mpv
cmake --preset debug -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build --preset debug
ctest --preset debug
```

ビルドしたアプリを開き、ウィンドウからディレクトリを選択します。

```sh
open build/debug/src/radinue.app
```

開発用ビルドでは、起動時にディレクトリを渡すこともできます。

```sh
open build/debug/src/radinue.app --args "/path/to/recordings"
```

Windows では MSVC x64 ツールを含む Visual Studio 2022 と MSVC x64 用の
Qt 6.8.3 をインストールし、PowerShell で次のコマンドを実行してください。
セットアップスクリプトは固定バージョンの libmpv SDK をダウンロードし、SHA-256
ダイジェストを検証します。

```powershell
cmake -DOUTPUT_DIR="$PWD/.deps/mpv" -P cmake/DownloadMpvWindows.cmake
./scripts/CreateMpvImportLibrary.ps1 -MpvRoot "$PWD/.deps/mpv"
cmake --preset ci-windows
cmake --build --preset ci-windows --parallel
ctest --preset ci-windows
```

Qt が CMake の標準検索パスにない場合は、ローカルの `CMakeUserPresets.json` に
`CMAKE_PREFIX_PATH` を設定するか、configure コマンドに指定してください。このファイルは
Git の管理対象外なので、ローカルパスがコミットされることはありません。

対応予定のプラットフォームは次のとおりです。

- Windows（MSVC によるネイティブビルド）
- macOS（Apple Clang によるネイティブビルド）

GitHub Actions は、すべてのプルリクエストと `main` への push に対して、両方の
プラットフォームでアプリケーションをビルドし、テストを実行します。

Git タグを push すると、Windows amd64 と macOS Apple Silicon 向けの配布物を
ビルドし、そのタグの GitHub Release に公開します。リポジトリには署名・公証用の
認証情報を設定していないため、現在の macOS 配布物は ad-hoc 署名です。

## コントリビューション

Radinue は、安定性、予測可能な動作、小さなメンテナンス範囲を重視しています。
変更を提案する前に、プロジェクト要件、アーキテクチャ、テスト方針をまとめた
[AGENTS.md](AGENTS.md) を確認してください。

## ライセンス

Radinue は [GNU General Public License v3.0 or later](LICENSE) のもとで
公開される自由ソフトウェアです。

依存関係のライセンスと入手元については
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) を参照してください。
