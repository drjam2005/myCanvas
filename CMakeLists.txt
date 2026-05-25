cmake_minimum_required(VERSION 3.28)
project(myCanvas LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(exec myCanvas)

set(src
    src/main.cpp
    src/canvas.cpp
    src/SDLHandler.cpp
)

add_executable(${exec} ${src})

target_include_directories(${exec} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if(WIN32)
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/SDL/windows/lib
        ${CMAKE_SOURCE_DIR}/vendor/raylib/windows/lib
    )
	target_include_directories(${exec} PRIVATE
		${CMAKE_SOURCE_DIR}/vendor/raylib/windows/include/
		${CMAKE_SOURCE_DIR}/vendor/SDL/windows/include/
	)

    target_compile_definitions(${exec} PRIVATE SDL_STATIC_LIB)
    target_link_libraries(${exec} PRIVATE
        raylib
		SDL::SDL3
        m
        gdi32
        winmm
        imm32
        ole32
        oleaut32
        version
        uuid
        advapi32
        setupapi
        shell32
        dinput8
        winspool
        comdlg32
    )
elseif(UNIX)
    target_link_directories(${exec} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/SDL/unix/lib
        ${CMAKE_SOURCE_DIR}/vendor/raylib/unix/lib
    )
    target_compile_definitions(${exec} PRIVATE SDL_STATIC_LIB)
	target_include_directories(${exec} PRIVATE
		${CMAKE_SOURCE_DIR}/vendor/raylib/unix/include/
		${CMAKE_SOURCE_DIR}/vendor/SDL/unix/include/
	)
    target_link_libraries(${exec} PRIVATE
        raylib
        SDL3
        m
        pthread
        dl
    )
endif()
