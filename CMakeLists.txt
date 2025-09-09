cmake_minimum_required(VERSION 3.10)

# 在build目录下执行 cmake -G "MinGW Makefiles" ..

# 指定编译器mingw
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(GameEngine)

set(CMAKE_CXX_STANDARD 17)

message(${CMAKE_SOURCE_DIR})

# sdl2模块的cmake路径
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/deps/SDL2/cmake)
set(SDL2_IMAGE_DIR ${CMAKE_SOURCE_DIR}/deps/SDL2_image/cmake)
set(SDL2_TTF_DIR ${CMAKE_SOURCE_DIR}/deps/SDL2_ttf/cmake)

list(APPEND CMAKE_MODULE_PATH ${SDL2_DIR} ${SDL2_IMAGE_DIR} ${SDL2_TTF_DIR})

# 查找这些依赖库的cmake
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# entt目录下有CMakelists.txt，才能使用add_subdirectory作为子模块使用
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/entt)

message(${SDL2_IMAGE_INCLUDE_DIRS})
message(${SDL2_TTF_INCLUDE_DIRS})

# 包含目录
include_directories(
    include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    # project include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/input
    ${CMAKE_SOURCE_DIR}/src/utils
    # imgui
    ${CMAKE_SOURCE_DIR}/deps/imgui
    ${CMAKE_SOURCE_DIR}/deps/imgui/backends
)

# 添加可执行文件
add_executable(GameEngine
    src/Engine.cpp
    src/Systems.cpp
    src/Resources.cpp
    src/Editor.cpp
    src/input/Keyboard.cpp
    src/Scene.cpp
    src/SceneManager.cpp
    main.cpp
    # ImGui 文件
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    # ImGui SDL2 后端
    deps/imgui/backends/imgui_impl_sdl2.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# 这些变量都在cmake里定义了
message(${SDL2_LIBDIR})
message(${SDL2_IMAGE_LIBRARIE_DIRS})
message(${SDL2_TTF_LIBRARIE_DIRS})

# 库目录
link_directories(
    ${SDL2_LIBDIR}
    ${SDL2_IMAGE_LIBRARIE_DIRS}
    ${SDL2_TTF_LIBRARIE_DIRS}
)

find_library(SDL2_LIBRARY SDL2 PATHS ${SDL2_LIBDIR})
find_library(SDL2_IMAGE_LIBRARY SDL2_image PATHS ${SDL2_IMAGE_LIBRARIE_DIRS})
find_library(SDL2_TTF_LIBRARY SDL2_ttf PATHS ${SDL2_TTF_LIBRARIE_DIRS})
find_library(SDL2MAIN_LIBRARY SDL2main PATHS ${SDL2_LIBDIR})

if (NOT SDL2_LIBRARY)
    message(FATAL_ERROR "SDL2 library not found")
endif()

# 链接库
target_link_libraries(GameEngine
    ${SDL2MAIN_LIBRARY}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    EnTT::EnTT
)

# mingw 需要链接一些win系统库
if(WIN32)
    message("platform is win")
    target_link_libraries(GameEngine
        # windows 系统库
        setupapi
        winmm
        imm32
        version
        ole32
        oleaut32
        shell32
        gdi32
        user32
        advapi32
        rpcrt4     # 用于 UuidCreate
    )
endif()

# 运行GameEngine.exe 需要 sdl2这些库的dll，将bin目录添加到环境变量PATH里即可