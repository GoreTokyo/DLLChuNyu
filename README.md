# DLLChuNyu

## 概要
このプログラムは、指定したDLLをターゲットプロセスにインジェクトするツールです。ユーザーがプロセスID (PID) を指定し、ロードするDLLのパスを入力することで、DLLを対象プロセスに注入します。

## 特徴
- Windows API を使用してプロセスのメモリ領域を確保し、DLL のパスをコピー
- kernel32.dll の `LoadLibraryA` を利用して DLL をロード
- エラーハンドリングを強化し、適切なエラーメッセージを表示
- `HandleWrapper` クラスを利用し、リソースの適切な解放を保証

## 動作環境
- Windows 10 以上
- C++ コンパイラ (MinGW / MSVC など)

## 必要な権限
- 実行には管理者権限が必要です。
- ターゲットプロセスに対する `PROCESS_ALL_ACCESS` の権限を取得できる必要があります。

## コンパイル方法
```sh
g++ -o DLLChuNyu.exe DLLChuNyu.cpp -static -mwindows
```
または Visual Studio の `cl` を利用する場合:
```sh
cl /EHsc DLLChuNyu.cpp /link /OUT:DLLChuNyu.exe
```

## 使用方法
1. プログラムを管理者権限で実行
2. 対象プロセスの ID を入力
3. インジェクトする DLL のパスを入力
4. DLL が正常にロードされるとメッセージが表示される

## コードの説明
- `InjectDLL` 関数
- `OpenProcess` を使用して対象プロセスを開く
- `VirtualAllocEx` でプロセスのメモリ領域を確保
- `WriteProcessMemory` で DLL のパスをプロセスのメモリに書き込む
- `GetProcAddress` で `LoadLibraryA` のアドレスを取得
- `CreateRemoteThread` を利用して `LoadLibraryA` をターゲットプロセスで実行し、DLL をロード

## 注意点
- インジェクションを行う際、ターゲットプロセスの安定性を損なう可能性があります。
- 一部のアンチウイルスソフトウェアによってブロックされる可能性があります。
- 悪用厳禁。合法的な用途のみでご利用ください。

## 免責事項
本ツールを使用したことによるいかなる損害も開発者は責任を負いません。自己責任でご使用ください。