#!/usr/bin/env bash

BINARY="$1"
shift

BINARY_DIR=$(dirname "$(realpath "$BINARY")")
REPORT_DIR="$BINARY_DIR/coverage-report"
PROFDATA="$BINARY_DIR/cov.profdata"

# Запуск (если нужно)
LLVM_PROFILE_FILE="$BINARY_DIR/raw-%p.profraw" "$BINARY" "$@"

# Мерж + HTML-отчёт
llvm-profdata merge -sparse "$BINARY_DIR"/*.profraw -o "$PROFDATA"
llvm-cov show "$BINARY" -instr-profile="$PROFDATA" -format=html -output-dir="$REPORT_DIR" -ignore-filename-regex='/usr/.*'

echo "Готово: $REPORT_DIR/index.html"