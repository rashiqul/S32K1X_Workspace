#******************************************************************************
#
#     File Name: Makefile
#     Project: Body Repository Template - Local Development
#     Description: Makefile for building, testing, and analyzing the project
#                  (Migrated from justfile for local development)
#
#     Author: Mohammad Rashiqul Alam
#     Copyright (c) 2025. All rights reserved.
#
#******************************************************************************

PACKAGE_NAME ?= c_cpp_template_workspace
PACKAGE_VERSION ?=
PYTHON_FILES ?= .
MOD_TO_ANALYZE ?= ""
C_FILES ?=
CONAN_CONFIG_VERSION ?= main
CONAN_CONFIG_URL ?=
CONAN_FLAGS_PARAM := $(CONAN_FLAGS)

# Default for local x86_64 development
TARGET_CPU ?= x86_64

# ============================================================================
# ARM Target (S32K144) configuration
# ============================================================================
S32DS_SDK_ROOT    ?= /mnt/c/NXP/S32DS.3.6.6/S32DS/software/PlatformSDK_S32K1_S32M24
NXP_GCC_PATH      ?= /home/rashiqul/NXP/gcc-10.2-arm32-eabi/bin
ARM_BUILD_TYPE    ?= RelWithDebInfo
ARM_BUILD_DIR     := build_s32k1/build_armv7/$(ARM_BUILD_TYPE)

# Only pass S32DS_SDK_ROOT to cmake when the user explicitly sets it (command line
# or environment). When using the default, let bsp/CMakeLists.txt prefer the
# bundled headers in bsp/platform_sdk/ — required for CI (no S32DS installed).
ifneq ($(filter command line environment,$(origin S32DS_SDK_ROOT)),)
CMAKE_SDK_FLAG := -DS32DS_SDK_ROOT=$(S32DS_SDK_ROOT)
else
CMAKE_SDK_FLAG :=
endif

# Auto-detect OS and set appropriate Conan profile
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	DEFAULT_CONAN_BUILD_PROFILE := $(CURDIR)/.conan/profiles/c_cpp_workspace_linux
else ifeq ($(UNAME_S),Darwin)
	DEFAULT_CONAN_BUILD_PROFILE := $(CURDIR)/.conan/profiles/c_cpp_workspace_macos
else
	$(error Unsupported OS: $(UNAME_S))
endif

CONAN_BUILD_PROFILE ?= $(DEFAULT_CONAN_BUILD_PROFILE)

ifeq ($(MAKECMDGOALS),coverage)
COVERAGE ?= 1
endif

ifeq ($(COVERAGE), 1)
TARGET_CPU = x86_64
BUILD_TYPE ?= Debug
CONAN_FLAGS += -o ${PACKAGE_NAME}/*:coverage=True
else
BUILD_TYPE ?= Release
endif

BUILD_DIR ?= build_$(TARGET_CPU)/$(BUILD_TYPE)
CONAN_FLAGS += -pr:b $(CONAN_BUILD_PROFILE)
CONAN_FLAGS += -pr:h $(CONAN_BUILD_PROFILE)
CONAN_FLAGS += --lockfile-partial

ifeq ($(LOCAL_CONAN),1)
export CONAN_HOME=${CURDIR}/.conan2_local/.conan2
endif

ifneq ($(VERBOSE),1)
.SILENT:
endif

.PHONY: default help
default: help

help:
	@echo "Available targets:"
	@echo ""
	@echo "Configuration:"
	@echo "  pre-configure     - Install Poetry dependencies"
	@echo "  configure         - Complete project configuration"
	@echo "  conan-install     - Install Conan dependencies"
	@echo "  clean             - Remove build artifacts"
	@echo "  clean-all         - Remove all artifacts (pristine state)"
	@echo ""
	@echo "Build (x86 host):"
	@echo "  build             - Build the project (Debug)"
	@echo "  build-debug       - Build Debug profile"
	@echo "  build-release     - Build Release profile"
	@echo "  list-targets      - List all available build targets"
	@echo ""
	@echo "Build (ARM target - S32K144):"
	@echo "  build_all_tgt     - Build S32K144 firmware (ARM GCC, Release)"
	@echo "  clean_tgt         - Remove ARM target build artifacts"
	@echo "    Variables:"
	@echo "      S32DS_SDK_ROOT  Platform SDK path (default: $(S32DS_SDK_ROOT))"
	@echo "      NXP_GCC_PATH    ARM GCC bin path  (default: $(NXP_GCC_PATH))"
	@echo ""
	@echo "Test:"
	@echo "  test              - Run all tests"
	@echo "  coverage          - Run full coverage analysis"
	@echo "  coverage-cbd      - Generate C/C++ coverage report"
	@echo ""
	@echo "Quality:"
	@echo "  lint              - Run linting checks"
	@echo "  format            - Format code"
	@echo "  clang-format      - Format all C/C++ code with clang-format"
	@echo ""
	@echo "Package:"
	@echo "  package           - Build ARM firmware + create x86 Conan package (matches CI)"
	@echo ""

list-targets:
	@if [ -f ${BUILD_DIR}/Makefile ]; then \
		echo "Available build targets:"; \
		poetry run cmake --build ${BUILD_DIR} --target help; \
	else \
		echo "Build directory not found. Run 'make configure' first."; \
	fi

.PHONY: all
all: configure build-release test lint

.PHONY: lint check
lint check:
	poetry check -v
	poetry run black --check $(PYTHON_FILES) --exclude "build_x86_64|.conan2_local"
	poetry run ruff $(PYTHON_FILES) --exclude "build_x86_64,.conan2_local"
	poetry run vulture --min-confidence 80 --sort-by-size $(PYTHON_FILES) --exclude "build_x86_64,.conan2_local"

.PHONY: setup
setup:
	@echo "🔧 Running development environment setup..."
	@./scripts/setup-dev-environment.sh

.PHONY: pre-configure
pre-configure:
	poetry install --no-root

.PHONY: conan-install
conan-install:
	poetry run conan install . --build=missing -s build_type=$(BUILD_TYPE) $(CONAN_FLAGS)
	CONAN_BUILD=0 CONAN_RUN_TESTS=0 poetry run conan build . --build=missing -s build_type=$(BUILD_TYPE) $(CONAN_FLAGS)

.PHONY: configure
configure: conanfile.py pre-configure conan-install

.PHONY: build-debug
build-debug:
	BUILD_TYPE=Debug $(MAKE) configure
	. build_$(TARGET_CPU)/Debug/generators/conanbuild.sh && poetry run cmake --build build_$(TARGET_CPU)/Debug -t all

.PHONY: build-release
build-release:
	BUILD_TYPE=Release $(MAKE) configure
	. build_$(TARGET_CPU)/Release/generators/conanbuild.sh && poetry run cmake --build build_$(TARGET_CPU)/Release -t all

.PHONY: build
build: build-debug

.PHONY: test
test: configure
	. ${BUILD_DIR}/generators/conanbuild.sh && poetry run cmake --build ${BUILD_DIR} -t test

.PHONY: package
package: build_all_tgt
	@echo "========================================================"
	@echo "  Creating x86_64 Conan package"
	@echo "  Profile : $(CONAN_BUILD_PROFILE)"
	@echo "  Build   : $(BUILD_TYPE)"
	@echo "========================================================"
	poetry run conan create . --build=missing \
		-s build_type=$(BUILD_TYPE) \
		-pr:h $(CONAN_BUILD_PROFILE) \
		-pr:b $(CONAN_BUILD_PROFILE)
	@echo "========================================================"
	@echo "  Package complete"
	@echo "  x86 : Conan cache"
	@echo "  ARM : $(ARM_BUILD_DIR)/bin/"
	@echo "========================================================"

.PHONY: coverage
coverage: clean pre-configure
	@echo "========================================================"
	@echo "          Full Coverage Analysis"
	@echo "========================================================"
	@echo ""
	@echo "Building and testing Debug profile with coverage..."
	@echo "--------------------------------------------------------"
	COVERAGE=1 $(MAKE) build-debug
	COVERAGE=1 BUILD_TYPE=Debug $(MAKE) test
	COVERAGE=1 BUILD_TYPE=Debug $(MAKE) coverage-nosanitize coverage-cbd
	@echo ""
	@echo "Building and testing Release profile with coverage..."
	@echo "--------------------------------------------------------"
	COVERAGE=1 $(MAKE) build-release
	COVERAGE=1 BUILD_TYPE=Release $(MAKE) test
	COVERAGE=1 BUILD_TYPE=Release $(MAKE) coverage-nosanitize coverage-cbd
	@echo ""
	@echo "Building and testing Debug profile with sanitizer..."
	@echo "--------------------------------------------------------"
	COVERAGE=1 BUILD_TYPE=Debug $(MAKE) coverage-sanitize
	COVERAGE=1 BUILD_TYPE=Debug $(MAKE) test
	COVERAGE=1 BUILD_TYPE=Debug $(MAKE) coverage-cbd
	@echo ""
	@echo "========================================================"
	@echo "          Coverage Reports Generated"
	@echo "========================================================"
	@echo "Debug HTML Report:   build_$(TARGET_CPU)/Debug/coverage/coverage_report.html"
	@echo "Release HTML Report: build_$(TARGET_CPU)/Release/coverage/coverage_report.html"
	@echo "========================================================"

.PHONY: coverage-nosanitize
coverage-nosanitize:
	find . -name "*.gcda" | xargs rm -f
	CONAN_RUN_ENABLE_SANITIZER=0 \
	poetry run conan build . --build=missing -s build_type=$(BUILD_TYPE) $(CONAN_FLAGS)

.PHONY: coverage-sanitize
coverage-sanitize:
	find . -name "*.gcda" | xargs rm -f
	CONAN_RUN_ENABLE_SANITIZER=1 \
	poetry run conan build . --build=missing -s build_type=$(BUILD_TYPE) $(CONAN_FLAGS)

.PHONY: coverage-cbd
coverage-cbd:
	mkdir -p ${BUILD_DIR}/coverage
	@echo ""
	@echo "C/C++ Code Coverage ($(BUILD_TYPE))"
	@echo "----------------------------------------"
	@(poetry run gcovr -r. -s \
	--exclude 'test/.*' \
	--exclude '.conan2_local/.*' \
	--html-details --html-title "C_Cpp Template Code Coverage Report ($(BUILD_TYPE))" \
	--cobertura ${BUILD_DIR}/coverage/cobertura.xml --cobertura-pretty \
	--sonarqube ${BUILD_DIR}/coverage/sonarqube.xml \
	-o ${BUILD_DIR}/coverage/coverage_report.html 2>/dev/null | grep -E '^(lines|functions|branches):' || echo "No C/C++ coverage data found")

.PHONY: clang-format
clang-format:
	@echo "Formatting C/C++ files with clang-format..."
	@find src include test -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
		! -path "src/bsp/*" \
		2>/dev/null | while read file; do \
		echo "  Formatting: $$file"; \
		clang-format -i "$$file"; \
	done
	@echo "✅ All C/C++ files formatted"

.PHONY: format
format:
	poetry run black $(PYTHON_FILES)
	poetry run ruff --fix $(PYTHON_FILES)

# ============================================================================
# ARM Target Build
# ============================================================================
.PHONY: build_all_tgt
build_all_tgt: pre-configure
	@echo "========================================================"
	@echo "  Building S32K144 ARM firmware"
	@echo "  Toolchain : $(NXP_GCC_PATH)/arm-none-eabi-gcc"
	@echo "  SDK Root  : $(S32DS_SDK_ROOT)"
	@echo "  Build Dir : $(ARM_BUILD_DIR)"
	@echo "========================================================"
	export PATH=$(NXP_GCC_PATH):$$PATH && \
	poetry run cmake -S . -B $(ARM_BUILD_DIR) \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/cmake/toolchains/arm-none-eabi.cmake \
		-DCMAKE_BUILD_TYPE=$(ARM_BUILD_TYPE) \
		-GNinja \
		$(CMAKE_SDK_FLAG) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
	ninja -C $(ARM_BUILD_DIR) -j$$(nproc)
	@echo "========================================================"
	@echo "  ARM build complete — artifacts in $(ARM_BUILD_DIR)/bin/"
	@echo "  ELF : $(ARM_BUILD_DIR)/bin/s32k144_firmware.elf"
	@echo "  HEX : $(ARM_BUILD_DIR)/bin/s32k144_firmware.hex"
	@echo "  BIN : $(ARM_BUILD_DIR)/bin/s32k144_firmware.bin"
	@echo "  MAP : $(ARM_BUILD_DIR)/bin/s32k144_firmware.map"
	@echo "========================================================"

# ============================================================================
# USB/IP — attach OpenSDA probe to WSL2
# Calls usbipd.exe via WSL interop; board must be plugged into Windows USB.
# ============================================================================
.PHONY: usbipd-connect
usbipd-connect:
	python3 scripts/usbipd_connect.py

# ============================================================================
# GDB server — PEMicro pegdbserver_console (native Linux ELF from S32DS)
# The S32K144EVB-Q100 OpenSDA uses PEMicro's proprietary USB protocol.
# pegdbserver_console is a statically-linked Linux binary that runs in WSL2
# and exposes a standard GDB remote protocol on port 7224.
#
# Leave this terminal open, then connect arm-none-eabi-gdb in another:
#   target remote localhost:7224
#   GDB server : localhost:7224
# ============================================================================
PEMICRO_SERVER ?= /mnt/c/NXP/S32DS.3.6.6/eclipse/plugins/com.pemicro.debug.gdbjtag.pne_6.0.8.202509081843/lin/pegdbserver_console
PEMICRO_DEVICE ?= NXP_S32K1xx_S32K144F512M15
GDB_PORT       ?= 7224

.PHONY: gdb-server
gdb-server:
	$(PEMICRO_SERVER) \
		-startserver \
		-device=$(PEMICRO_DEVICE) \
		-serverport=$(GDB_PORT) \
		-singlesession \
		-verbose

# ============================================================================
# Flash — standalone erase + program + verify, no GDB client required.
# pegdbserver_console connects to the probe, programs the ELF, then exits.
# Run `make build_all_tgt` first to ensure the ELF is up to date.
# ============================================================================
FIRMWARE_ELF ?= $(ARM_BUILD_DIR)/bin/s32k144_firmware.elf

.PHONY: flash_tgt
flash_tgt:
	@echo "========================================================"
	@echo "  Flashing S32K144 firmware"
	@echo "  ELF : $(FIRMWARE_ELF)"
	@echo "========================================================"
	$(PEMICRO_SERVER) \
		-startserver \
		-device=$(PEMICRO_DEVICE) \
		-serverport=7225 \
		-flashobjectfile=$(FIRMWARE_ELF) \
		-programmingtype=0 \
		-quitafterprogramming \
		-singlesession \
		-verbose
	@echo "Flash complete."

# ============================================================================
# Attach — full debug session setup:
#   1. Attach OpenSDA probe to WSL2
#   2. Flash the latest firmware
#   3. Start GDB server in background on :7224
#   4. Prompt user to press F5 in VS Code
#
# VS Code launch.json connects to the running server on localhost:7224,
# loads symbols from the ELF, and enters the debug perspective.
# ============================================================================
.PHONY: attach_tgt
attach_tgt:
	@echo "========================================================"
	@echo "  Step 1/3 — Attaching OpenSDA probe to WSL2"
	@echo "========================================================"
	python3 scripts/usbipd_connect.py
	@echo ""
	@echo "========================================================"
	@echo "  Step 2/3 — Flashing firmware"
	@echo "========================================================"
	$(MAKE) flash_tgt
	@echo ""
	@echo "========================================================"
	@echo "  Step 3/3 — Starting GDB server on :$(GDB_PORT)"
	@echo "========================================================"
	$(PEMICRO_SERVER) \
		-startserver \
		-device=$(PEMICRO_DEVICE) \
		-serverport=$(GDB_PORT) \
		-singlesession \
		-verbose &
	@sleep 2
	@echo ""
	@echo "========================================================"
	@echo "  GDB server running on localhost:$(GDB_PORT)"
	@echo "  Press F5 in VS Code to start the debug session."
	@echo "  (launch.json must be configured — see .vscode/)"
	@echo "========================================================"

.PHONY: clean_tgt
clean_tgt:
	rm -rf build_s32k1
	@echo "ARM target build artifacts removed"

.PHONY: clean clean-all
clean:
	find . -maxdepth 1 -type d -name "build*" | xargs rm -rf
	rm -rf .benchmark .coverage .venv .*_cache site
	find . -type d -name __pycache__ | xargs rm -rf

clean-all: clean clean_tgt
	@echo "Removing all dependencies and virtual environments..."
	rm -rf .conan2_local poetry.lock
	@echo "Project reset to pristine state"

.DEFAULT:
	. ${BUILD_DIR}/generators/conanbuild.sh && poetry run cmake --build ${BUILD_DIR} --target $@
