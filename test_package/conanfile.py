import os
from pathlib import Path
from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy


class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def generate(self):
        for req, dep in self.dependencies.items():
            dep_build_folder = Path(self.build_folder) / "upstream" / req.ref.name
            if dep.package_folder is not None:
                copy(self, "*", dep.package_folder, dep_build_folder)

    def build(self):
        if self.settings.arch == 'x86_64':
            cmake = CMake(self)
            cmake.configure()
            cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "test_package")
            self.run(cmd, env="conanrun")
