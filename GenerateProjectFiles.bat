@ECHO OFF
REM clean command is not yet implemented, so the call will just do nothing. 
REM We let this stay here though so it will run properly once the action is implemented.
echo "Creating vs2022 solution with msvc compiler..."
CALL "Bootstrap-Tools\premake5.exe" clean && CALL "Bootstrap-Tools\premake5.exe" vs2022
