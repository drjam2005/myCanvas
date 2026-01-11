cmake_minimum_required(VERSION 3.28)
project(myCanvas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(exec myCanvas)

set(src
    src/main.cpp
    src/canvas.cpp
)

add_executable(${exec} ${src})

target_include_directories(${exec} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if (WIN32)
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/windows
    )
else()
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/unix
    )
endif()

target_link_libraries(${exec} PRIVATE raylib)

# ---- Platform-specific deps ----
if (WIN32)
    target_link_libraries(${exec} PRIVATE
        winmm
        gdi32
        opengl32
        shell32
        user32
    )

    add_custom_command(
        TARGET ${exec} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/lib/windows/raylib.dll
            $<TARGET_FILE_DIR:${exec}>
    )

elseif (UNIX)
    target_link_libraries(${exec} PRIVATE
        m
        pthread
        dl
        GL
        X11
    )
endif()
