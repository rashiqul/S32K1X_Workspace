#!/bin/bash
# Development environment setup script for c_cpp_template_workspace
# This script checks for required tools and installs them if missing

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "🔧 Setting up development environment for c_cpp_template_workspace..."
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check apt package
check_apt_package() {
    dpkg -l "$1" 2>/dev/null | grep -q "^ii"
}

MISSING_PACKAGES=()

# Check GCC
if command_exists gcc && command_exists g++; then
    GCC_VERSION=$(gcc --version | head -n1 | awk '{print $NF}')
    echo -e "${GREEN}✓${NC} GCC compiler found (version $GCC_VERSION)"
else
    echo -e "${RED}✗${NC} GCC compiler not found"
    MISSING_PACKAGES+=("gcc" "g++")
fi

# Check CMake
if command_exists cmake; then
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    echo -e "${GREEN}✓${NC} CMake found (version $CMAKE_VERSION)"
else
    echo -e "${RED}✗${NC} CMake not found"
    MISSING_PACKAGES+=("cmake")
fi

# Check Ninja
if command_exists ninja; then
    NINJA_VERSION=$(ninja --version)
    echo -e "${GREEN}✓${NC} Ninja build system found (version $NINJA_VERSION)"
else
    echo -e "${RED}✗${NC} Ninja build system not found"
    MISSING_PACKAGES+=("ninja-build")
fi

# Check Python3
if command_exists python3; then
    PYTHON_VERSION=$(python3 --version | awk '{print $2}')
    echo -e "${GREEN}✓${NC} Python3 found (version $PYTHON_VERSION)"
else
    echo -e "${RED}✗${NC} Python3 not found"
    MISSING_PACKAGES+=("python3" "python3-pip")
fi

# Check pip
if command_exists pip3; then
    echo -e "${GREEN}✓${NC} pip3 found"
else
    echo -e "${RED}✗${NC} pip3 not found"
    # Only add python3-pip if not already added
    if [[ ! " ${MISSING_PACKAGES[*]} " =~ " python3-pip " ]]; then
        MISSING_PACKAGES+=("python3-pip")
    fi
fi

# Check clang-format
if command_exists clang-format; then
    CLANG_FORMAT_VERSION=$(clang-format --version | head -n1 | awk '{print $3}')
    echo -e "${GREEN}✓${NC} clang-format found (version $CLANG_FORMAT_VERSION)"
else
    echo -e "${RED}✗${NC} clang-format not found"
    MISSING_PACKAGES+=("clang-format")
fi

# Check shellcheck
if command_exists shellcheck; then
    SHELLCHECK_VERSION=$(shellcheck --version | head -n2 | tail -n1 | awk '{print $2}')
    echo -e "${GREEN}✓${NC} shellcheck found (version $SHELLCHECK_VERSION)"
else
    echo -e "${RED}✗${NC} shellcheck not found"
    MISSING_PACKAGES+=("shellcheck")
fi

# Check Git
if command_exists git; then
    GIT_VERSION=$(git --version | awk '{print $3}')
    echo -e "${GREEN}✓${NC} Git found (version $GIT_VERSION)"
else
    echo -e "${RED}✗${NC} Git not found"
    MISSING_PACKAGES+=("git")
fi

echo ""

# Install missing packages if any
if [ ${#MISSING_PACKAGES[@]} -gt 0 ]; then
    echo -e "${YELLOW}📦 Missing packages detected:${NC} ${MISSING_PACKAGES[*]}"
    echo ""
    
    read -p "Do you want to install missing packages? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Installing packages..."
        sudo apt-get update
        sudo apt-get install -y "${MISSING_PACKAGES[@]}"
        echo -e "${GREEN}✓${NC} Packages installed successfully"
    else
        echo -e "${YELLOW}⚠${NC}  Skipping package installation. Please install manually:"
        echo "    sudo apt-get install ${MISSING_PACKAGES[*]}"
    fi
    echo ""
fi

# Check Conan
if command_exists conan; then
    CONAN_VERSION=$(conan --version | awk '{print $3}')
    echo -e "${GREEN}✓${NC} Conan found (version $CONAN_VERSION)"
    
    # Check if version is >= 2.0
    MAJOR_VERSION=$(echo "$CONAN_VERSION" | cut -d. -f1)
    if [ "$MAJOR_VERSION" -lt 2 ]; then
        echo -e "${YELLOW}⚠${NC}  Conan version is < 2.0, upgrading..."
        pip3 install --upgrade --user "conan>=2.0"
    fi
else
    echo -e "${RED}✗${NC} Conan not found"
    echo ""
    read -p "Do you want to install Conan? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Installing Conan..."
        pip3 install --user "conan>=2.0"
        echo -e "${GREEN}✓${NC} Conan installed successfully"
        echo ""
        echo -e "${YELLOW}⚠${NC}  You may need to add ~/.local/bin to your PATH"
        echo "    Add this to your ~/.bashrc:"
        echo "    export PATH=\"\$HOME/.local/bin:\$PATH\""
    else
        echo -e "${YELLOW}⚠${NC}  Skipping Conan installation. Please install manually:"
        echo "    pip3 install --user 'conan>=2.0'"
    fi
fi

echo ""
echo "🎉 Development environment check complete!"
echo ""
echo "Next steps:"
echo "  1. Run: make build"
echo "  2. Run: make test"
echo ""
echo "Note: The project uses a custom Conan profile (.conan/profiles/c_cpp_workspace)"
echo "      This ensures builds work consistently across WSL, Linux, and Mac."
echo ""
