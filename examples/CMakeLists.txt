cmake_minimum_required(VERSION 3.10)

add_executable(example_find_export example_find_export.cpp)
target_include_directories(example_find_export PRIVATE ../include)
target_link_libraries(example_find_export pe)

add_executable(example_print_sections example_print_sections.cpp)
target_include_directories(example_print_sections PRIVATE ../include)
target_link_libraries(example_print_sections pe)

add_executable(example_print_exports example_print_exports.cpp)
target_include_directories(example_print_exports PRIVATE ../include)
target_link_libraries(example_print_exports pe)

add_executable(example_print_imports example_print_imports.cpp)
target_include_directories(example_print_imports PRIVATE ../include)
target_link_libraries(example_print_imports pe)
