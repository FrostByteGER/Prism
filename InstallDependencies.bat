@ECHO OFF

del /s /q .\Distribution\*.dll

IF NOT "%~1"=="" (
    set CONAN_PROFILE=--profile:build=%1
) ELSE (
    conan profile detect
)

conan install . --build missing %CONAN_PROFILE% --settings=build_type=Debug
conan install . --build missing %CONAN_PROFILE% --settings=build_type=Release