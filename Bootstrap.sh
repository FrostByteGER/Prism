#!/bin/bash

function check_python_version {
    # Check if Python is available
    if ! command -v python &> /dev/null
    then
        echo "Python is not installed."
        return 1
    fi

    # Check Python version
    ver=$(python -V 2>&1 | grep -Po '(?<=Python )(.+)')
    major=$(echo "$ver" | cut -d. -f1)
    minor=$(echo "$ver" | cut -d. -f2)

    if [ "$major" -lt 3 ]; then
        return 1
    fi

    if [ "$major" -eq 3 ] && [ "$minor" -lt 9 ]; then
        return 1
    fi

    echo "Found Python version is $ver."
    return 0
}

function check_conan_version {
    # Check if Conan is available
    if ! command -v conan &> /dev/null
    then
        echo "Conan is not installed."
        return 1
    fi

    # Check Conan version
    ver=$(conan --version | cut -d' ' -f3)
    major=$(echo $ver | cut -d'.' -f1)
    minor=$(echo $ver | cut -d'.' -f2)

    if (( major <= 1)); then
        return 1
    fi

    echo "Found suitable conan version $ver."
    return 0
}

echo Bootstrapping Prism...
if ! [ -x "$(command -v clang)" ]; then
  echo 'Error: clang is not installed.' >&2
  exit 1
fi

echo "Checking for suitable python installation..."
check_python_version
if [ $? -eq 1 ]; then
  echo 'No python installation found, please install python 3.9 with pip or later. Alternatively install Conan C++ package manager standalone'
  exit 1
fi

echo "Checking for suitable conan installation..."
check_conan_version
if [ $? -eq 1 ]; then
  echo 'No conan installation found, checking for python installation to install conan via pip...'
  if ! [ -x "$(command -v pip)" ]; then
    echo 'No pip found in your python installation found, please install pip for your python installation'
	  exit 1
  fi
  echo 'Python installation has a pip, trying to install conan with it...'
  # this will fail on python2
  if ! pip install "conan>=2"; then
    echo "Failed to install conan via pip. Please manually install it!"
    exit 1
  fi
else
  echo 'Found a suitable conan installation, proceeding with bootstrap...'
fi

BOOTSTRAP_DIR="./Bootstrap-Tools"
if ! [ -d "$BOOTSTRAP_DIR" ]; then
  mkdir $BOOTSTRAP_DIR
fi

mkdir -p "./Distribution/Debug"
mkdir -p "./Distribution/Release"

if ! [ -x "$(command -v curl)" ]; then
  echo 'No curl found, please install curl'
fi
echo 'Pulling premake'
cd $BOOTSTRAP_DIR
curl -L -o "premake.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-linux.tar.gz"
tar -xf "premake.tar.gz"
rm "premake.tar.gz"
cd ..

source "./InstallDependencies.sh"
source "./GenerateProjectFiles.sh"
