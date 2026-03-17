find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(imgui STATIC
        # Main Imgui files
        libs/imgui/imgui.cpp
        libs/imgui/imgui.h
        libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp
        libs/imgui/imgui_widgets.cpp
        libs/imgui/imgui_demo.cpp
        # backend-specific files
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_glfw.h
        libs/imgui/backends/imgui_impl_opengl3.cpp
        libs/imgui/backends/imgui_impl_opengl3.h)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/ ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/)

add_subdirectory(libs/ImGuiFileDialog)
target_link_libraries(ImGuiFileDialog PRIVATE imgui)

add_executable(pe_viewer
        src/main.cpp
        src/pe_display.h
        src/pe_display.cpp)
target_include_directories(pe_viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/libs)
target_link_libraries(pe_viewer imgui ImGuiFileDialog OpenGL::GL glfw glm::glm pe)
