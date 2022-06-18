-- Append solution type to build dir
local BUILD_DIR = path.join("build/", _ACTION)
-- If specific compiler specified, append that too
if _OPTIONS["cc"] ~= nil then
    BUILD_DIR = BUILD_DIR .. "_" .. _OPTIONS["cc"]
end

newoption {
    trigger = "with-exceptions",
    description = "Exceptions Always Enabled? (ON/OFF)"
}
local EXCEPTIONS_ENABLED = "Off"
if _OPTIONS["with-exceptions"] ~= nil then
    EXCEPTIONS_ENABLED = "On"
end

newoption {
    trigger = "dynamic-runtime",
    description = "Should use dynamically linked runtime?"
}
local STATIC_RUNTIME = "On"
if _OPTIONS["dynamic-runtime"] ~= nil then
    STATIC_RUNTIME = "Off"
end

local SRC_DIR = "src/"
local INCLUDE_DIR = "include/"

workspace "hierarchical-tags"
    location (BUILD_DIR)
    startproject "hierarchical-tags-tests"
    configurations {
        "Debug",
        "Release"
    }
    -- Let 32 bit die already
    if os.is64bit() then
        platforms "x86_64"
    else
        platforms "x86"
    end

    targetdir (BUILD_DIR .. "/bin/" .. "%{cfg.shortname}")
    debugdir "%{cfg.targetdir}"
    objdir (BUILD_DIR .. "/bin/obj/" .. "%{cfg.shortname}")
    filter "configurations:Debug"
        defines
        {
            "_DEBUG"
        }
        optimize "Debug"
        symbols "On"
    filter "configurations:Release"
        defines
        {
            "NDEBUG"
        }
        optimize "Full"
    filter "platforms:x86"
        architecture "x86"
    filter "platforms:x86_64"
        architecture "x86_64"
    filter "action:vs*"
        buildoptions { "/Zc:__cplusplus" }
        flags { "MultiProcessorCompile" }

project "hierarchical-tags-lib"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        exceptionhandling (EXCEPTIONS_ENABLED)
        rtti "Off"
        staticruntime (STATIC_RUNTIME)
        files
        {
            path.join(SRC_DIR, "**.cpp"),
            path.join(INCLUDE_DIR, "**.h")
        }
        includedirs
        {
            (INCLUDE_DIR)
        }
        links { "cityhash-c", "libxxhash" }

-- project "hierarchical-tags-tests"
--         kind ""

include "cityhash/cityhash-clib.lua"
include "xxHash/premake_unofficial/xxhash.lua"
