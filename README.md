# ParaCL (младший брат С, калека, интерпретируемый язык программирования)

![C++](https://img.shields.io/badge/C++-23-blue?style=for-the-badge&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.28+-green?style=for-the-badge&logo=cmake)
![Testing](https://img.shields.io/badge/Google_Test-Framework-red?style=for-the-badge&logo=google)

# По поводу Windows и MSVC
![нельзя](./assets/no-msvc.png)

# Зависимости:
[flex](https://github.com/westes/flex)\
[bison](https://web.mit.edu/gnu/doc/html/bison_7.html)
 
Для удобной работы понадобятся следующие инструменты:

## Для комплиляции:
[clang++](https://clang.org/)\
[g++](https://gcc.gnu.org/)\
`clang` рекомендуется для данного проекта, так как на момент написания проекта он лучше умеет работать с `.cppm` модулями.

## Для сборки:
[cmake](https://cmake.org/ "ссылка на официальный сайт cmake")\
[ninja](https://ninja-build.org "ссылка на оффициальный сайт ninja")\
`Ninja` рекомендуется для данного проекта, так как в большистве пакетных менеджеров поставляется версия, для которой `cmake` умеет работать с модулями (`.cppm`).

# Установка:

```bash
git clone https://github.com/Matvey787/ParaCL --recurse-submodules
cd ParaCL
```

# Сборка:
Если поддреживается bash или совместимый аналог:
```bash
bash start.sh
```
Иначе (релизная сборка по умолчанию):
```bash
cmake -S . -B build
cmake --build build
```

Рекомендуется явно указать `Ninja` и `clang++`:
```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
```


# Использование:
*из папки build*
Чтобы передать текст программы через `stdout`:
```bash
./paracl
# paracl ожидает ввод текста программы с stdout
```

Чтобы передать программу через файлы:
```bash
./paracl <1>.cl <2>.cl  ... 
```

Чтобы получить помощь по использованию `ParaCL`:
```bash
./paracl --help
./paracl -h
```
Чтобы получить текущую версию `ParaCL`, установленную на Вашем устройстве:
```bash
./paracl --version
./paracl -v
```

Если вы собрали `ParaCL` в опцией cmake `GRAPHVIZ=ON`, то тогда доступен флаг `--ast-dump` и его короткая версия `-d`:
```bash
./paracl --ast-dump=ast.dot ...
./paracl -dast.dot ...
```
Будет создан файл `ast.dot`, с дампом ast для вашей программы. Подробнее про [graphviz](https://graphviz.org/).


На данный момент разрабатывается опция `--compile`, которая вместо интерпретации кода, копмипилирует его. ast языка переводится в llvm ir.
```bash
./paracl --compile some.cl build/some.o -o some_exe
./paracl -c some1.cl some2.cl # создаст исполняемый файл a.out
```
*на самом деле многофайловость поддерживает только на уровне парсинга опций и сборки объектников. а на практике из-за отсуствия в языке функций каждый .cl файл представляет собой один большой main*

**ВАЖНО: `ParaCL`, как уважающий себя язык имеет собственно расширение: `.cl`. Файл с другим расширением программой на `ParaCL` являться НЕ может!**

# Что поддерживает язык:

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
многостровные комментари - `/* <text> */`
