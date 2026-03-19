import os
from pathlib import Path
from subprocess import check_output

from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, rmdir
from conan.errors import ConanException
from conan.errors import ConanException


class CCppTemplateWorkspaceConan(ConanFile):
    name = "c_cpp_template_workspace"
    description = "C/C++ Template Workspace"
    license = "MIT"
    url = "https://github.com/example/c_cpp_template_workspace"
    homepage = "https://github.com/example/c_cpp_template_workspace"
    topics = ("c", "cpp", "template")

    settings = "os", "compiler", "build_type", "arch"
    package_type = "library"
    no_copy_source = True
    generators = "CMakeDeps", "CMakeToolchain"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "coverage": [True, False],
        "header_only": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "coverage": False,
        "header_only": True,
    }
    implements = (
        "auto_shared_fpic",
        "auto_header_only",
    )

    exports_sources = (
        "include/*",
        "libs/*",
        "CMakeLists.txt",
        "LICENSE",
        "README.md",
        "cmake/*",
        "scripts/*",
        "docs/*",
        "src/*",
        "test/*",
        "!.conan2*/",
        "!.git/*",
    )

    def requirements(self):
        pass

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    @property
    def _root_folder(self):
        return Path(self.recipe_folder)

    @property
    def _scripts_folder(self):
        return self._root_folder / "scripts"

    @property
    def _should_test(self):
        # See https://github.com/conan-io/conan/issues/11969
        should_test = os.environ.get("CONAN_RUN_TESTS", "1").lower()
        return should_test in ("yes", "true", "t", "1")

    @property
    def _should_build(self):
        should_build = os.environ.get("CONAN_BUILD", "1").lower()
        return should_build in ("yes", "true", "t", "1")

    @property
    def _custom_target(self):
        return os.environ.get("CONAN_CUSTOM_TARGET", None)

    @property
    def _custom_target(self):
        return os.environ.get("CONAN_CUSTOM_TARGET", None)

    @property
    def _install_path(self):
        return Path(self.build_folder) / self.folders.package

    def set_version(self):
        if self.version:
            return
        try:
            version_script = self._scripts_folder / "version"
            self.version = check_output(version_script, text=True).strip()
        except Exception as e:
            # Fallback to default version if script fails (e.g., in CI without full git history)
            self.output.warning(f"Failed to get version from script: {e}")
            self.version = "0.1.0"

    def _get_arch(self):
        # Only x86_64 architecture supported for local development
        return self.settings.arch

    def layout(self):
        self.folders.root = self._root_folder
        self.folders.package = "package"
        cmake_layout(self, build_folder=f"build_{self._get_arch()}")

    def generate(self):
        # Local development - no external dependencies required
        pass

    def build(self):
        cmake = CMake(self)
        cmake.configure(
            variables={
                "CMAKE_INSTALL_PREFIX": self._install_path,
                "COVERAGE": "ON" if self.options.coverage else "OFF",
                "CONAN_PACKAGE_VERSION": str(self.version),
            },
        )

        if self._custom_target != None:
            cmake.build(target=self._custom_target)
        elif self._should_build:
            cmake.build()

        if self._should_test:
            # Then run the tests with output on failure
            try:
                cmake.ctest(cli_args=["--output-on-failure", "--progress", "--output-junit", "gtest_junit_ut_cicd.xml"])
            except ConanException as e:
                # In here we catch the ConanException which is raised when the tests fail and we can log the error
                # message, but we still want to continue the execution to report the gcovr reports
                self.output.error(f"Tests failed with exit code {e}")

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", "CCppTemplateWorkspace")

        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = []
        self.cpp_info.srcdirs = ["src"]
