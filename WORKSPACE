workspace(name = "polaris")

#-------------------------------------------------------------------------------
# Point One cross-compilation support
#
# See https://github.com/curtismuntz/bazel_compilers/tree/boost-example.
load("//compilers:dependencies.bzl", "cross_compiler_dependencies")

cross_compiler_dependencies()

#-------------------------------------------------------------------------------
# Import Polaris Client dependencies.
load("//bazel:repositories.bzl", polaris_dependencies = "dependencies")

polaris_dependencies()

#-------------------------------------------------------------------------------
# Import dependencies for example applications (not needed by the library
# itself).
load("//bazel:examples_repositories.bzl", polaris_examples_dependencies = "dependencies")

polaris_examples_dependencies()

load("//bazel:examples_dependencies.bzl", load_polaris_examples_dependencies = "load_dependencies")

load_polaris_examples_dependencies()
