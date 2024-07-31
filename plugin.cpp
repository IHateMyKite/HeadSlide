#include "include/HeadSlide.h"
#include "include/Papyrus.h"
#include <spdlog/sinks/basic_file_sink.h>

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}



SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SetupLog();

    SKSE::GetMessagingInterface()->RegisterListener("SKSE",[](SKSE::MessagingInterface::Message *message) 
    {
        switch (message->type)
        {
            // Game loaded
            case 3:
            {
                HESL::Config::GetSingleton()->Setup();
                HESL::HeadSlide::GetSingleton()->Init();
            }
        }
    });

    SKSE::GetPapyrusInterface()->Register(HESL::RegisterPapyrusFunctions);

    return true;
}