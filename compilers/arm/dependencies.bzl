load(
    "//compilers/arm/aarch64-none-linux-gnu11:dependencies.bzl",
    aarch64_arm_linux_gnu11_dependencies = "cross_compiler_dependencies",
)
load(
    "//compilers/arm/arm-none-linux-gnueabihf11:dependencies.bzl",
    armv7_arm_linux_gnu11_dependencies = "cross_compiler_dependencies",
)

def cross_compiler_dependencies(**kwargs):
    aarch64_arm_linux_gnu11_dependencies()
    armv7_arm_linux_gnu11_dependencies()
