from conan import ConanFile
from conan.tools import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMakeDeps

class CppApRecipe(ConanFile):
    name = "cpp-ap"
    version = "2.5.1"
    license = "MIT"
    author = "SpectraL519"
    url = "https://github.com/SpectraL519/cpp-ap"
    description = "Command-line argument parser for C++20"
    topics = ("cpp", "argument-parser", "header-only")
    settings = "os", "compiler", "build_type", "arch"
    no_copy_source = True
    exports_sources = "include/*", "CMakeLists.txt", "cmake/*"

    def generate(self):
        CMakeToolchain(self).generate()
        CMakeDeps(self).generate()

    def package(self):
        self.copy("LICENSE.md", dst="licenses")
        self.copy("*.hpp", dst="include", src="include")
        self.copy("cpp-ap-config.cmake", dst="lib/cmake/cpp-ap", keep_path=False)
        self.copy("cpp-ap-config-version.cmake", dst="lib/cmake/cpp-ap", keep_path=False)
        self.copy("cpp-ap-targets.cmake", dst="lib/cmake/cpp-ap", keep_path=False)

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "cpp-ap")
        self.cpp_info.set_property("cmake_target_name", "cpp-ap::cpp-ap")
        self.cpp_info.includedirs = ["include"]

    def validate(self):
        check_min_cppstd(self, "20")
