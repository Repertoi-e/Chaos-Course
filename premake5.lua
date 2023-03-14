workspace "Chaos"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Week 2"

    location(_OPTIONS["to"] or 'Build/' .. _ACTION)

function common_settings()
    architecture "x64"

    targetdir("Bin/%{cfg.buildcfg}/%{prj.name}")
    objdir("Bin/%{cfg.buildcfg}/%{prj.name}/int")

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

project "glad"
    common_settings()

    kind "StaticLib"
    language "C"

    files {
        "Third Party/glad/src/glad.c"
    }
    
    includedirs {
        "Third Party/glad/include"
    }

project "Week 2"
    common_settings()

    kind "ConsoleApp"
    language "C++"
    
    cppdialect "C++17"

    files {
        "Week 2/**.h",
        "Week 2/**.cpp"
    }

    includedirs {
        "Week 2",
        "Third Party/glad/include",
        "Third Party/glfw/include",
        "Third Party/stb/include"
    }

    links {
        "glad",
        "Third Party/glfw/lib-vc2022/glfw3dll.lib"
    }

    postbuildcommands {
        "{COPY} \"" .. _WORKING_DIR .. "/Week 2/data/\" \"%{cfg.targetdir}/data/\"",
        "{COPY} \"" .. _WORKING_DIR .. "/Third Party/glfw/lib-vc2022/glfw3.dll\" \"%{cfg.targetdir}\""
    }