workspace "Chaos"
    architecture "x64"
    configurations { "Debug", "DebugOptimized", "Release" }
    location(_OPTIONS["to"] or "build/" .. _ACTION)

    filter { "action:gmake2" }
        toolset "clang"
    filter {}

    startproject "chaos"

OUT_DIR = "build/bin/%{cfg.buildcfg}/%{prj.name}"
INT_DIR = "build/bin-int/%{cfg.buildcfg}/%{prj.name}"

group "dependencies"
    include "third-party/light-std/premake5-lstd"
    include "third-party/imgui"
    include "third-party/glfw"
    include "third-party/glad"
group ""

group "core"
    include "premake5-chaos"
group ""
