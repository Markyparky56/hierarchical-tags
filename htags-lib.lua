include "htags-common.lua"

project "hierarchical-tags-lib"
        kind "StaticLib"
        language "C"
        cdialect "C17"
        exceptionhandling (EXCEPTIONS_ENABLED)
        rtti "Off"
        staticruntime (STATIC_RUNTIME)
        files
        {
            path.join(SRC_DIR, "**.c"),
            path.join(INCLUDE_DIR, "**.h")
        }
        includedirs
        {
            (INCLUDE_DIR),
            path.join("cityhash", "src"),
            "xxHash"
        }
        links { "cityhash-c", "libxxhash" }

include "cityhash/cityhash-clib.lua"
include "xxHash/premake_unofficial/xxhash.lua"
