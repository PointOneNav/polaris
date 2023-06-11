load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def dependencies():
    #---------------------------------------------------------------------------
    # Bazel Skylib (used by the Bazel build system - not a software dependency)
    maybe(
        http_archive,
        name = "bazel_skylib",
        urls = [
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        ],
        sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
    )

    #---------------------------------------------------------------------------
    # Google BoringSSL
    #
    # Optional; only used if the system is compiled with POLARIS_USE_SSL
    # defined.
    maybe(
        git_repository,
        name = "boringssl",
        commit = "5cc6bd5367d1046af217f56bf7266008e93029b2",
        remote = "https://boringssl.googlesource.com/boringssl",
    )
