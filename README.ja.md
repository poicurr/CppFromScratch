# CppFromScratch

[English README](README.md)

C++20でデータ構造・アルゴリズム・低レベルユーティリティを自力で実装し、復習・参照用に蓄積するリポジトリです。製品コードで標準ライブラリを置き換えることは目的にしていません。

## 動作環境

- CMake 3.24以上、Ninja、C++20対応コンパイラ。
- **Debug**：ASan + UBSanに対応したClangまたはGCC。Windowsでは対応するSanitizerランタイムを含むClangが必要です。MSVC単独ではこの設定のUBSanを利用できません。
- Catch2 v3はSanitizer有効時にソースからビルドします。初回はネットワーク接続またはローカルのCatch2ソースが必要です。
- **TSan**：Linux専用。ASan / UBSanとは別ビルド。

## ビルド・CTest

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

```sh
cmake --preset release
cmake --build --preset release
ctest --preset release
```

## コード品質チェック

```sh
cmake --build --preset debug --target check
```

`check` はテストをビルドしてCTestを実行し、導入済みの解析ツールを実行します。

| ターゲット | ツール | 内容 |
| --- | --- | --- |
| `format-check` | clang-format | フォーマットを変更せず検査 |
| `clang-tidy` | clang-tidy | 静的解析 |
| `cppcheck` | Cppcheck | 補助的な静的解析 |

WindowsのClang + ASanでは `cmake/AsanRuntime.cmake` が通常ビルドのたびにASan DLLを実行ファイルの隣へ配置します。LinuxではDLLコピーを行わず、`-fsanitize` によってランタイムをリンクします。DebugではMSVCの非Debug CRT（`/MD`）を使い、デバッグ情報とSanitizerは維持します。

DebugのASan + UBSanはCatch2のソースビルドにも適用されるため、MSVC STLのASanアノテーション不整合を防げる構成です。

LinuxでTSanを使用する場合：

```sh
cmake --preset tsan
cmake --build --preset tsan
ctest --preset tsan
```

## 課題リスト

- [x] `LinkedList<T>` — 双方向連結リスト
- [x] `Stack<T>` — LIFO
- [x] `Queue<T>` — FIFO
- [ ] `DynamicArray<T>` — 動的配列
- [ ] `RingBuffer<T>` — 循環バッファ
- [ ] `UniquePtr<T>` — RAIIと排他的所有権
- [ ] `BinarySearchTree<T>` — 二分探索木
- [ ] `BinaryHeap<T>` — 二分ヒープ
- [ ] `HashMap<K, V>` — ハッシュの衝突処理と再ハッシュ
- [ ] `BitSet` — ビット演算
- [ ] `ObjectPool<T>` — オブジェクトの再利用
- [ ] `ArenaAllocator` — メモリアロケータとアラインメント
- [ ] `AVLTree<T>` — 平衡二分探索木
- [ ] `RedBlackTree<T>` — 赤黒木
- [ ] `LRUCache<K, V>` — ハッシュと連結リストの組み合わせ
- [ ] `SparseSet<T>` — 疎集合
- [ ] `SlotMap<T>` — 世代付きハンドル
- [ ] `SharedPtr<T>` — 共有所有権
- [ ] `Function<R(Args...)>` — 型消去
- [ ] `LockFreeQueue<T>` — アトミック操作とメモリ順序

## 課題の追加

`include/CppFromScratch/` にヘッダーを作り、`tests/` にテストを追加して `tests/CMakeLists.txt` へ登録します。

CIおよび旧Presetのエイリアスは含めません。

## ライセンス

MIT（[LICENSE](LICENSE)）。
