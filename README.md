# S32K1X Workspace

Embedded firmware development workspace for NXP S32K1xx microcontrollers, with a host-side x86_64 build and test pipeline for fast local iteration and CI validation.

## Target Platform

- **Microcontroller Family**: NXP S32K1xx
- **Project Scope**: Embedded application and platform software bring-up
- **Current CI Execution Target**: x86_64 Linux (build, unit test, coverage, SonarQube)

## Prerequisites

### Build Environment
- **OS**: Linux (Ubuntu 20.04+ recommended)
- **GCC/G++**: 11+
- **CMake**: 3.23+
- **Ninja**: Build system
- **Python**: 3.11+ with Poetry
- **Conan**: 2.x
- **Git**: Version control

### Development Tools (Recommended)
- **clang-format**: C/C++ formatting
- **shellcheck**: Shell script linting
- **S32 Design Studio**: BSP generation, flash, and debug workflows

## Project Structure

```
S32K1X_Workspace/
├── src/cbd/template/            # Core source code
├── test/cbd/unit_tests/         # Unit tests (Google Test)
├── include/                     # Public headers
├── libs/                        # Local third-party dependencies
├── .conan/profiles/             # Conan host/build profiles
├── .github/workflows/           # CI pipelines
├── scripts/                     # Utility and setup scripts
└── build_x86_64/{Debug,Release} # Generated build artifacts
```

## Quick Start

### Setup

1. Install and validate developer dependencies:

```bash
make setup
```

2. Install project dependencies and configure build files:

```bash
make configure
```

### Build and Test

```bash
# Build Debug profile
make build

# Build Release profile
make build-release

# Run tests
make test

# Generate coverage (Debug + Release + Debug with sanitizer)
make coverage
```

## Build Targets

### Configuration
- `make pre-configure` - Install Poetry dependencies
- `make conan-install` - Install Conan dependencies and generate build files
- `make configure` - Complete full project configuration
- `make clean` - Remove build artifacts
- `make clean-all` - Remove all generated artifacts and local caches

### Build
- `make build` - Build Debug profile
- `make build-debug` - Build Debug profile explicitly
- `make build-release` - Build Release profile
- `make list-targets` - List available CMake targets

### Test and Quality
- `make test` - Run all tests
- `make coverage` - Run full coverage workflow
- `make lint` - Run Python/tooling lint checks
- `make clang-format` - Format all C/C++ sources

### Package
- `make package` - Create Conan package

### Help
- `make help` - Show all available targets

## Testing

The project uses Google Test and CTest integration.

- Console output: `make test`
- JUnit XML output: `build_x86_64/{Debug,Release}/gtest_junit_ut_cicd.xml`

`make test` uses the current `BUILD_TYPE` value.

## Code Coverage

Coverage reports are generated with gcovr.

- Debug HTML: `build_x86_64/Debug/coverage/coverage_report.html`
- Release HTML: `build_x86_64/Release/coverage/coverage_report.html`
- Cobertura XML: `build_x86_64/{Debug,Release}/coverage/cobertura.xml`
- SonarQube XML: `build_x86_64/{Debug,Release}/coverage/sonarqube.xml`

## Local Libraries

The workspace includes the following local libraries in `libs/`:
- **gtest**
- **spdlog**
- **fmt**
- **autosar-platform-types**

## CI/CD Pipeline

The GitHub Actions pipeline in `.github/workflows/linux-gcc-pipeline.yml` runs:

1. Repository compliance checks
2. Environment validation
3. Format and shell lint checks
4. Build with warnings-as-errors
5. Conan package creation and validation
6. Unit tests with test result artifacts
7. SonarQube analysis (optional, enabled when `SONAR_TOKEN` is configured)

## SonarQube Setup (Optional)

To enable SonarQube analysis:

1. Sign up at [SonarCloud](https://sonarcloud.io/) and create or select organization `rashiqul-sonar-workspace`
2. Create/import the project for this repository
   - Project key format used by CI: `<github_owner>_S32K1X_Workspace`
3. Add the repository secret in GitHub:
   - Settings -> Secrets and variables -> Actions -> `SONAR_TOKEN`
4. Ensure the GitHub repository is bound in SonarCloud to enable PR decoration

Note: SonarScanner properties are passed directly from the GitHub workflow; there is no `sonar-project.properties` file in this repository.

## Development Workflow

### Linux (Build, Test, Static Analysis)
1. Make code changes in `src/`, `include/`, and `test/`
2. Run `make build` and `make test`
3. Run `make coverage` and review generated reports

### S32 Design Studio (BSP and Target Bring-up)
1. Generate BSP/startup/linker assets for your selected S32K1xx MCU in S32 Design Studio
2. Integrate generated files into this workspace in dedicated BSP folders
3. Keep host-side tests in this repository to preserve CI feedback loops

## Known Gaps and Roadmap

- [ ] Add S32K1xx BSP structure (startup, linker scripts, device headers)
- [ ] Add target-specific cross-compilation profile(s)
- [ ] Add embedded target build workflow alongside host build
- [ ] Expand module-level unit test coverage
- [ ] Rename remaining template-era Conan profile/package naming where needed

## License

See [LICENSE](LICENSE) for details.

## Author

**Mohammad Rashiqul Alam**

## Contributing

This is a project workspace under active development. For issues or suggestions, contact the repository owner.
