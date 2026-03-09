# Компилятор и интепретор ParaCL

ААА БАЛЯ КОНАН РАБОТАЕТ
```shell
git clone https://github.com/Maksim-Sebelev/ParaCL.git;
cd ParaCL;
python3 -m venv .venv && source .venv/bin/activate && pip3 install conan;
conan install . --output-folder=third-party --build=missing;
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./third-party/conan_toolchain.cmake -DCMAKE_CXX_COMPILER=clang++ -G Ninja;
cmake --build build;
ctest --test-dir build;
build/benchmark;
```

## Зависимости

[Boost-1.70.0+](https://www.boost.org/releases/1.70.0/)\
[LLVM-17.0.1](https://github.com/llvm/llvm-project/releases/tag/llvmorg-17.0.1)\
[Bison-3.8.2](https://ftp.gnu.org/gnu/bison/)\
[Flex-2.6.4](https://sourceforge.net/projects/gnuwin32/files/flex/)

## Установка

```shell
git clone https://github.com/Maksim-Sebelev/ParaCL.git;
cd ParaCL;
cmake -S . -B build;
cmake --build build;
```

## Использование

компилятора:

```shell
build/paraclc <source>.cl [ -o <executbale> ];
./executable;
```

Использование интепретатора:

```shell
build/paracli <source>.cl
```

## Тестирование

```shell
ctest --test-dir build;
```

Если нужен полный отчет о каждом тесте:

```shell
ctest --test-dir build/subprojects/Compiler;
ctest --test-dir build/subprojects/Interpreter;
```

## Бенчмарк

```shell
build/benchmark
```

Сравнение интепретатора и компилятора:

```txt
Run becnmark with: 'benchmark/dat/0000.cl'
INTERPRETER
execution time: 0.00 s
COMPILER
compilation time: 0.08 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0001.cl'
INTERPRETER
execution time: 0.30 s
COMPILER
compilation time: 0.08 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0002.cl'
INTERPRETER
execution time: 0.29 s
COMPILER
compilation time: 0.07 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0003.cl'
INTERPRETER
execution time: 2.03 s
COMPILER
compilation time: 0.07 s
execution time: 0.03 s

Run becnmark with: 'benchmark/dat/0004.cl'
INTERPRETER
execution time: 3.65 s
COMPILER
compilation time: 0.07 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0005.cl'
INTERPRETER
execution time: 16.36 s
COMPILER
compilation time: 0.07 s
execution time: 0.02 s

Run becnmark with: 'benchmark/dat/0006.cl'
INTERPRETER
execution time: 0.00 s
COMPILER
compilation time: 0.07 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0007.cl'
INTERPRETER
execution time: 9.57 s
COMPILER
compilation time: 0.08 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0008.cl'
INTERPRETER
execution time: 103.42 s
COMPILER
compilation time: 0.08 s
execution time: 0.15 s

Run becnmark with: 'benchmark/dat/0009.cl'
INTERPRETER
execution time: 0.00 s
COMPILER
compilation time: 0.07 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0010.cl'
INTERPRETER
execution time: 1.51 s
COMPILER
compilation time: 0.08 s
execution time: 0.00 s

Run becnmark with: 'benchmark/dat/0011.cl'
INTERPRETER
execution time: 553.23 s
COMPILER
compilation time: 0.08 s
execution time: 0.95 s
```

Как видно, компилятор работает многогратно быстрее. А значит слава llvm и их ir.

## О языке

Компилятор и интерпретатор, по крайней мере по ожиданиям разработчиков, ведут себя совершенно одинаково.

## Что поддерживает язык

1 единственный тип для переменных - `int`\
области видимости\
арифметические операторы `+`, `-`, `*`, `%`, `/`, `+=`, `-=`, `*=`, `/=`, `%=`\
логические операторы: `&&`, `and`, `||`, `or`, `!`, `not`\
так же `+`, `-` могут быть унарными операторами\
цикл `while`\
условные операторы `if`-`else if`-`else`\
оператор ввода с stdin - `?`\
оператор ввывод в stdout - `print`\
`print` принимает аргументы через запятую - строковые константы - `"<text>"` и переменные. в конце всегда печатается перенос строки\
одностровные комментарии - `//`\
многостровные комментари - `/* <text> */`\
`#!/path/to/paracl` - shebang
