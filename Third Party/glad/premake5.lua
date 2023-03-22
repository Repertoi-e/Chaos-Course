project "glad"
    kind "StaticLib"
    language "C"

    common_settings()

    files { "src/glad.c" }
    includedirs { "include" }