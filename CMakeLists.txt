cmake_minimum_required(VERSION 3.30)
project(GLGame)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(deps/assimp)
add_subdirectory(deps/backward-cpp)
add_subdirectory(deps/freetype)
add_subdirectory(deps/glfw)

set(IMGUI_FILES
	deps/imgui/backends/imgui_impl_glfw.cpp
	deps/imgui/backends/imgui_impl_opengl3.cpp
	deps/imgui/imgui.cpp
	deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
)

set(DEPENDENCY_FILES
    deps/glad/src/glad.c
    ${IMGUI_FILES}
)

set(ENGINE_FILES
    src/engine/mesh.cpp
    src/engine/shader.cpp
    src/engine/shapes.cpp
    src/engine/texture.cpp
)

set(GAME_FILES
    src/common.cpp
    src/main.cpp
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
    deps/stb/
)

add_executable(main ${SOURCE_FILES})
target_include_directories(main PRIVATE ${INCLUDE_DIRS})
target_compile_features(main PRIVATE cxx_std_23)
target_compile_options(main PRIVATE "-O0")

set(LIBS
	GL
	glfw
    Backward::Object
    assimp
    freetype
)

target_link_libraries(main PRIVATE ${LIBS})
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
