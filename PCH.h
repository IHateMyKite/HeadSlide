#pragma once

#pragma warning( disable : 4100 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4245 )
#pragma warning( disable : 6031 )
//#pragma warning( disable : 26444 )

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <Windows.h>

using namespace std::literals;

#define SINGLETONHEADER(cname)                          \
        public:                                         \
            cname(cname &) = delete;                    \
            void operator=(const cname &) = delete;     \
            static cname* GetSingleton();               \
        protected:                                      \
            cname(){}                                   \
            ~cname(){}                                  \
            static cname* _this;

#define SINGLETONBODY(cname)                            \
        cname * cname::_this = new cname;               \
        cname * cname::GetSingleton(){return _this;}

#include "Config.h"

#if (1)
    #define LOG(...)    {if (HESL::Config::GetSingleton()->GetVariable<int>("General.iLogging",1) >= 2) auto log_l = SKSE::log::info(__VA_ARGS__);}
    #define WARN(...)   {if (HESL::Config::GetSingleton()->GetVariable<int>("General.iLogging",1) >= 1) auto log_w = SKSE::log::warn(__VA_ARGS__);}
    #define ERROR(...)  {auto log_e = SKSE::log::error(__VA_ARGS__);}
    #define DEBUG(...)  {auto log_d = SKSE::log::debug(__VA_ARGS__);}
#else
    #define LOG(...)    {}
    #define WARN(...)   {}
    #define ERROR(...)  {}
    #define DEBUG(...)  {}
#endif


#define PAPYRUSFUNCHANDLE RE::StaticFunctionTag* a_psft