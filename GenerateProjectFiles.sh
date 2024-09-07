#!/bin/bash

# clean command is not yet implemented, so the call will just do nothing. 
# We let this stay here though so it will run properly once the action is implemented.
echo "Creating gmake2 project files with clang compiler..."
./Bootstrap-Tools/premake5 clean && ./Bootstrap-Tools/premake5 --cc=clang --verbose gmake2
