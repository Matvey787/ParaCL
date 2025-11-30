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
./paracl -d ast.dot ...
```
Будет создан файл `ast.dot`, с дампом ast для вашей программы. Подробнее про [graphviz](https://graphviz.org/).


Доступна опция `--compile`, которая вместо интерпретации кода, копмипилирует его. ast языка переводится в llvm ir.
```bash
./paracl --compile some.cl build/some.o -o some_exe
./paracl -c some1.cl some2.cl # создаст исполняемый файл a.out
```

пример сгенерированного llvm ir:
```ll
; ModuleID = '../tests/e2e/dat/0001.cl'
source_filename = "../tests/e2e/dat/0001.cl"

@printf_format = private unnamed_addr constant [11 x i8] c"F(6) = %d\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @scanf(ptr, ...)

define i32 @main() {
entry:
  %fst = alloca i32, align 4
  store i32 0, ptr %fst, align 4
  %snd = alloca i32, align 4
  store i32 1, ptr %snd, align 4
  %iters = alloca i32, align 4
  store i32 6, ptr %iters, align 4
  %iters1 = load i32, ptr %iters, align 4
  %0 = icmp sgt i32 %iters1, 0
  %cmp_result = zext i1 %0 to i32
  %1 = icmp ne i32 %cmp_result, 0
  br i1 %1, label %while, label %endwhile

while:                                            ; preds = %while, %entry
  %fst2 = load i32, ptr %fst, align 4
  %tmp = alloca i32, align 4
  store i32 %fst2, ptr %tmp, align 4
  %snd3 = load i32, ptr %snd, align 4
  store i32 %snd3, ptr %fst, align 4
  %snd4 = load i32, ptr %snd, align 4
  %tmp5 = load i32, ptr %tmp, align 4
  %2 = add i32 %snd4, %tmp5
  store i32 %2, ptr %snd, align 4
  %iters6 = load i32, ptr %iters, align 4
  %3 = sub i32 %iters6, 1
  store i32 %3, ptr %iters, align 4
  %iters7 = load i32, ptr %iters, align 4
  %4 = icmp sgt i32 %iters7, 0
  %cmp_result8 = zext i1 %4 to i32
  %5 = icmp ne i32 %cmp_result8, 0
  br i1 %5, label %while, label %endwhile

endwhile:                                         ; preds = %while, %entry
  %snd9 = load i32, ptr %snd, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @printf_format, i32 %snd9)
  ret i32 0
}
```

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
многостровные комментари - `/* <text> */`\
`#!/path/to/paracl` - shebang


