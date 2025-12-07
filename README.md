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
```llvm
; ModuleID = '/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/tests/e2e/dat/0045.cl'
source_filename = "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/tests/e2e/dat/0045.cl"

@__printfFormat = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @scanf(ptr, ...)

define i32 @main() {
entry:
  %N = alloca i32, align 4
  store i32 10, ptr %N, align 4
  %M = alloca i32, align 4
  store i32 20, ptr %M, align 4
  %P = alloca i32, align 4
  store i32 30, ptr %P, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i_load = load i32, ptr %i, align 4
  %__addAsgnResult = add i32 %i_load, 1
  store i32 %__addAsgnResult, ptr %i, align 4
  %N_load = load i32, ptr %N, align 4
  %__isLseResult = icmp sle i32 %__addAsgnResult, %N_load
  %__cmpResult = zext i1 %__isLseResult to i32
  %__convertToI1Result = icmp ne i32 %__cmpResult, 0
  br i1 %__convertToI1Result, label %while, label %endwhile

while:                                            ; preds = %endwhile2, %entry
  %j = alloca i32, align 4
  store i32 0, ptr %j, align 4
  %j_load = load i32, ptr %j, align 4
  %__addAsgnResult3 = add i32 %j_load, 1
  store i32 %__addAsgnResult3, ptr %j, align 4
  %M_load = load i32, ptr %M, align 4
  %__isLseResult4 = icmp sle i32 %__addAsgnResult3, %M_load
  %__cmpResult5 = zext i1 %__isLseResult4 to i32
  %__convertToI1Result6 = icmp ne i32 %__cmpResult5, 0
  br i1 %__convertToI1Result6, label %while1, label %endwhile2

endwhile:                                         ; preds = %endwhile2, %entry
  ret i32 0

while1:                                           ; preds = %endwhile8, %while
  %k = alloca i32, align 4
  store i32 0, ptr %k, align 4
  %k_load = load i32, ptr %k, align 4
  %__addAsgnResult9 = add i32 %k_load, 1
  store i32 %__addAsgnResult9, ptr %k, align 4
  %P_load = load i32, ptr %P, align 4
  %__isLseResult10 = icmp sle i32 %__addAsgnResult9, %P_load
  %__cmpResult11 = zext i1 %__isLseResult10 to i32
  %__convertToI1Result12 = icmp ne i32 %__cmpResult11, 0
  br i1 %__convertToI1Result12, label %while7, label %endwhile8

endwhile2:                                        ; preds = %endwhile8, %while
  %i_load28 = load i32, ptr %i, align 4
  %__addAsgnResult29 = add i32 %i_load28, 1
  store i32 %__addAsgnResult29, ptr %i, align 4
  %N_load30 = load i32, ptr %N, align 4
  %__isLseResult31 = icmp sle i32 %__addAsgnResult29, %N_load30
  %__cmpResult32 = zext i1 %__isLseResult31 to i32
  %__convertToI1Result33 = icmp ne i32 %__cmpResult32, 0
  br i1 %__convertToI1Result33, label %while, label %endwhile

while7:                                           ; preds = %while7, %while1
  %i_load13 = load i32, ptr %i, align 4
  %j_load14 = load i32, ptr %j, align 4
  %k_load15 = load i32, ptr %k, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @__printfFormat, i32 %i_load13, i32 %j_load14, i32 %k_load15)
  %k_load16 = load i32, ptr %k, align 4
  %__addAsgnResult17 = add i32 %k_load16, 1
  store i32 %__addAsgnResult17, ptr %k, align 4
  %P_load18 = load i32, ptr %P, align 4
  %__isLseResult19 = icmp sle i32 %__addAsgnResult17, %P_load18
  %__cmpResult20 = zext i1 %__isLseResult19 to i32
  %__convertToI1Result21 = icmp ne i32 %__cmpResult20, 0
  br i1 %__convertToI1Result21, label %while7, label %endwhile8

endwhile8:                                        ; preds = %while7, %while1
  %j_load22 = load i32, ptr %j, align 4
  %__addAsgnResult23 = add i32 %j_load22, 1
  store i32 %__addAsgnResult23, ptr %j, align 4
  %M_load24 = load i32, ptr %M, align 4
  %__isLseResult25 = icmp sle i32 %__addAsgnResult23, %M_load24
  %__cmpResult26 = zext i1 %__isLseResult25 to i32
  %__convertToI1Result27 = icmp ne i32 %__cmpResult26, 0
  br i1 %__convertToI1Result27, label %while1, label %endwhile2
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


# По учебной части:

## 9.
**Вопрос преподователя:**
+// seems like const method
NameValue *lookup(const std::string &name);

**Ответ:**
```text
/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/backend/interpreter/name_table.cppm:112:16: error: cannot initialize return object of type 'int *' with an rvalue of type 'const int *'
  112 |         return &(found->second);
      |                ^~~~~~~~~~~~~~~~
```

Здесь мы используем указатель, чтобы получить доступ к значению переменной и одновременно с этим использовать `nullptr` чтобы сигнализировать об отсуствии запрашиваемой переменной:

```cpp
    int *name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value);

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);
    *name_ptr = value;
```

Так как мы после `lookup` мы делаем еще присваивание по указателю мы не можем сделать `name_ptr` как `const int*`, поэтому и вернуть из `lookup` `const int*` мы не можем. Но если метод аннотирован как `const`, то так как мы внутри `lookup` итерируемся по `scopes_` (приватное поле в `NameTable`), то вернуть на его внутреннее состояние не `const` указатель мы не можем. Поэтому этот метод не `const`.

Так же мы его используем для того чтобы найти значение переменной, которое мы хотим поменять, поэтому `lookup` даже идейно не `const`.

## 14
**Вопрос преподователя:**
What the difference between AssignExpr, CombinedAssingExpr, AssignStmt and CombinedAssingStmt.
They are the same structures.
Please, describe with examples.


**Ответ:**
Мотивация для создания и `AssignStmt`, и `AssignExpr` (и подобных штук):

Возвращать значение, когда его у тебя не просят, немного странно: зачем делать лишний return, если его можно не делать. Мы можем на уровне парсинга грамматики определить, должно ли выражение `a = 0` иметь возвращаемое значение. Поэтому, мы сделали разделение на statement и expression.\
Пока это писал, пришла в голову мысль что возможно мы больше времени потеряем на лишнем dynamic_cast. Надо бы это попробовать забенчмарать (\*здесь появится сообщение о результатах, когда *(если)* я это сделаю). Но это для интепретатора, а для компилятора это кажется тем более бессмлысленно, потому что на llvm ir все соптимизируется. Но мы пока не будет это менять, нам интересно сделать бенчмарк и почитать, что Вы об этом думаете.

Примеры:

## 15
**Вопрос преподователя:**
What is the StringConstant? Please, describe with examples.

**Ответ:**

```cpp
struct StringConstant : public Expression;
```

Это структура неследуемая от Expression. Нужна для единственной цели - сделать вывод `print` более информативным. То есть строку можно использовать только как аргумент оператора `print`. Примеры использования:

Код:
```cl
print "Hello, string!";
```
Вывод:
```text
Hello, string!
```

Код:
```cl
int a = 5;
int b = 6;
print "sum of 'a' and 'b' is ", a + b;
```

Вывод:
```text
sum of 'a' and 'b' is 11
```
