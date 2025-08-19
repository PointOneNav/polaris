workspace(name = "polaris")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

#-------------------------------------------------------------------------------
http_archive(
    name = "platforms",
    sha256 = "218efe8ee736d26a3572663b374a253c012b716d8af0c07e842e82f238a0a7ee",
    # v0.0.10 - 2024/04/25
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.10/platforms-0.0.10.tar.gz",
        "https://github.com/bazelbuild/platforms/releases/download/0.0.10/platforms-0.0.10.tar.gz",
    ],
)

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
