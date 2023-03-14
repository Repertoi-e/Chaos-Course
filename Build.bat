REM @Platform

@echo off

call msbuild "Build\vs2022\Chaos.sln" -nologo -verbosity:m
