# It's recommended to set a minimum CMake version.
# If you use CMake features from higher versions, update this to match.
cmake_minimum_required(VERSION 3.21)

# Set your project name. This will be the name of your SKSE .dll file.
project(HeadSlide VERSION 1.0.0 LANGUAGES CXX)

# === Set source files ===
# Add new .cpp here, otherwise the build will end with link error
set(source_files ${source_files}
    src/HeadSlide.cpp
    src/Config.cpp
    src/Papyrus.cpp
)

# Setup your SKSE plugin as an SKSE plugin!
find_package(CommonLibSSE CONFIG REQUIRED)
add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${source_files} plugin.cpp) # <--- specifies plugin.cpp
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23) # <--- use C++23 standard
target_precompile_headers(${PROJECT_NAME} PRIVATE PCH.h) # <--- PCH.h is required!
target_include_directories(${PROJECT_NAME} PRIVATE include)
