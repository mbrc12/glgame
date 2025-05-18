cmake_minimum_required(VERSION 3.30)
project(GLGame)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/backward-cpp)
add_subdirectory(deps/glfw)
add_subdirectory(deps/freetype)

set(IMGUI_FILES
	deps/imgui/imgui.cpp
	deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/imgui_tables.cpp
	deps/imgui/backends/imgui_impl_glfw.cpp
	deps/imgui/backends/imgui_impl_opengl3.cpp
)

set(ENGINE_FILES
    src/engine/shader.cpp
)

set(GAME_FILES
    src/main.cpp
)

set(DEPENDENCY_FILES
    deps/glad/src/glad.c
    ${IMGUI_FILES}
)

set(SOURCE_FILES
    ${DEPENDENCY_FILES} 
    ${ENGINE_FILES}	    	# engine source
    ${GAME_FILES}
)

set(INCLUDE_DIRS
	src/

	deps/glad/include/
	deps/imgui/
	deps/imgui/backends/
    deps/glm/
)

add_executable(main ${SOURCE_FILES})
target_include_directories(main PRIVATE ${INCLUDE_DIRS})
target_compile_features(main PRIVATE cxx_std_23)
target_compile_options(main PRIVATE "-O0")

set(LIBS
	glfw
    freetype
	GL
    Backward::Object
)

target_link_libraries(main PRIVATE ${LIBS})
