from conan import ConanFile
from conan.tools.files import copy
import os
import shutil


class Prism(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"
    requires = "spdlog/1.14.1", "shaderc/2023.6", "vulkan-headers/1.3.290.0", "glm/cci.20230113", "glfw/3.4", "stb/cci.20240531", "cxxopts/3.2.0"

    def configure(self):
        self.options["fmt"].header_only = True
        self.options["spdlog"].header_only = True
        self.options["glfw"].shared = True

    def generate(self):
        for dep in self.dependencies.values():
            if dep.cpp_info.bindirs:
                self.copy_with_symlink_check("glfw3.dll", dep.cpp_info.bindirs[0], "./Distribution/Debug")
                self.copy_with_symlink_check("glfw3.dll", dep.cpp_info.bindirs[0], "./Distribution/Release")
                self.copy_with_symlink_check("glslc.exe", dep.cpp_info.bindirs[0], "./Bootstrap-Tools")
                self.copy_with_symlink_check("glslc", dep.cpp_info.bindirs[0], "./Bootstrap-Tools")
            if dep.cpp_info.libdirs:
                self.copy_with_symlink_check("libglfw.so", dep.cpp_info.libdirs[0], "./Distribution/Debug")
                self.copy_with_symlink_check("libglfw.so", dep.cpp_info.libdirs[0], "./Distribution/Release")

    def copy_with_symlink_check(self, file_name, src_folder, dest_folder):
        """Copy the file from src_folder to dest_folder, derefencing symlinks."""
        src_path = os.path.join(src_folder, file_name)

        if os.path.islink(src_path):
            shutil.copy2(src_path, os.path.join(dest_folder, file_name), follow_symlinks=True) # Dereference symlinks
        else:
            copy(self, file_name, src_folder, dest_folder)