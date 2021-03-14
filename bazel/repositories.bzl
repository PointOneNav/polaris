load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def dependencies():
    #---------------------------------------------------------------------------
    # Bazel build file formatting tool (buildifier).
    #
    # Needed for //compilers cross-compilation dependencies.
    maybe(
        http_archive,
        name = "com_github_bazelbuild_buildtools",
        sha256 = "a5fca3f810588b441a647cf601a42ca98a75aa0681c2e9ade3ce9187d47b506e",
        strip_prefix = "buildtools-3.4.0",
        # v3.4.0 - 2020/7/18
        url = "https://github.com/bazelbuild/buildtools/archive/3.4.0.zip",
    )
