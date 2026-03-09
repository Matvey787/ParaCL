# THE-LAST (the library abstract syntax tree)

Всем import std!

В данном проекта реализовано то, что в сообществе принято называть ast (abstract syntax tree), разумеется вместе с прилагающимися к нему нодами. Конечно предусмотрены базовые методы работы с деревом.
В данной библиотеке мы попытались пересмотреть классический подход к проектированию ast. Думаю, долгая практика покажет несостоятельность многих наших идей, но революция должна с чего то начинаться))\
Основная идея данной библиотеки в ее абсолютной расширяемости. Вы можете добавлять абсолютно любые ноды, без необходимости наследовать их от чего-то, а чтобы реализовать поддержку методов работы с ними достаточно лишь добавить внешнюю функцию. Главным отличием здешнего подхдода к проектированию ast от стандартного в том, что ноды абсолютно независимы друг друга и методов работы с ними. Нет никаких ужасающих иерархий наследования нод, нет никаких громоздких 'визиторов' с множеством перегруженных виртуальных функий и методами accept в нодах, так же нет нужды хранить ноды как указатели.

Все это конечно далось нам несовсем бесплатно, но о деталях позже.

Главным классом всего проекта является `last::BasicNode`. Он реализует паттерн type erasure для нод в проекте (здесь нет привязки к слову "нода", он может стирать любой тип). Как Вам несомненно известно, суть паттерна type erasure есть совмещение двух идей, назовем их "внешними виртуальными функциями" и "хранением разных объектов в одном контейнере с семантикой значения, а не указателей".
Однако сами ноды дерева содержат поля типа last::BasicNode (или зависящего от last::BasicNode), поэтому возникает проблема с классическим подходом реализации методов работы с нодами через перегрузку внешних функцию, а именно:
так как сам last::BasicNode становится полноценным участником дерева, то он должен реализовывать все методы работы с ним, которые нужны при работе с деревом. Но конечно серьезной ошибкой проектирования было бы реализовать функцию в духе 'void interpret(last::AST const& ast)' в библиотеке самого ast. Поэтому классическая реализация type erasure здесь неприменима.

**Далее пойдет объяснение некоторых моментов довольно популярно, чтобы рассказать суть происходящего, за реальными деталями реализации просим обратиться непосредственно к коду :)**\
Для выхода из положения в данном проекте было предложено то, что **с точки зрения пользователя** было бы правильно воспринимать следующим образом: "шаблонные виртуальные функции".\
Причина по которой шаблонные виртуальные функции невозожны в С++ - это то, что базовый и наследуемый классы могут находиться с разных единицах трансляции, потому неясно что и где инстанцировать. Но в паттерн type erasure скрывает наследование как деталь реализации одного класса, потому здесь можно попытаться изобрести эти самые шаблонные виртуальные функции. В классе `last::BasicNode` есть шаблонный подкласс `template <typename... Singantures> last::BasicNode::Actions`, а в нем статический метод `template <typename NodeT> last::BasicNode create(NodeT&& node)`. Данный метод заменяет конструктор класса. Шаблонный параметр самого метода есть тип, который стирает BasicNode, аргумент - соостветсвующий объект. Шаблонные параметры подкласса Action есть сигнатуры "виртуальных функций", которые должна поддерживать эта нода. Это стоит рассматривать как просьбу к компилятору инстанциовать такие-то "специализации шаблонной виртуальной функции visit" для этого этой ноды.\
Например:

```cpp
using dumpable = void (std::ostream& /* dump_os */, size_t /* enclosure = tabs quantity */);
using calculable = int ();

auto&& left = /* ... */;
auto&& right = /* ... */;
auto&& bin_op = last::node::BinaryOperator{last::node::BinaryOperator::PLUS, std::move(left), std::move(right)};
auto&& node = last::node::BasicNode::Actions<dumpable, calculable>::create(std::move(bin_op));

/* node dump */
auto&& dump_os = std::ostream{/*ctor*/};
visit<void, std::ostream&, size_t>(node, dump_os, 0);

/* node calculate */
auto&& bin_op_result = visit<int>(node);
```

## Как реализовать свой метод работы с last::node::BasicNode?

В модуле, отвечающем за BasicNode, есть шаблонная функция:

```cpp
template <typename NodeT, typename ReturnT, typename... Args>
ReturnT last::node::visit_specializations::visit(NodeT const &, Args...)
{
    static_assert(false, "using not specialized visit function. "
                         "this static_assert help you to not see terribale linker errors :)");
}
```

Она нужна, чтобы при сборке модуля компилятор мог полагаться на наличие функции с именем visit и наличием первого аргумента, как ссылку на константые данные (внешняя функция из паттерна type erasure для тех самых "шаблонных виртуальных функций"). Пусть Вас не пугает `static_assert(false)`, ведь для реализации Ваших методов для работы с нодами, Вам нужно будет делать ее специализации для каждой ноды. Например:

```cpp
/* модуль графического дампа дерева */
template <>
void visit(Print const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Print", "style=filled, fillcolor=\"lightyellow\"");

    for (auto&& arg : node)
        graphic_dump::dump_and_link_with_parent(os, unique_node_id, arg);
}
```

Для всех нод реализованы аналогичные функции visit с такой же сигнатурой. После того, как были реализованы все функции, можете вызывать `visit<void, unique_node_id_t, std::ofstream&>(/* BasicNode */ node)`.

## Интерфейс visit для BasicNode

Сама функция visit для BasicNode выглядит так:

```cpp
template<typename ReturnT, typename... Args>
ReturnT visit(BasicNode const & node, Args... args)
```

То есть, если вы специализировали visit для нужных типов нод, вы можете использовать для BasicNode visit с соответствующей сигнатурой.\
Например:

```cpp

using dumpable = void(unique_node_id_t, std::ofstream&);

SPECIALIZE_CREATE(Print, dumpale)

template <>
void visit(Print const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Print", "style=filled, fillcolor=\"lightyellow\"");

    for (auto&& arg : node)
        graphic_dump::dump_and_link_with_parent(os, unique_node_id, arg);
}

void dump(BasicNode const & node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    /* select necessary visit specialization */
    return visit<void, unique_node_id_t, std::ofstream&>(node, unique_node_id, os);
}

/* ... */

auto&& node = create(Print{/*args*/});
auto&& ofs = std::ofstream{/*args*/};
dump(node, &node, ofs);
```

## Получается нужно при первом построении AST сразу указывать все поддерживаемые сигнатуры?

Можно и так, но можно обойтись и без этого :)

Реализован шаблон функции:

```cpp
export
template <typename NodeT>
BasicNode create(NodeT)
{
    /*
    use this like:
    return BasicNode::Actions<__your_signatures__>::create(node)
    */
    static_assert(false, "using unspecialized create.");
};
```

Эта функция вызывается в read. Поэтому если в единице траснялции специализированы шаблоны для нужных нод, то они и будут использованы. Про read отдельно поговорим далее, это довольно неприятный момент.\
**Единственный правильный** способ специализации create это:

```cpp
template <>
last::node::BasicNode last::node::create(NodeT node)
{
    return last::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node));
}
```

Для этого реализован макрос в `include/create-basic-node.hpp`:

```cpp
#define SPECIALIZE_CREATE(NodeT, ...)                                            \
template <>                                                                      \
last::node::BasicNode last::node::create(NodeT node)                             \
{                                                                                \
    return last::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node)); \
}
```

Если же нужно специализировать для всех нод все одинаково, то в том же `include/create-basic-node.hpp`:

```cpp

#define CREATE_SAME(...)                                                                                                           \
SPECIALIZE_CREATE(Scope           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Print           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Scan            , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(UnaryOperator   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(BinaryOperator  , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(If              , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Else            , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Condition       , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(While           , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(NumberLiteral   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(StringLiteral   , __VA_ARGS__)                                                                                   \
SPECIALIZE_CREATE(Variable        , __VA_ARGS__)
```

Почему не отдельная шаблонная функция? - потому что вызов из под read должен быть универсален, а как либо перегрузить create для возможнстей и специализировать для всех разом, и для каждой ноды отдельно - увы, не предяставляется возможным.

<br>

Да, я каюсь, я прибегнул к ужасным методам, но в свое оправдание скажу - можно обойтись и без них, просто надо будет больше писать однотипного кода. Использовать макросы или нет - Ваш выбор :)

## О неприятном

Как и обещал, возвращаемся к read. Данная функция предназначена для чтения специального тектового представления дерева. Как Вы помните, для построения ноды нужно указывать нужные сигнатуры visit. Так как read библиотечная функция, то она как то должна получать нужную специализацию функции create. Поэтому:

1) данная функция реализована в хедере
2) хедер `read-ast.hpp` следует инклюдить после специализаций create. Да, это довольно сомнительное решение. Но для первой версии библиотеки это рабочее решение. В дальнейшем будем думать что с этим сделать.

*Сейчас кажется, что это просто плата за универсальность BasicNode.*

## Установка и сборка

## Как библиотеку

```shell
git clone https://github.com/Maksim-Sebelev/THE-LAST;
cd THE-LAST;
cmake -S . -B build
cmake --build build --target install
```

## Как сабмодуль в ваш git проект

```shell
git submodule add https://github.com/Maksim-Sebelev/THE-LAST <your third-patry directory>
```

в CMakeLists.txt:

```cmake
add_subdirectory(<your third-patry directory>)

# ===== #

target_link_libraries(<your target>
    # PUBLIC/PRIVATE
    TheLast::TheLast
)
```

## Зависимости

Boost

## Интерфейс класса AST

```cpp
class AST
{
public:
    /* may be too long, because all tree will copy */
    AST(node::BasicNode const & root);
    AST(node::BasicNode&& root);

public:
    node::BasicNode const &root() const noexcept;
};
```

AST реализует прокси класс для хранения корня дерева. Умеет конструироваться из BasicNode, путями копирования и перемещения  (при первом будь осторожны, все подноды будут скопированые рекурсивно!) и даваться константную ссылку на корень.

## Функции для работы с AST, предоставляемые данной библиотекой

```cpp
void last::write(last::AST const & ast, std::filesystem::path const &file);
```

write реализует запись ast (аргумент ast) в файл (аргумет file) в специальном текстовом формате. Является парной для read.

[Пример такой записи](./assets/ast-text-format-example.txt)

<br>

```cpp
last::AST last::read(std::filesystem::path const & ast_txt);
```

read реализует чтение ast из текстового представления (аргумент ast_txt). Является парной для write.

<br>

```cpp
void dump(last::AST const & ast, std::filesystem::path const & dot_file, std::filesystem::path const & img_file = "");
```

dump реализует дамп ast (аргумент ast) в формате [Graphviz](https://graphviz.org/). Дамп происходит в файл dot_file. Если аргумент img_file являтся пустой строкой (значение по умолчанию), то будет просто произведена генерация файла dot_file. Если же img_file не является пустой строкой, то будет сгенерирован файл с изображением ast. Это будет сделано путем явного вызова graphviz.

<br>

```cpp
template <typename... Signatures>
bool last::node::BasicNode::support();
```

Возврщаает true, если нода поддерживает все сигнатуры, иначе false.

<br>

```cpp
template <typename T>
bool last::node::BasicNode::is_a();
```

Возврщаает true, если нода была инициализирована объектом типа T, иначе false.

<br>