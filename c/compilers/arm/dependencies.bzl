load(
    "//compilers/arm/linaro_linux_gcc_aarch64:dependencies.bzl",
    linaro_aarch64_dependencies = "cross_compiler_dependencies",
)
load(
    "//compilers/arm/linaro_linux_gcc_armv7:dependencies.bzl",
    linaro_armv7_dependencies = "cross_compiler_dependencies",
)

def cross_compiler_dependencies(**kwargs):
    linaro_aarch64_dependencies()
    linaro_armv7_dependencies()
