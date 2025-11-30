module;

//---------------------------------------------------------------------------------------------------------------

#include <filesystem>
#include <iostream>
#include <string>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

//---------------------------------------------------------------------------------------------------------------

export module spdlog_init;

//---------------------------------------------------------------------------------------------------------------

namespace spdlog
{

//---------------------------------------------------------------------------------------------------------------

export void init_spdlogger()
try
{
#if not defined(LOG_DIR)
#define LOG_DIR "."
#warning "Log out dir not given. using defaul: ."
#endif /* not defined(LOG_DIR) */

    std::filesystem::path log_file_path(std::string(LOG_DIR) + std::string("/global.log"));
    std::filesystem::remove(log_file_path); /* remove old logs */

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_path.string());
    auto logger = std::make_shared<spdlog::logger>("global", file_sink);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [%s:%#] %v");

    /*
        я в душен не чаю зачем нужна следующая строчка кода есть spdlog::set_default_logger(logger); в 36 строке
        мне ее сказал дипсик и я вставил ее
        почему тыо без нее в main даже после вызывова иницилизатора LOGINFO херачит в консоль.
        причем только в main и только в релизной версии. я хз короче, нейросети рулят.
        (c) Себелев Максим
    */

    spdlog::details::registry::instance().set_default_logger(logger);
}
catch (const spdlog::spdlog_ex &ex)
{
    std::cerr << "spdlog setup failed: " << ex.what() << "\n";
}
catch (...)
{
    std::cerr << "spdlog unknown exception\n";
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace spdlog */

//---------------------------------------------------------------------------------------------------------------
