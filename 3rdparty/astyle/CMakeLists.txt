# Artistic Style (AStyle) as a static library, built into the plugin.
#
# Upstream: https://gitlab.com/saalen/astyle  (MIT license, see LICENSE.md)
# Version:  see VERSION.txt
#
# The sources are compiled with ASTYLE_LIB defined, which strips the console
# front end (main(), ASConsole) and keeps the library API: ASFormatter,
# ASBeautifier, ASEnhancer, ASOptions and ASStreamIterator.

add_library(AstyleLib STATIC
    ASBeautifier.cpp
    ASEnhancer.cpp
    ASFormatter.cpp
    ASLocalizer.cpp
    ASLocalizer.h
    ASResource.cpp
    astyle.h
    astyle_main.cpp
    astyle_main.h
    LICENSE.md
    VERSION.txt
)

target_compile_definitions(AstyleLib PRIVATE ASTYLE_LIB)

set_target_properties(AstyleLib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

if (MSVC)
    target_compile_options(AstyleLib PRIVATE /utf-8)
    # The upstream sources are not warning clean with /W4.
    target_compile_options(AstyleLib PRIVATE /W0)
else()
    # Third party code: do not drown the build log in its warnings.
    target_compile_options(AstyleLib PRIVATE -w)
endif()
