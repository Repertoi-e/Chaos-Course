project "glad"
    kind "StaticLib"
    language "C"

    setup_configurations()

    files { "src/glad.c" }
    includedirs { "include" }