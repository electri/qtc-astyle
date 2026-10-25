# Standalone test for the formatting core of the Astyle plugin.
#
# It is *not* part of the Qt Creator build: it compiles the plugin's
# astyleformatter.cpp together with the bundled Artistic Style sources and
# checks the option switches, line ending handling and encoding handling.
#
# Usage (from a shell with a MSVC/GCC environment at hand):
#
#   cmake -S <qtcreator>/src/plugins/astyle/tests/standalone \
#         -B <build dir> -G Ninja -DCMAKE_PREFIX_PATH=<Qt 6 prefix>
#   cmake --build <build dir>
#   <build dir>/astyle-core-test
#
cmake_minimum_required(VERSION 3.20)
project(astyle-core-test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

get_filename_component(PLUGIN_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_subdirectory("${PLUGIN_DIR}/3rdparty/astyle" astyle)

add_executable(astyle-core-test
    main.cpp
    "${PLUGIN_DIR}/astyleformatter.cpp"
)

target_include_directories(astyle-core-test PRIVATE
    "${PLUGIN_DIR}"
    "${PLUGIN_DIR}/3rdparty/astyle"
)
target_compile_definitions(astyle-core-test PRIVATE ASTYLE_LIB ASTYLE_NO_EXPORT)
target_link_libraries(astyle-core-test PRIVATE Qt6::Core AstyleLib)
