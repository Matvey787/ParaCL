#include <gtest/gtest.h>
#include <filesystem>
#include <vector>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>

import compileOpts;
import general;
import thelast;

namespace fs = std::filesystem;



// ───────────────────────────────────────────────
// Группа тестов: Логика парсинга и генерации AST (general)
// ───────────────────────────────────────────────

class GeneralLogicTest : public ::testing::Test {
protected:
    std::string validFile = "valid_test.pcl";
    std::string syntaxErrorFile = "syntax_error.pcl";

    void SetUp() override {
        // Создаем валидный файл
        std::ofstream f_valid(validFile);
        f_valid << "print(42);\n";
        f_valid << "a = 10;\n";
        f_valid << "if (a > 5) { print(a); }\n";
        f_valid.close();

        // Создаем файл с синтаксической ошибкой
        std::ofstream f_error(syntaxErrorFile);
        f_error << "print(42;\n";  // пропущена скобка
        f_error << "a = 10\n";
        f_error.close();
    }

    void TearDown() override {
        fs::remove(validFile);
        fs::remove(syntaxErrorFile);
        fs::remove("non_existent.pcl");
    }
};

TEST_F(GeneralLogicTest, GenerateAST_ValidFile_Success) {
    // Этот тест должен покрыть строки 23-41 (успешный путь)
    EXPECT_NO_THROW({
        last::AST ast = ParaCL::general::generateAST(validFile);
        EXPECT_TRUE(true);  // Если дошли сюда - успех
    });
}

TEST_F(GeneralLogicTest, GenerateAST_FileNotFound_Throws) {
    // Этот тест должен покрыть строки 28-29 (ошибка открытия файла)
    EXPECT_THROW({
        ParaCL::general::generateAST("non_existent.pcl");
    }, std::runtime_error);
}

TEST_F(GeneralLogicTest, GenerateAST_SyntaxError_Throws) {
    // Этот тест должен покрыть строки 36-37 (ошибка парсинга)
    EXPECT_THROW({
        ParaCL::general::generateAST(syntaxErrorFile);
    }, std::runtime_error);
}

TEST_F(GeneralLogicTest, InitLogging_NoCrash) {
    // Этот тест должен покрыть строки 46-53
    EXPECT_NO_THROW({
        ParaCL::general::init_logging();
    });
}

// ───────────────────────────────────────────────
// Вспомогательная инфраструктура для захвата вывода
// ───────────────────────────────────────────────
namespace {

struct CapturedOutput {
    std::string stdout_str;
    std::string stderr_str;
    int exit_code = 0;
};

CapturedOutput run_handle_with_args(std::vector<std::string> args) {
    // Добавляем имя программы в начало
    args.insert(args.begin(), "paracl-front");

    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for (auto& s : args) {
        argv.push_back(s.data());
    }
    argv.push_back(nullptr);
    int argc = static_cast<int>(args.size());

    // Перенаправляем stdout и stderr во внутренние буферы
    std::stringstream cout_buf, cerr_buf;
    std::streambuf* old_cout = std::cout.rdbuf(cout_buf.rdbuf());
    std::streambuf* old_cerr = std::cerr.rdbuf(cerr_buf.rdbuf());

    bool exception_thrown = false;
    
    try {
        auto data = ParaCL::general::handleCompileOpts(argc, argv.data());
        (void)data; 
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::cerr << e.what() << std::endl;
    } catch (...) {
        exception_thrown = true;
    }

    // Возвращаем стандартные потоки на место
    std::cout.rdbuf(old_cout);
    std::cerr.rdbuf(old_cerr);

    return {cout_buf.str(), cerr_buf.str(), exception_thrown ? 1 : 0};
}

}  // namespace

// ───────────────────────────────────────────────
// Группа тестов: Обработка аргументов командной строки (compileOpts)
// ───────────────────────────────────────────────

TEST(CompileOpts, NoInputFiles_Throws) {
    auto res = run_handle_with_args({});
    EXPECT_EQ(res.exit_code, 1);
}

TEST(CompileOpts, ShowHelp_PrintsHelp) {
    auto res = run_handle_with_args({"-h"});
    EXPECT_FALSE(res.stdout_str.empty());
    EXPECT_TRUE(res.stdout_str.find("OVERVIEW") != std::string::npos || 
                res.stdout_str.find("options") != std::string::npos);
}

TEST(CompileOpts, ShowVersion_PrintsVersion) {
    auto res = run_handle_with_args({"-v"});
    EXPECT_FALSE(res.stdout_str.empty());
    EXPECT_TRUE(res.stdout_str.find("ParaCL") != std::string::npos);
}

TEST(CompileOpts, ExplicitOutputs_WrongCount_Throws) {
    auto res = run_handle_with_args({"-o", "out1.json", "in1.cl", "in2.cl"});
    EXPECT_EQ(res.exit_code, 1);
}

TEST(CompileOpts, SingleInputNoOutput_Works) {
    auto res = run_handle_with_args({"file.cl"});
    EXPECT_EQ(res.exit_code, 0);
}

TEST(CompileOpts, MultipleInputsNoOutput_Works) {
    auto res = run_handle_with_args({"file1.cl", "file2.cl"});
    EXPECT_EQ(res.exit_code, 0);
}

TEST(CompileOpts, OutputDirectory_Works) {
    fs::create_directory("test_out_dir");
    auto res = run_handle_with_args({"-o", "test_out_dir", "file1.cl", "file2.cl"});
    EXPECT_EQ(res.exit_code, 0);
    fs::remove_all("test_out_dir");
}

TEST(CompileOpts, ExplicitOutputs_Works) {
    auto res = run_handle_with_args({"-o", "out1.json", "-o", "out2.json", "file1.cl", "file2.cl"});
    EXPECT_EQ(res.exit_code, 0);
}

// ───────────────────────────────────────────────
// Интеграционные тесты - связка handleCompileOpts + generateAST
// ───────────────────────────────────────────────

TEST(IntegrationTest, FullPipeline_WithValidFile) {
    // Создаем тестовый файл
    std::string testFile = "integration.pcl";
    std::ofstream f(testFile);
    f << "x = 42;\n";
    f << "print(x);\n";
    f.close();
    
    // Сначала парсим аргументы
    std::vector<std::string> args = {testFile};
    args.insert(args.begin(), "paracl-front");
    
    std::vector<char*> argv;
    for (auto& s : args) {
        argv.push_back(s.data());
    }
    argv.push_back(nullptr);
    int argc = static_cast<int>(args.size());
    
    // Вызываем handleCompileOpts
    auto cmdData = ParaCL::general::handleCompileOpts(argc, argv.data());
    
    // Затем вызываем generateAST для входного файла
    EXPECT_NO_THROW({
        last::AST ast = ParaCL::general::generateAST(cmdData.inputFiles[0].string());
    });
    
    fs::remove(testFile);
}

// ───────────────────────────────────────────────
// Main
// ───────────────────────────────────────────────

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 