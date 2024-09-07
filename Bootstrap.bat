@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

ECHO Bootstrapping Prism...

ECHO Checking for suitable python installation...
call :check_python_version
IF !ERRORLEVEL! NEQ 0 (
	ECHO No suitable python installation found, please install python 3.9 with pip or later. Alternatively install Conan C++ package manager standalone
	GOTO :ERROR
)

ECHO Checking for suitable conan installation...
call :check_conan_version
IF !ERRORLEVEL! NEQ 0 (
    ECHO No suitable conan installation found, checking for python pip install conan via pip...
	where /q pip
	IF !ERRORLEVEL! NEQ 0 (
		ECHO No pip found in your python installation found, please install pip for your python installation
		GOTO :ERROR
	) ELSE (
		ECHO Python installation has a pip, trying to install conan with it...
		pip install "conan>=2"
		IF !ERRORLEVEL! NEQ 0 (
			GOTO :ERROR
		)
	)
) ELSE ( 
    ECHO Found a conan installation, proceeding with bootstrap...
)

IF NOT EXIST "./Bootstrap-Tools" (
    MKDIR "./Bootstrap-Tools"
)

ECHO Pulling premake...
cd "./Bootstrap-Tools"
curl -L -o "premake.zip" https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip
tar -xf "premake.zip"
del "premake.zip"
cd ..

IF "%~1"=="-conanprofile" (
    IF NOT "%~2"=="" (
	    set CONAN_PROFILE=%2
	) ELSE (
		ECHO No Conan profile specified, autodetecting...
		set CONAN_PROFILE=
	)
)


CALL "InstallDependencies.bat" %CONAN_PROFILE%
CALL "GenerateProjectFiles.bat"
ECHO Modifying Sandbox/Sandbox.vcxproj...
python "./AddVSUpToDateCheckProperty.py"
PAUSE
GOTO:EOF

:ERROR
PAUSE
GOTO:EOF

:check_python_version
:: Check if Python is available
where python >nul 2>nul
IF errorlevel 1 (
    echo Python is not available.
    exit /b 1
)

:: Check Python version
for /f "tokens=2 delims= " %%i in ('python --version 2^>^&1') do set "ver=%%i"
for /f "tokens=1,2 delims=." %%i in ("%ver%") do (
    set "major=%%i"
    set "minor=%%j"
)

IF %major% LSS 3 (
    exit /b 1
)

IF %major% EQU 3 IF %minor% LSS 9 (
    exit /b 1
)

echo Found suitable python version %ver%.
exit /b 0

:check_conan_version
:: Check if Conan is available
where conan >nul 2>nul
IF errorlevel 1 (
    echo Conan is not available.
    exit /b 1
)

:: Check Conan version
for /f "tokens=3 delims= " %%i in ('conan --version') do set "ver=%%i"
for /f "tokens=1,2 delims=." %%i in ("%ver%") do (
    set "major=%%i"
    set "minor=%%j"
)

IF %major% LSS 2 (
    exit /b 1
)

echo Found suitable conan version %ver%.
exit /b 0
