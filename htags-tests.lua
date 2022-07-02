include "htags-common.lua"

project "hierarchical-tags-tests"
        kind "ConsoleApp"
        language "C"
        cdialect "C17"
        exceptionhandling (EXCEPTIONS_ENABLED)
        rtti "Off"
        staticruntime (STATIC_RUNTIME)
        files
        {
            path.join(TESTS_DIR, "*.c"),
            path.join(TESTS_DIR, "*.h")
        }
        includedirs
        {
            (INCLUDE_DIR)
        }
        links { "hierarchical-tags-lib" }

