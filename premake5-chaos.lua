project "chaos"
    kind "ConsoleApp"
    architecture "x64"

    language "C++" 
    cppdialect "C++20"     
	
    characterset "Unicode"
    
    targetdir(OUT_DIR)
    objdir(INT_DIR)

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src",
        "third-party/light-std/include",
        "third-party/glad/include",
        "third-party/glfw/include",
        "third-party/stb/include",
        "third-party/imgui/"
    }

    links {
        "glad",
        "GLFW",
        "ImGui"
    }

    link_lstd()

    postbuildcommands {
        "{COPY} \"" .. _WORKING_DIR .. "/src/data/\" \"%{cfg.targetdir}/data/\""
    }
