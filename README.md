![C++](https://img.shields.io/badge/C++-23-blue?style=for-the-badge&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.17+-green?style=for-the-badge&logo=cmake)
![Testing](https://img.shields.io/badge/Google_Test-Framework-red?style=for-the-badge&logo=google)

# ParaCL

Тот самый младший брат языка C78, который только только начинает свой путь.  

bison -d src/parser.y -o src/parser.tab.cpp
flex -o src/lexer.yy.cpp src/lexer.l
g++ -Iinc src/parser.tab.cpp src/lexer.yy.cpp src/main.cpp -o paracl -lfl
