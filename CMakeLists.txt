cmake_minimum_required(VERSION 3.28)
project(myCanvas LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(exec myCanvas)

set(src
    src/main.cpp
    src/canvas.cpp
    src/helpers.cpp
	src/canvas_misc.cpp
    src/canvas_update.cpp
    src/canvas_render.cpp
    src/events.cpp
    src/SDLHandler.cpp
)

add_executable(${exec} ${src})

target_include_directories(${exec} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(${exec} PRIVATE
	-O3
	-static
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
        libraylib.a
		SDL3
		m
		hid
		setupapi
		winmm
		imm32
		version
		ole32
		oleaut32
		uuid
		advapi32
		user32
		gdi32
		shell32
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
        libraylib.a
        SDL3
        m
        pthread
        dl
		X11
		Xi
    )
endif()
