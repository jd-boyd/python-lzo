import os
import subprocess
import sys
import shutil
import pathlib
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Required for auto-detection & inclusion of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"

        # Check for USE_SYSTEM_LZO environment variable
        use_system_lzo = os.environ.get("USE_SYSTEM_LZO", "").lower() in (
            "1",
            "true",
            "on",
            "yes",
        )

        # CMake lets you override the generator - we need to check this.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
            f"-DUSE_SYSTEM_LZO={use_system_lzo}",
        ]

        build_args = []

        # Platform-specific configuration
        if self.compiler.compiler_type != "msvc":
            # Unix-like systems
            if not cmake_generator or cmake_generator == "Ninja":
                try:
                    import ninja  # noqa: F401

                    ninja_executable_path = os.path.join(ninja.BIN_DIR, "ninja")
                    cmake_args += [
                        "-GNinja",
                        f"-DCMAKE_MAKE_PROGRAM:FILEPATH={ninja_executable_path}",
                    ]
                except ImportError:
                    pass
        else:
            # Windows MSVC
            cmake_args += [
                f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}",
            ]
            build_args += ["--config", cfg]

        # Set CMAKE_BUILD_PARALLEL_LEVEL to control the parallel build level
        # across all generators.
        if "CMAKE_BUILD_PARALLEL_LEVEL" not in os.environ:
            # self.parallel is a Python 3 only way to set parallel jobs by hand
            # using -j in the build_ext call, not supported by pip or PyPA-build.
            if hasattr(self, "parallel") and self.parallel:
                build_args += [f"-j{self.parallel}"]

        build_temp = os.path.join(self.build_temp, ext.name)
        if not os.path.exists(build_temp):
            os.makedirs(build_temp)

        # Check if we have the lzo-2.10 subdirectory (unless using system lzo)
        if not use_system_lzo:
            lzo_dir = os.path.join(ext.sourcedir, "lzo-2.10")
            if not os.path.exists(lzo_dir):
                raise RuntimeError(
                    f"liblzo source directory not found: {lzo_dir}\n"
                    "Please ensure lzo-2.10 directory exists, or set USE_SYSTEM_LZO=1 "
                    "to use system liblzo"
                )

        # Run CMake configure
        try:
            subprocess.check_call(["cmake", ext.sourcedir] + cmake_args, cwd=build_temp)
        except subprocess.CalledProcessError as e:
            raise RuntimeError(f"CMake configure failed: {e}")

        # Run CMake build
        try:
            subprocess.check_call(
                ["cmake", "--build", "."] + build_args, cwd=build_temp
            )
            extension_path = pathlib.Path(self.get_ext_fullpath(ext.name))
            print("COOKIES!!!!!")
            print("N: ", ext.name)
            print("GEF: ", self.get_ext_fullpath(ext.name))
            _, file_ext = os.path.splitext(self.get_ext_fullpath(ext.name))
            print("EXT_2: ", file_ext)
            src = build_temp + file_ext
            print("  shutil.copy(", src, ", ", extension_path, ")")

            shutil.copy(src, extension_path)
        except subprocess.CalledProcessError as e:
            raise RuntimeError(f"CMake build failed: {e}")


# Read the contents of your README file
this_directory = pathlib.Path(__file__).parent
readme_file = this_directory / "README.md"
if readme_file.exists():
    long_description = readme_file.read_text(encoding="utf-8")
else:
    long_description = "Python bindings for the LZO compression library"

# Check for lzomodule.c
if not (this_directory / "lzomodule.c").exists():
    raise RuntimeError("lzomodule.c not found in project root")

setup(
    ext_modules=[CMakeExtension("lzo")],
    cmdclass={"build_ext": CMakeBuild},
)
