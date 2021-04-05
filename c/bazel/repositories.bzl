load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def dependencies():
    #---------------------------------------------------------------------------
    # Google BoringSSL
    #
    # Optional; only used if the system is compiled with POLARIS_USE_SSL
    # defined.
    maybe(
        git_repository,
        name = "boringssl",
        commit = "87f3087d6343b89142d1191388a5885d74459df2",
        # 2020/4/7
        remote = "https://boringssl.googlesource.com/boringssl",
        shallow_since = "1586306564 +0000",
    )
