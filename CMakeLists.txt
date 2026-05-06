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
	target_include_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/raylib/windows/include
	)
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/raylib/windows/lib
    )
    target_link_libraries(${exec} PRIVATE
        winmm
        gdi32
        opengl32
        shell32
        user32
		raylib
    )
else()
	target_include_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/raylib/unix/include
	)
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/raylib/unix/lib
    )
    target_link_libraries(${exec} PRIVATE
        m
        pthread
        dl
        X11
		raylib
    )
endif()

