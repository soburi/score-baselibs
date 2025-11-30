..
    *******************************************************************************
    Copyright (c) 2025 Contributors to the Eclipse Foundation

    See the NOTICE file(s) distributed with this work for additional
    information regarding copyright ownership.

    This program and the accompanying materials are made available under the
    terms of the Apache License Version 2.0 which is available at
    https://www.apache.org/licenses/LICENSE-2.0

    SPDX-License-Identifier: Apache-2.0
    *******************************************************************************

S-CORE 向け OSAL に ASP3 バックエンドを追加するタスク
==================================================

目的
----

S-CORE を OS 非依存に進化させるために、Baselibs に OS 抽象化レイヤー（OSAL）を新設し、
既存の POSIX/QNX 依存コードを整理したうえで ASP3 (TOPPERS) バックエンドを追加する。
当面は logging など小規模モジュールで OSAL を介した動作を確認する。

スコープ
--------

* Baselibs に OSAL API を定義し、OS 中立なヘッダーとして公開する。
* 既存 POSIX/QNX 実装を OSAL 配下へ集約し、Bazel の select() で切り替えられるようにする。
* ASP3 (TOPPERS) バックエンドを追加する。C++ 実装で、公開ヘッダーに TOPPERS 型を漏らさない。
* logging モジュールを OSAL 経由に置き換え、Linux/QNX の既存テストを維持する。
* ASP3 で周期ログを出すサンプルを用意し、ビルド可能なことを確認する。

ディレクトリ構成案
------------------

::

  baselibs/
    score/
      os/
        include/score/os/
          osal.hpp      // OS 全体初期化・エラー型
          thread.hpp    // Thread / Task 抽象
          sync.hpp      // Mutex / Lock / Condition
          time.hpp      // Clock / Timer / sleep
          io.hpp        // 必要なら簡易出力
        src/common/     // OS 非依存コード
        src/posix/      // POSIX バックエンド
        src/qnx/        // QNX バックエンド
        src/asp3/       // ASP3(TOPPERS) バックエンド

最小 API セット
----------------

* 初期化: ``void os_init();``
* スレッド/タスク: ``Thread`` クラス（start/join 相当があれば十分）
* 排他: ``Mutex``（基本的な lock/unlock）
* 時刻/タイマ: ``TimePoint now();`` と ``void sleep_for(duration);``
* IO（任意）: ``void write(const char* buf, std::size_t len);``

ASP3 バックエンド指針
---------------------

* ``src/asp3/thread_asp3.cpp``: Thread を ASP3 タスクにマッピング。
  動的生成が難しければ静的タスク群への割り当てで可。
* ``src/asp3/sync_asp3.cpp``: 可能なら ASP3 のミューテックス/セマフォを使用。
  シングルコアなら ``loc_cpu()/unl_cpu()`` ベースでも簡易実装可。
* ``src/asp3/time_asp3.cpp``: ``get_utm()`` などから ns 変換して ``now()``、
  ``dly_tsk()`` などで ``sleep_for()``。
* ``src/asp3/io_asp3.cpp`` (必要に応じて): ``serial_wri_dat()`` 等を使った簡易出力。
* TOPPERS 固有ヘッダ（``kernel.h`` 等）は ``src/asp3`` に閉じ込め、
  公開ヘッダからは見せない。

Bazel 連携
----------

* ``score/os`` に ``cc_library`` を置き、``select()`` で OS ごとにソース切替。
* ``//score/config:os_asp3`` の ``config_setting`` を追加し、``--define score_os=asp3``
  のようなフラグで切り替える。

最小接続モジュール
------------------

* 対象は logging など小さな共通コンポーネント。
* pthread/QNX API への直接依存を洗い出し、すべて OSAL API 経由に差し替える。
* Linux/QNX の既存テストを継続して通す。
* ASP3 向けに「一定周期でログを出す」程度のサンプルを用意し、ビルドが通ることを確認する。

完了条件
--------

* Baselibs に ``score/os`` のインタフェースが追加されている。
* POSIX/QNX バックエンドが ``score/os/osal/posix`` と ``score/os/osal/qnx`` に整理され、
  既存機能に退行がない。
* ASP3 バックエンドが ``score/os/osal/asp3`` に追加され、``os_asp3`` ビルドが通る。
* logging モジュールが OSAL 経由で動作し、Linux/QNX テストがパスする。
* ASP3 でサンプルアプリがビルド・起動し、ログ出力が確認できる。
