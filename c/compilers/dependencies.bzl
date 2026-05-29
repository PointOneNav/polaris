load(
    "//compilers/arm:dependencies.bzl",
    arm_dependencies = "cross_compiler_dependencies",
)

def cross_compiler_dependencies(**kwargs):
    arm_dependencies()
