# 📁 ParaCL - Project Structure

*Generated on: 27.02.2026, 17:21:06*

## 📋 Quick Overview

| Metric | Value |
|--------|-------|
| 📄 Total Files | 362 |
| 📁 Total Folders | 84 |
| 🌳 Max Depth | 6 levels |

## ⭐ Important Files

- 🟡 🚫 **.gitignore** - Git ignore rules
- 🔴 📖 **README.md** - Project documentation
- 🟡 🚫 **.gitignore** - Git ignore rules
- 🔴 📖 **README.md** - Project documentation
- 🟡 🚫 **.gitignore** - Git ignore rules
- 🔴 📖 **README.md** - Project documentation

## 📊 File Statistics

### By File Type

- 📄 **.h** (Other files): 108 files (29.8%)
- 📄 **.cpp** (Other files): 46 files (12.7%)
- 📄 **.ans** (Other files): 45 files (12.4%)
- 📄 **.cl** (Other files): 45 files (12.4%)
- 📄 **.cppm** (Other files): 27 files (7.5%)
- 📄 **.cmake** (Other files): 17 files (4.7%)
- 📄 **.hpp** (Other files): 17 files (4.7%)
- 📄 **.txt** (Text files): 10 files (2.8%)
- 📄 **.** (Other files): 8 files (2.2%)
- 📄 **.in** (Other files): 8 files (2.2%)
- ⚙️ **.yml** (YAML files): 6 files (1.7%)
- 🖼️ **.png** (PNG images): 6 files (1.7%)
- 📄 **.sh** (Other files): 5 files (1.4%)
- 🚫 **.gitignore** (Git ignore): 3 files (0.8%)
- 📖 **.md** (Markdown files): 3 files (0.8%)
- 📄 **.py** (Other files): 3 files (0.8%)
- 📄 **.ipynb** (Other files): 1 files (0.3%)
- 📄 **.l** (Other files): 1 files (0.3%)
- 📄 **.y** (Other files): 1 files (0.3%)
- 📄 **.rst** (Other files): 1 files (0.3%)
- 🎨 **.svg** (SVG images): 1 files (0.3%)

### By Category

- **Other**: 333 files (92.0%)
- **Docs**: 13 files (3.6%)
- **Assets**: 7 files (1.9%)
- **Config**: 6 files (1.7%)
- **DevOps**: 3 files (0.8%)

### 📁 Largest Directories

- **root**: 362 files
- **third-party**: 191 files
- **third-party/spdlog**: 176 files
- **third-party/spdlog/include/spdlog**: 105 files
- **third-party/spdlog/include**: 105 files

## 🌳 Directory Structure

```
ParaCL/
├── 📄 .clang-format
├── 📂 .github/
│   └── 📂 workflows/
│   │   └── ⚙️ regression.yml
├── 🟡 🚫 **.gitignore**
├── 📄 .gitmodules
├── 📦 assets/
│   ├── 🖼️ no-msvc.png
│   ├── 🖼️ project-with-cmake-eyes.png
│   ├── 🖼️ test-failed-example.png
│   └── 🖼️ test-passed-example.png
├── 📂 cmake/
│   ├── 📄 debug.cmake
│   ├── 📂 dep/
│   │   ├── 📄 check-bison.cmake
│   │   ├── 📄 check-flex.cmake
│   │   ├── 📄 check-llvm.cmake
│   │   ├── 📄 check-optarg.cmake
│   │   └── 📄 global.cmake
│   ├── 📄 logger.cmake
│   └── 🧪 tests/
│   │   ├── 📂 e2e/
│   │   │   ├── 📄 add-e2e-to-target-function.cmake
│   │   │   └── 📄 e2e-paracl-python-script.cmake
│   │   └── 📂 unit/
│   │   │   ├── 📄 add-all-unit-tests.cmake
│   │   │   └── 📄 add-unit-test-to-target-function.cmake
├── 📄 CMakeLists.txt
├── 📄 conanfile.txt
├── 📂 debug/
│   ├── 📂 ast/
│   │   ├── 📄 ast-graph-dump.cppm
│   │   └── 📄 ast-text-dump.cppm
│   ├── 📂 in/
│   │   ├── 📄 run_compile_tets.sh.in
│   │   └── 📄 run_test.sh.in
│   ├── 📂 log/
│   │   ├── 📄 log_api.hpp
│   │   └── 📂 spdlog-init/
│   │   │   └── 📄 spdlog_init.cppm
│   ├── 📂 parse_program_output/
│   │   ├── 📄 parse_compile_test_result.py
│   │   └── 📄 parse_result.py
│   └── 📂 stacktrace/
│   │   └── 📄 exceptions-stacktrace.cppm
├── 📄 debug-start.sh
├── 📄 format.sh
├── 📂 include/
│   ├── 📂 codegen/
│   │   └── 📄 main_catch_blocks.in
│   └── 📂 global/
│   │   ├── 📄 custom_console_output.hpp
│   │   └── 📄 global.hpp
├── 📄 manual_for_collaborators.ipynb
├── 📂 paracl_compiler/
│   ├── 📂 backend/
│   ├── 📂 cmake/
│   │   └── 📂 deps/
│   │   │   ├── 📄 check-bison.cmake
│   │   │   └── 📄 check-flex.cmake
│   ├── 📄 CMakeLists.txt
│   ├── 📂 front/
│   │   ├── 📄 CMakeLists.txt
│   │   ├── 📂 lexer/
│   │   │   ├── 📂 inc/
│   │   │   │   └── 📄 lexer.hpp
│   │   │   └── 📁 src/
│   │   │   │   └── 📄 lexer.l
│   │   ├── 📄 main.cpp
│   │   └── 📂 parser/
│   │   │   └── 📁 src/
│   │   │   │   ├── 📄 ast.cppm
│   │   │   │   ├── 📄 check_variables.cpp
│   │   │   │   ├── 📄 check_variables.hpp
│   │   │   │   ├── 📄 parse_error.cpp
│   │   │   │   ├── 📄 parse_error.hpp
│   │   │   │   └── 📄 parser.y
│   └── 📂 frontened_old/
│   │   └── 📂 parser/
│   │   │   ├── 📂 inc_old/
│   │   │   │   ├── 📄 ast.hpp
│   │   │   │   ├── 📄 parser_exceptions.hpp
│   │   │   │   ├── 📄 parser.hpp
│   │   │   │   └── 📄 token_types.hpp
│   │   │   └── 📂 src_old/
│   │   │   │   └── 📄 parser_exceptions.cpp
├── 🔴 📖 **README.md**
├── 📁 src/
│   ├── 📂 ast/
│   │   ├── 📄 ast.cppm
│   │   ├── 📄 node-type-erasure.cppm
│   │   ├── 📄 nodes.cppm
│   │   ├── 📄 test.cpp
│   │   └── 📄 write.cppm
│   ├── 📂 backend/
│   │   ├── 📂 interpreter/
│   │   │   ├── 📄 interpreter_2.cppm
│   │   │   ├── 📄 interpreter.cppm
│   │   │   └── 📄 nametable.cppm
│   │   └── 📂 toolchain/
│   │   │   ├── 📂 compiler/
│   │   │   │   ├── 📄 libc_stadart_functions.cppm
│   │   │   │   ├── 📄 nametable.cppm
│   │   │   │   └── 📄 objects_builder.cppm
│   │   │   ├── 📂 linker/
│   │   │   │   └── 📄 linker.cppm
│   │   │   └── 📄 toolchain.cppm
│   ├── 📂 frontend/
│   │   ├── 📂 ast_builder/
│   │   │   └── 📄 ast_builder.cppm
│   │   ├── 📂 hir_builder/
│   │   │   └── 📄 hir_builder.cppm
│   │   └── 📂 parser/
│   ├── 📂 hir/
│   │   └── 📄 hir.cppm
│   ├── 📄 main.cpp
│   ├── 📂 options/
│   │   ├── 📄 compiler_options.cppm
│   │   ├── 📄 interpreter_options.cppm
│   │   └── 📄 options.cppm
│   ├── 📂 options_parser/
│   │   └── 📄 options_parser.cppm
│   ├── 📂 paracl_info/
│   │   └── 📄 paracl_info.cppm.in
│   └── 📂 paracl_runner/
│   │   ├── 📄 parse_exceptions.cppm
│   │   └── 📄 runner.cppm
├── 📄 start.sh
├── 🧪 tests/
│   ├── 📂 e2e/
│   │   ├── 📂 ans/
│   │   │   ├── 📄 0001.ans
│   │   │   ├── 📄 0002.ans
│   │   │   ├── 📄 0003.ans
│   │   │   ├── 📄 0004.ans
│   │   │   ├── 📄 0005.ans
│   │   │   ├── 📄 0006.ans
│   │   │   ├── 📄 0007.ans
│   │   │   ├── 📄 0008.ans
│   │   │   ├── 📄 0009.ans
│   │   │   ├── 📄 0010.ans
│   │   │   ├── 📄 0011.ans
│   │   │   ├── 📄 0012.ans
│   │   │   ├── 📄 0013.ans
│   │   │   ├── 📄 0014.ans
│   │   │   ├── 📄 0015.ans
│   │   │   ├── 📄 0016.ans
│   │   │   ├── 📄 0017.ans
│   │   │   ├── 📄 0018.ans
│   │   │   ├── 📄 0019.ans
│   │   │   ├── 📄 0020.ans
│   │   │   ├── 📄 0021.ans
│   │   │   ├── 📄 0022.ans
│   │   │   ├── 📄 0023.ans
│   │   │   ├── 📄 0024.ans
│   │   │   ├── 📄 0025.ans
│   │   │   ├── 📄 0026.ans
│   │   │   ├── 📄 0027.ans
│   │   │   ├── 📄 0028.ans
│   │   │   ├── 📄 0029.ans
│   │   │   ├── 📄 0030.ans
│   │   │   ├── 📄 0031.ans
│   │   │   ├── 📄 0032.ans
│   │   │   ├── 📄 0033.ans
│   │   │   ├── 📄 0034.ans
│   │   │   ├── 📄 0035.ans
│   │   │   ├── 📄 0036.ans
│   │   │   ├── 📄 0037.ans
│   │   │   ├── 📄 0038.ans
│   │   │   ├── 📄 0039.ans
│   │   │   ├── 📄 0040.ans
│   │   │   ├── 📄 0041.ans
│   │   │   ├── 📄 0042.ans
│   │   │   ├── 📄 0043.ans
│   │   │   ├── 📄 0044.ans
│   │   │   └── 📄 0045.ans
│   │   └── 📂 dat/
│   │   │   ├── 📄 0001.cl
│   │   │   ├── 📄 0002.cl
│   │   │   ├── 📄 0003.cl
│   │   │   ├── 📄 0004.cl
│   │   │   ├── 📄 0005.cl
│   │   │   ├── 📄 0006.cl
│   │   │   ├── 📄 0007.cl
│   │   │   ├── 📄 0008.cl
│   │   │   ├── 📄 0009.cl
│   │   │   ├── 📄 0010.cl
│   │   │   ├── 📄 0011.cl
│   │   │   ├── 📄 0012.cl
│   │   │   ├── 📄 0013.cl
│   │   │   ├── 📄 0014.cl
│   │   │   ├── 📄 0015.cl
│   │   │   ├── 📄 0016.cl
│   │   │   ├── 📄 0017.cl
│   │   │   ├── 📄 0018.cl
│   │   │   ├── 📄 0019.cl
│   │   │   ├── 📄 0020.cl
│   │   │   ├── 📄 0021.cl
│   │   │   ├── 📄 0022.cl
│   │   │   ├── 📄 0023.cl
│   │   │   ├── 📄 0024.cl
│   │   │   ├── 📄 0025.cl
│   │   │   ├── 📄 0026.cl
│   │   │   ├── 📄 0027.cl
│   │   │   ├── 📄 0028.cl
│   │   │   ├── 📄 0029.cl
│   │   │   ├── 📄 0030.cl
│   │   │   ├── 📄 0031.cl
│   │   │   ├── 📄 0032.cl
│   │   │   ├── 📄 0033.cl
│   │   │   ├── 📄 0034.cl
│   │   │   ├── 📄 0035.cl
│   │   │   ├── 📄 0036.cl
│   │   │   ├── 📄 0037.cl
│   │   │   ├── 📄 0038.cl
│   │   │   ├── 📄 0039.cl
│   │   │   ├── 📄 0040.cl
│   │   │   ├── 📄 0041.cl
│   │   │   ├── 📄 0042.cl
│   │   │   ├── 📄 0043.cl
│   │   │   ├── 📄 0044.cl
│   │   │   └── 📄 0045.cl
│   └── 📂 unit/
│   │   └── 📂 options/
│   │   │   └── 📄 options.cpp
└── 📂 third-party/
│   ├── 📂 logger/
│   │   ├── 🟡 🚫 **.gitignore**
│   │   ├── 📦 assets/
│   │   │   └── 🖼️ example.png
│   │   ├── 📂 examples/
│   │   │   └── 📂 header-only/
│   │   │   │   ├── 📄 CMakeLists.txt
│   │   │   │   └── 📄 test.cpp
│   │   ├── 🔴 📖 **README.md**
│   │   └── 📁 Src/
│   │   │   ├── 📂 header-only/
│   │   │   │   ├── 📄 custom_console_output.hpp
│   │   │   │   ├── 📄 global.hpp
│   │   │   │   ├── 📄 log_background_settings.hpp
│   │   │   │   └── 📄 pineaplog.hpp
│   │   │   └── 📂 modules-based/
│   │   │   │   ├── 📂 cmake/
│   │   │   │   │   └── 📄 debug.cmake
│   │   │   │   ├── 📄 CMakeLists.txt
│   │   │   │   ├── 📂 include/
│   │   │   │   │   ├── 📂 global/
│   │   │   │   │   │   ├── 📄 custom_console_output.hpp
│   │   │   │   │   │   └── 📄 global.hpp
│   │   │   │   │   └── 📂 logger/
│   │   │   │   │   │   └── 📄 log_background_settings.hpp
│   │   │   │   └── 📁 src/
│   │   │   │   │   └── 📄 log.cppm
│   └── 📂 spdlog/
│   │   ├── 📄 .clang-format
│   │   ├── 📄 .clang-tidy
│   │   ├── 📄 .git-blame-ignore-revs
│   │   ├── 📄 .gitattributes
│   │   ├── 📂 .github/
│   │   │   └── 📂 workflows/
│   │   │   │   ├── ⚙️ coverity_scan.yml
│   │   │   │   ├── ⚙️ linux.yml
│   │   │   │   ├── ⚙️ macos.yml
│   │   │   │   └── ⚙️ windows.yml
│   │   ├── 🟡 🚫 **.gitignore**
│   │   ├── ⚙️ appveyor.yml
│   │   ├── 📂 bench/
│   │   │   ├── 📄 async_bench.cpp
│   │   │   ├── 📄 bench.cpp
│   │   │   ├── 📄 CMakeLists.txt
│   │   │   ├── 📄 formatter-bench.cpp
│   │   │   ├── 📄 latency.cpp
│   │   │   └── 📄 utils.h
│   │   ├── 📂 cmake/
│   │   │   ├── 📄 ide.cmake
│   │   │   ├── 📄 pch.h.in
│   │   │   ├── 📄 spdlog.pc.in
│   │   │   ├── 📄 spdlogConfig.cmake.in
│   │   │   ├── 📄 spdlogCPack.cmake
│   │   │   ├── 📄 utils.cmake
│   │   │   └── 📄 version.rc.in
│   │   ├── 📄 CMakeLists.txt
│   │   ├── 📂 example/
│   │   │   ├── 📄 CMakeLists.txt
│   │   │   └── 📄 example.cpp
│   │   ├── 📂 include/
│   │   │   └── 📂 spdlog/
│   │   │   │   ├── 📄 async_logger-inl.h
│   │   │   │   ├── 📄 async_logger.h
│   │   │   │   ├── 📄 async.h
│   │   │   │   ├── 📂 cfg/
│   │   │   │   │   ├── 📄 argv.h
│   │   │   │   │   ├── 📄 env.h
│   │   │   │   │   ├── 📄 helpers-inl.h
│   │   │   │   │   └── 📄 helpers.h
│   │   │   │   ├── 📄 common-inl.h
│   │   │   │   ├── 📄 common.h
│   │   │   │   ├── 📂 details/
│   │   │   │   │   ├── 📄 backtracer-inl.h
│   │   │   │   │   ├── 📄 backtracer.h
│   │   │   │   │   ├── 📄 circular_q.h
│   │   │   │   │   ├── 📄 console_globals.h
│   │   │   │   │   ├── 📄 file_helper-inl.h
│   │   │   │   │   ├── 📄 file_helper.h
│   │   │   │   │   ├── 📄 fmt_helper.h
│   │   │   │   │   ├── 📄 log_msg_buffer-inl.h
│   │   │   │   │   ├── 📄 log_msg_buffer.h
│   │   │   │   │   ├── 📄 log_msg-inl.h
│   │   │   │   │   ├── 📄 log_msg.h
│   │   │   │   │   ├── 📄 mpmc_blocking_q.h
│   │   │   │   │   ├── 📄 null_mutex.h
│   │   │   │   │   ├── 📄 os-inl.h
│   │   │   │   │   ├── 📄 os.h
│   │   │   │   │   ├── 📄 periodic_worker-inl.h
│   │   │   │   │   ├── 📄 periodic_worker.h
│   │   │   │   │   ├── 📄 registry-inl.h
│   │   │   │   │   ├── 📄 registry.h
│   │   │   │   │   ├── 📄 synchronous_factory.h
│   │   │   │   │   ├── 📄 tcp_client-windows.h
│   │   │   │   │   ├── 📄 tcp_client.h
│   │   │   │   │   ├── 📄 thread_pool-inl.h
│   │   │   │   │   ├── 📄 thread_pool.h
│   │   │   │   │   ├── 📄 udp_client-windows.h
│   │   │   │   │   ├── 📄 udp_client.h
│   │   │   │   │   └── 📄 windows_include.h
│   │   │   │   ├── 📂 fmt/
│   │   │   │   │   ├── 📄 bin_to_hex.h
│   │   │   │   │   ├── 📂 bundled/
│   │   │   │   │   │   ├── 📄 args.h
│   │   │   │   │   │   ├── 📄 base.h
│   │   │   │   │   │   ├── 📄 chrono.h
│   │   │   │   │   │   ├── 📄 color.h
│   │   │   │   │   │   ├── 📄 compile.h
│   │   │   │   │   │   ├── 📄 core.h
│   │   │   │   │   │   ├── 📄 fmt.license.rst
│   │   │   │   │   │   ├── 📄 format-inl.h
│   │   │   │   │   │   ├── 📄 format.h
│   │   │   │   │   │   ├── 📄 os.h
│   │   │   │   │   │   ├── 📄 ostream.h
│   │   │   │   │   │   ├── 📄 printf.h
│   │   │   │   │   │   ├── 📄 ranges.h
│   │   │   │   │   │   ├── 📄 std.h
│   │   │   │   │   │   └── 📄 xchar.h
│   │   │   │   │   ├── 📄 chrono.h
│   │   │   │   │   ├── 📄 compile.h
│   │   │   │   │   ├── 📄 fmt.h
│   │   │   │   │   ├── 📄 ostr.h
│   │   │   │   │   ├── 📄 ranges.h
│   │   │   │   │   ├── 📄 std.h
│   │   │   │   │   └── 📄 xchar.h
│   │   │   │   ├── 📄 formatter.h
│   │   │   │   ├── 📄 fwd.h
│   │   │   │   ├── 📄 logger-inl.h
│   │   │   │   ├── 📄 logger.h
│   │   │   │   ├── 📄 mdc.h
│   │   │   │   ├── 📄 pattern_formatter-inl.h
│   │   │   │   ├── 📄 pattern_formatter.h
│   │   │   │   ├── 📂 sinks/
│   │   │   │   │   ├── 📄 android_sink.h
│   │   │   │   │   ├── 📄 ansicolor_sink-inl.h
│   │   │   │   │   ├── 📄 ansicolor_sink.h
│   │   │   │   │   ├── 📄 base_sink-inl.h
│   │   │   │   │   ├── 📄 base_sink.h
│   │   │   │   │   ├── 📄 basic_file_sink-inl.h
│   │   │   │   │   ├── 📄 basic_file_sink.h
│   │   │   │   │   ├── 📄 callback_sink.h
│   │   │   │   │   ├── 📄 daily_file_sink.h
│   │   │   │   │   ├── 📄 dist_sink.h
│   │   │   │   │   ├── 📄 dup_filter_sink.h
│   │   │   │   │   ├── 📄 hourly_file_sink.h
│   │   │   │   │   ├── 📄 kafka_sink.h
│   │   │   │   │   ├── 📄 mongo_sink.h
│   │   │   │   │   ├── 📄 msvc_sink.h
│   │   │   │   │   ├── 📄 null_sink.h
│   │   │   │   │   ├── 📄 ostream_sink.h
│   │   │   │   │   ├── 📄 qt_sinks.h
│   │   │   │   │   ├── 📄 ringbuffer_sink.h
│   │   │   │   │   ├── 📄 rotating_file_sink-inl.h
│   │   │   │   │   ├── 📄 rotating_file_sink.h
│   │   │   │   │   ├── 📄 sink-inl.h
│   │   │   │   │   ├── 📄 sink.h
│   │   │   │   │   ├── 📄 stdout_color_sinks-inl.h
│   │   │   │   │   ├── 📄 stdout_color_sinks.h
│   │   │   │   │   ├── 📄 stdout_sinks-inl.h
│   │   │   │   │   ├── 📄 stdout_sinks.h
│   │   │   │   │   ├── 📄 syslog_sink.h
│   │   │   │   │   ├── 📄 systemd_sink.h
│   │   │   │   │   ├── 📄 tcp_sink.h
│   │   │   │   │   ├── 📄 udp_sink.h
│   │   │   │   │   ├── 📄 win_eventlog_sink.h
│   │   │   │   │   ├── 📄 wincolor_sink-inl.h
│   │   │   │   │   └── 📄 wincolor_sink.h
│   │   │   │   ├── 📄 spdlog-inl.h
│   │   │   │   ├── 📄 spdlog.h
│   │   │   │   ├── 📄 stopwatch.h
│   │   │   │   ├── 📄 tweakme.h
│   │   │   │   └── 📄 version.h
│   │   ├── 📄 INSTALL
│   │   ├── 📄 LICENSE
│   │   ├── 📂 logos/
│   │   │   ├── 🎨 jetbrains-variant-4.svg
│   │   │   └── 🖼️ spdlog.png
│   │   ├── 🔴 📖 **README.md**
│   │   ├── 📂 scripts/
│   │   │   ├── 📄 ci_setup_clang.sh
│   │   │   ├── 📄 extract_version.py
│   │   │   └── 📄 format.sh
│   │   ├── 📁 src/
│   │   │   ├── 📄 async.cpp
│   │   │   ├── 📄 bundled_fmtlib_format.cpp
│   │   │   ├── 📄 cfg.cpp
│   │   │   ├── 📄 color_sinks.cpp
│   │   │   ├── 📄 file_sinks.cpp
│   │   │   ├── 📄 spdlog.cpp
│   │   │   └── 📄 stdout_sinks.cpp
│   │   └── 🧪 tests/
│   │   │   ├── 📄 CMakeLists.txt
│   │   │   ├── 📄 includes.h
│   │   │   ├── 📄 main.cpp
│   │   │   ├── 📄 test_async.cpp
│   │   │   ├── 📄 test_backtrace.cpp
│   │   │   ├── 📄 test_bin_to_hex.cpp
│   │   │   ├── 📄 test_cfg.cpp
│   │   │   ├── 📄 test_circular_q.cpp
│   │   │   ├── 📄 test_create_dir.cpp
│   │   │   ├── 📄 test_custom_callbacks.cpp
│   │   │   ├── 📄 test_daily_logger.cpp
│   │   │   ├── 📄 test_dup_filter.cpp
│   │   │   ├── 📄 test_errors.cpp
│   │   │   ├── 📄 test_eventlog.cpp
│   │   │   ├── 📄 test_file_helper.cpp
│   │   │   ├── 📄 test_file_logging.cpp
│   │   │   ├── 📄 test_fmt_helper.cpp
│   │   │   ├── 📄 test_macros.cpp
│   │   │   ├── 📄 test_misc.cpp
│   │   │   ├── 📄 test_mpmc_q.cpp
│   │   │   ├── 📄 test_pattern_formatter.cpp
│   │   │   ├── 📄 test_registry.cpp
│   │   │   ├── 📄 test_ringbuffer.cpp
│   │   │   ├── 📄 test_sink.h
│   │   │   ├── 📄 test_stdout_api.cpp
│   │   │   ├── 📄 test_stopwatch.cpp
│   │   │   ├── 📄 test_systemd.cpp
│   │   │   ├── 📄 test_time_point.cpp
│   │   │   ├── 📄 utils.cpp
│   │   │   └── 📄 utils.h
```

## 📖 Legend

### File Types
- 📄 Other: Other files
- ⚙️ Config: YAML files
- 🚫 DevOps: Git ignore
- 📄 Docs: Text files
- 📖 Docs: Markdown files
- 🖼️ Assets: PNG images
- 🎨 Assets: SVG images

### Importance Levels
- 🔴 Critical: Essential project files
- 🟡 High: Important configuration files
- 🔵 Medium: Helpful but not essential files
