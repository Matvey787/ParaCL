#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#if defined(LOGGER)
import spdlog_init;
#endif /* defined(LOGGER) */

import options_parser;

using namespace Options;

#define CREATE_OP(...)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        char *argv[] = {const_cast<char *>("exe"), __VA_ARGS__, NULL};                                                 \
        int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;                                                          \
        po = OptionsParser(argc, argv).get_program_options();                                                          \
    } while (0)

TEST(options_paser, find_sources)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));

    EXPECT_EQ(po.sources.size(), 1);
    EXPECT_EQ(po.sources[0].string(), "some.cl");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-c"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--compile"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("second.o"));

    EXPECT_EQ(po.sources.size(), 5);
    EXPECT_EQ(po.sources[0].string(), "1.cl");
    EXPECT_EQ(po.sources[1].string(), "2.cl");
    EXPECT_EQ(po.sources[2].string(), "3.cl");
    EXPECT_EQ(po.sources[3].string(), "4.cl");
    EXPECT_EQ(po.sources[4].string(), "5.cl");
}

TEST(options_paser, find_objects)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));

    EXPECT_EQ(po.object_files.size(), 1);
    EXPECT_EQ(po.object_files[0].string(), "some.o");

    CREATE_OP(const_cast<char *>("--compile"), const_cast<char *>("1.cl"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("4.cl"), const_cast<char *>("5.cl"));

    EXPECT_EQ(po.object_files.size(), 5);
    EXPECT_EQ(po.object_files[0].string(), "1.o");
    EXPECT_EQ(po.object_files[1].string(), "2.o");
    EXPECT_EQ(po.object_files[2].string(), "3.o");
    EXPECT_EQ(po.object_files[3].string(), "4.o");
    EXPECT_EQ(po.object_files[4].string(), "5.o");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-c"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--compile"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("second.o"));

    EXPECT_EQ(po.object_files.size(), 5);
    EXPECT_EQ(po.object_files[0].string(), "first.obj");
    EXPECT_EQ(po.object_files[1].string(), "second.o");
    EXPECT_EQ(po.object_files[2].string(), "3.o");
    EXPECT_EQ(po.object_files[3].string(), "4.o");
    EXPECT_EQ(po.object_files[4].string(), "5.o");
}

TEST(options_paser, find_ir)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));

    EXPECT_EQ(po.llvm_ir_files.size(), 1);
    EXPECT_EQ(po.llvm_ir_files[0].string(), "some.ll");

    CREATE_OP(const_cast<char *>("--compile"), const_cast<char *>("1.cl"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("4.cl"), const_cast<char *>("5.cl"));

    EXPECT_EQ(po.llvm_ir_files.size(), 5);
    EXPECT_EQ(po.llvm_ir_files[0].string(), "1.ll");
    EXPECT_EQ(po.llvm_ir_files[1].string(), "2.ll");
    EXPECT_EQ(po.llvm_ir_files[2].string(), "3.ll");
    EXPECT_EQ(po.llvm_ir_files[3].string(), "4.ll");
    EXPECT_EQ(po.llvm_ir_files[4].string(), "5.ll");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-c"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--compile"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("second.o"));

    EXPECT_EQ(po.llvm_ir_files.size(), 5);
    EXPECT_EQ(po.llvm_ir_files[0].string(), "first.ll");
    EXPECT_EQ(po.llvm_ir_files[1].string(), "second.ll");
    EXPECT_EQ(po.llvm_ir_files[2].string(), "3.ll");
    EXPECT_EQ(po.llvm_ir_files[3].string(), "4.ll");
    EXPECT_EQ(po.llvm_ir_files[4].string(), "5.ll");
}

TEST(options_paser, find_progam_name)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));
    EXPECT_EQ(po.program_name, "exe");

    CREATE_OP(const_cast<char *>("--compile"), const_cast<char *>("1.cl"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("4.cl"), const_cast<char *>("5.cl"));
    EXPECT_EQ(po.program_name, "exe");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-c"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--compile"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("second.o"));
    EXPECT_EQ(po.program_name, "exe");
}

TEST(options_paser, find_executable_name)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));
    EXPECT_EQ(po.executable_file, "a.out");

    CREATE_OP(const_cast<char *>("--compile"), const_cast<char *>("1.cl"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("4.cl"), const_cast<char *>("5.cl"));
    EXPECT_EQ(po.executable_file, "a.out");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-o"), const_cast<char *>("govno"),
              const_cast<char *>("-c"), const_cast<char *>("2.cl"), const_cast<char *>("3.cl"),
              const_cast<char *>("--compile"), const_cast<char *>("4.cl"), const_cast<char *>("5.cl"),
              const_cast<char *>("first.obj"), const_cast<char *>("second.o"));
    EXPECT_EQ(po.executable_file, "govno");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-o"), const_cast<char *>("govno"),
              const_cast<char *>("-c"), const_cast<char *>("2.cl"), const_cast<char *>("3.cl"),
              const_cast<char *>("--compile"), const_cast<char *>("4.cl"), const_cast<char *>("--output=fuck"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("second.o"));
    EXPECT_EQ(po.executable_file, "fuck");
}

#if defined(GRAPHVIZ)
TEST(options_paser, find_dot_file)
{
#if defined(LOGGER)
    spdlog::init_spdlogger();
#endif /* defined(LOGGER) */

    program_options_t po;

    CREATE_OP(const_cast<char *>("some.cl"));

    EXPECT_FALSE(po.ast_dump);

    CREATE_OP(const_cast<char *>("--compile"), const_cast<char *>("1.cl"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--ast-dump=lol"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"));

    EXPECT_TRUE(po.ast_dump);
    EXPECT_EQ(po.dot_file.string(), "lol");

    CREATE_OP(const_cast<char *>("1.cl"), const_cast<char *>("-c"), const_cast<char *>("2.cl"),
              const_cast<char *>("3.cl"), const_cast<char *>("--compile"), const_cast<char *>("4.cl"),
              const_cast<char *>("5.cl"), const_cast<char *>("first.obj"), const_cast<char *>("-d"),
              const_cast<char *>("ast.dot"), const_cast<char *>("second.o"));

    EXPECT_TRUE(po.ast_dump);
    EXPECT_EQ(po.dot_file.string(), "ast.dot");
}
#endif /* defined(GRAPHVIZ) */
