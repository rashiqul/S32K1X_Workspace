# S32K1X Workspace

Embedded firmware development workspace for NXP S32K1xx microcontrollers.
Supports both a host-side x86_64 build/test pipeline for fast local iteration and CI, and a cross-compiled ARM firmware build targeting the S32K144 (Cortex-M4).

## Author

**Mohammad Rashiqul Alam**

## License

MIT — see [LICENSE](LICENSE) for full terms.

---

## Target Platform

| Item | Detail |
|------|--------|
| MCU Family | NXP S32K1xx |
| Current Target | S32K144 (ARM Cortex-M4 @ up to 112 MHz) |
| Evaluation Board | S32K144EVB-Q100 |
| AUTOSAR RTD | 4.7.0 (RTD 3.0.0) |
| FreeRTOS | V11.1.0 (GCC/ARM_CM4F port, heap_1) |
| CI Execution Target | x86_64 Linux (build, unit test, coverage, SonarQube) |

---

## Prerequisites

### Build Environment
- **OS**: Linux (Ubuntu 20.04+ recommended) / WSL2
- **GCC/G++**: 11+ (x86_64 host builds)
- **arm-none-eabi-gcc**: 10.2+ (ARM target builds — NXP bundled at `~/NXP/gcc-10.2-arm32-eabi/bin` or system install)
- **CMake**: 3.23+
- **Ninja**: Build system
- **Python**: 3.11+ with Poetry
- **Conan**: 2.x
- **Git**: Version control

### ARM Target Dependencies
- **NXP S32DS 3.6.6** *(optional — CI uses bundled headers)*
  - Platform SDK headers are bundled under `bsw/platform_sdk/` — no S32DS install required for CI or headless builds
  - Set `S32DS_SDK_ROOT` to override with a local S32DS installation
- Required SDK paths (resolved automatically from `bsw/platform_sdk/`):
  - `RTD/BaseNXP_TS_T40D2M30I0R0/`
  - `RTD/Platform_TS_T40D2M30I0R0/`

### Flash and Debug Dependencies
- **usbipd-win** (Windows) — attach USB debug probe to WSL2 (`make usbipd-connect`)
- **arm-none-eabi-gdb** — ARM GNU Toolchain 15.2+ (GDB client)
- **PEMicro pegdbserver_console** — bundled in S32DS at `eclipse/plugins/com.pemicro.debug.gdbjtag.pne_.../lin/`; used as GDB server (`make gdb-server`)

### Development Tools (Recommended)
- **clang-format**: C/C++ formatting (bsw/ vendor code excluded)
- **shellcheck**: Shell script linting
- **S32 Design Studio 3.6.6**: BSP configuration generation and flash/debug reference

---

## Project Structure

```
S32K1X_Workspace/
├── bsw/                          # Basic Software (AUTOSAR BSW layer)
│   ├── startup/                  #   Startup assembly + system init (NXP S32DS)
│   ├── linker/                   #   Linker script for FLASH execution
│   ├── drivers/rtd/              #   NXP RTD AUTOSAR 4.7 MCAL drivers
│   │   ├── include/              #     Driver headers (Clock, Mcu, Port, DIO, CAN...)
│   │   └── src/                  #     Driver sources
│   ├── config/
│   │   ├── generated/            #   S32DS-generated peripheral configuration
│   │   │   ├── include/
│   │   │   └── src/
│   │   └── board/                #   Board-level pin configuration
│   ├── rtos/                     #   FreeRTOS V11.1.0 (GCC/ARM_CM4F port)
│   │   ├── FreeRTOS/Source/      #     Kernel + portable layer
│   │   └── config/               #     FreeRTOSConfig.h
│   └── platform_sdk/             #   Bundled NXP Platform SDK headers (CI-friendly)
│
├── src/                          # Application source
│   ├── main.c                    #   Firmware entry point (FreeRTOS tasks + CAN)
│   ├── cbd/                      #   Component-Based Design (x86 unit-testable)
│   │   ├── template/             #     Example CBD component
│   │   └── iohal/                #     I/O Hardware Abstraction Layer (IoHal)
│   └── mbd/                      #   Model-Based Design (generated code — placeholder)
│
├── include/                      # Public interface headers
│   ├── cbd/
│   │   ├── template/
│   │   └── iohal/                #   IoHal public API headers
│   ├── mbd/                      #   MBD public headers (placeholder)
│   └── rtwtypes.h                #   Simulink Coder type definitions
│
├── test/                         # Unit tests (Google Test, x86 only)
│   └── cbd/unit_tests/
│
├── cmake/
│   ├── toolchains/
│   │   └── arm-none-eabi.cmake   # ARM Cortex-M4 cross-compile toolchain
│   └── CodeCoverage.cmake
│
├── libs/                         # Local third-party header-only libraries
│   ├── gtest/                    #   Google Test
│   ├── spdlog/                   #   Logging
│   ├── fmt/                      #   String formatting
│   └── autosar-platform-types/   #   AUTOSAR standard types (x86 builds)
│
├── .conan/profiles/
│   ├── c_cpp_workspace_linux     #   x86_64 Linux host profile
│   ├── c_cpp_workspace_macos     #   x86_64 macOS host profile
│   └── s32k1x_arm_cortex_m4     #   ARM Cortex-M4 bare-metal profile
│
├── scripts/                      # Utility and setup scripts
│   ├── usbipd_connect.py         #   Auto-attach OpenSDA probe to WSL2
│   ├── setup-dev-environment.sh  #   Developer toolchain setup
│   ├── setup-git-hooks.sh        #   Git hook installation
│   ├── log_command.sh            #   CI command logging helper
│   └── version                  #   Semver string generator
│
├── .github/workflows/            # CI/CD pipeline
│   └── linux-gcc-pipeline.yml
│
├── build_x86_64/{Debug,Release}/ # x86 build artifacts (generated)
└── build_s32k1/build_armv7/     # ARM target build artifacts (generated)
```

---

## Quick Start

### Setup

```bash
make setup       # Validate and install developer dependencies
make configure   # Install Poetry + Conan deps, configure x86 build
```

### x86 Host — Build and Test

```bash
make build          # Build Debug profile
make build-release  # Build Release profile
make test           # Run all unit tests
make coverage       # Full coverage analysis (Debug + Release)
```

### ARM Target — S32K144 Firmware

```bash
# Cross-compile firmware (uses bundled SDK headers — no S32DS required)
make build_all_tgt

# Outputs in build_s32k1/build_armv7/Release/bin/:
#   s32k144_firmware.elf   ELF with debug symbols
#   s32k144_firmware.hex   Intel HEX for flashing
#   s32k144_firmware.bin   Raw binary
#   s32k144_firmware.map   Linker map

# Override SDK/toolchain paths if needed:
make build_all_tgt \
    S32DS_SDK_ROOT=/mnt/c/NXP/S32DS.3.6.6/S32DS/software/PlatformSDK_S32K1_S32M24 \
    NXP_GCC_PATH=/home/rashiqul/NXP/gcc-10.2-arm32-eabi/bin
```

---

## Build Targets

### Configuration
- `make pre-configure` — Install Poetry dependencies
- `make conan-install` — Install Conan dependencies and generate build files
- `make configure` — Complete full project configuration
- `make clean` — Remove x86 build artifacts
- `make clean-all` — Remove all artifacts (x86 + ARM)

### Build — x86 Host
- `make build` — Build Debug profile
- `make build-debug` — Build Debug profile explicitly
- `make build-release` — Build Release profile
- `make list-targets` — List available CMake targets

### Build — ARM Target (S32K144)
- `make build_all_tgt` — Cross-compile S32K144 firmware (ARM GCC, Release)
- `make clean_tgt` — Remove ARM target build artifacts

### Flash and Debug — S32K144EVB
- `make usbipd-connect` — Auto-detect and attach OpenSDA probe to WSL2
- `make flash_tgt` — Flash firmware via PEMicro (no GDB client needed)
- `make gdb-server` — Start PEMicro GDB server on `localhost:7224`
- `make attach_tgt` — Full sequence: usbipd → flash → start GDB server

```bash
# Debug session (after gdb-server is running)
arm-none-eabi-gdb \
  -ex "target remote localhost:7224" \
  -ex "monitor reset halt" \
  -ex "load" \
  -ex "continue" \
  build_s32k1/build_armv7/RelWithDebInfo/bin/s32k144_firmware.elf
```

### Test and Quality
- `make test` — Run all unit tests
- `make coverage` — Full coverage workflow (Debug + Release + sanitizer)
- `make lint` — Python/tooling lint checks
- `make clang-format` — Format C/C++ sources (bsw/ excluded)

### Package
- `make package` — Build ARM firmware + create x86 Conan package

### Help
- `make help` — Show all available targets

---

## Firmware Architecture

The S32K144 firmware (`src/main.c`) implements:

- **Three FreeRTOS periodic tasks** (drift-free via `vTaskDelayUntil`):
  - `Task_10ms` — priority 3 (highest), 10 ms period
  - `Task_20ms` — priority 2, 20 ms period
  - `Task_1000ms` — priority 1, 1000 ms period; transmits a CAN frame

- **CAN transmit** via AUTOSAR FlexCAN RTD driver (`Can_43_FLEXCAN_Write`)
  - CAN ID `0x123`, 8-byte payload, standard 11-bit frame
  - LED on PTD0 toggles on every TX-complete interrupt (`App_CanTxConfirmation`)

- **AUTOSAR 4.7 RTD MCAL driver init** sequence (Platform → Mcu → Port → CanIf → CAN):
  - `Platform_Init` — IRQ routing (must be first)
  - `Mcu_Init` / `Mcu_InitClock` — 48 MHz FIRC
  - `Port_Init` — PTD0 as GPIO output (LED)
  - `CanIf_Init` / `Can_43_FLEXCAN_Init` / `SetControllerMode`

### IoHal — I/O Hardware Abstraction Layer

`src/cbd/iohal/` provides an AUTOSAR-style software component that abstracts BSW details from the application:

```
Application (main.c)
    │
    ▼
IoHal (src/cbd/iohal/)
    ├── iohal.c       — EcuM-style init sequencer
    ├── iohal_can.c   — CAN abstraction (hides FlexCAN/CanIf)
    └── iohal_os.c    — RTOS abstraction (hides FreeRTOS API)
    │
    ▼
BSW (bsw/) — NXP RTD AUTOSAR 4.7 drivers + FreeRTOS V11.1.0
```

### Memory Map (S32K144)

| Region | Base | Size |
|--------|------|------|
| Vector table (FLASH) | `0x00000000` | 1 KB |
| Flash config | `0x00000400` | 16 B |
| Code / rodata | `0x00000410` | ~511 KB |
| SRAM (data + BSS) | `0x1FFF8100` | ~56 KB |
| Stack (Core 0) | `0x20006000` | 4 KB |

### Toolchain Configuration

| Setting | Value |
|---------|-------|
| Compiler | `arm-none-eabi-gcc 10.2` |
| CPU flags | `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16` |
| ABI | `-fno-short-enums` (32-bit enum, matches NXP RTD) |
| ASM | `-x assembler-with-cpp` (C preprocessor on `.s` files) |
| Linker | `-specs=nano.specs -specs=nosys.specs -nostartfiles` |

---

## Testing

Google Test + CTest integration (x86 host only).

- Console: `make test`
- JUnit XML: `build_x86_64/{Debug,Release}/gtest_junit_ut_cicd.xml`

## Code Coverage

Generated with gcovr (x86 host only).

- Debug HTML: `build_x86_64/Debug/coverage/coverage_report.html`
- Release HTML: `build_x86_64/Release/coverage/coverage_report.html`
- Cobertura XML: `build_x86_64/{Debug,Release}/coverage/cobertura.xml`
- SonarQube XML: `build_x86_64/{Debug,Release}/coverage/sonarqube.xml`

---

## CI/CD Pipeline

GitHub Actions (`.github/workflows/linux-gcc-pipeline.yml`):

1. Repository compliance checks (required files, linear git history, commit messages)
2. Environment validation (GCC, CMake, Conan, clang-format, shellcheck)
3. Format checks — clang-format on `src/`, `include/`, `test/`; shellcheck on `scripts/`
4. ARM cross-compilation build (S32K144 firmware — bundled SDK headers, no S32DS)
5. x86 host build with warnings-as-errors
6. Conan package creation and validation
7. Unit tests with JUnit XML artifacts
8. SonarQube analysis with gcovr coverage (requires `SONAR_TOKEN` secret)

---

## SonarQube Setup (Optional)

1. Sign up at [SonarCloud](https://sonarcloud.io/) and select org `rashiqul-sonar-workspace`
2. Import the repository — project key: `<github_owner>_S32K1X_Workspace`
3. Add `SONAR_TOKEN` secret: GitHub → Settings → Secrets → Actions
4. SonarScanner properties are passed directly from the workflow — no `sonar-project.properties` file needed

---

## Roadmap

- [x] S32K144 BSW structure (startup, linker, AUTOSAR RTD MCAL drivers)
- [x] ARM Cortex-M4 cross-compilation toolchain + CMake
- [x] `make build_all_tgt` firmware build workflow
- [x] Bundle NXP Platform SDK headers for CI (no S32DS install required)
- [x] ARM cross-compilation CI job in GitHub Actions
- [x] Firmware: MCU + clock init (48 MHz FIRC)
- [x] Firmware: Port/DIO init + LED on PTD0
- [x] Firmware: FreeRTOS V11.1.0 — three periodic tasks (10/20/1000 ms)
- [x] Firmware: CAN transmit via FlexCAN RTD + TX-confirm LED toggle
- [x] `make usbipd-connect` — auto-attach OpenSDA probe to WSL2
- [x] `make gdb-server` — PEMicro GDB server on port 7224
- [x] `make flash_tgt` / `make attach_tgt` — flash and debug targets
- [x] VS Code IntelliSense + SonarLint using ARM compile_commands.json
- [x] Restructure: bsw/ at workspace root (AUTOSAR BSW layer naming)
- [x] Restructure: src/cbd/ + src/mbd/ application layer split
- [x] IoHal software component scaffold (iohal_can, iohal_os, AUTOSAR-style)
- [ ] IoHal implementation — fill in BSW call stubs
- [ ] Refactor main.c to use IoHal API
- [ ] Unit tests for IoHal (stub/mock BSW for x86)
- [ ] Expand MCAL coverage (UART, SPI, ADC)
- [ ] MBD component integration under src/mbd/
