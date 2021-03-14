workspace(name = "polaris")

#-------------------------------------------------------------------------------
# Import dependencies.
load("//bazel:repositories.bzl", polaris_dependencies = "dependencies")

polaris_dependencies()

#-------------------------------------------------------------------------------
# Point One cross-compilation support
#
# See https://github.com/curtismuntz/bazel_compilers/tree/boost-example.
load("//compilers:dependencies.bzl", "cross_compiler_dependencies")

cross_compiler_dependencies()
