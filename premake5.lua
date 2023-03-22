workspace "Chaos"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Chaos"

    location(_OPTIONS["to"] or 'Build/' .. _ACTION)

function common_settings()
    architecture "x64"

    targetdir("Build/Bin/%{cfg.buildcfg}/%{prj.name}")
    objdir("Build/Bin-Int/%{cfg.buildcfg}/%{prj.name}")

    rtti "Off"
    justmycode "Off"
    editandcontinue "Off"
    exceptionhandling "Off" -- SEH still works

    characterset "Unicode"	

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"
    
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter {}
end

group "Dependencies"
    include "Third Party/glad"
    include "Third Party/glfw"
    include "Third Party/imgui"
group ""

group "Core"
    project "Chaos"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        common_settings()

        files {
            "src/**.h",
            "src/**.cpp"
        }

        includedirs {
            "src",
            "Third Party/glad/include",
            "Third Party/glfw/include",
            "Third Party/stb/include",
            "Third Party/imgui/"
        }

        links {
            "glad",
            "GLFW",
            "ImGui"
        }

        postbuildcommands {
            "{COPY} \"" .. _WORKING_DIR .. "/src/data/\" \"%{cfg.targetdir}/data/\""
        }
group ""
