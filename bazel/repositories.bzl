load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
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

    #---------------------------------------------------------------------------
    # Google Commandline Flags (gflags)
    #
    # Required by glog.
    maybe(
        http_archive,
        name = "com_github_gflags_gflags",
        sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
        strip_prefix = "gflags-2.2.2",
        # 2018/11/11
        urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
    )

    #---------------------------------------------------------------------------
    # Google Logging Library (glog)
    #
    # Optional; may be omitted if POLARIS_HAVE_GLOG is not defined.
    #
    # Note: Explicitly disabling shallow_since on this repo to avoid a known
    # issue between recent versions of git and Bazel that causes some needed
    # commit data to be omitted incorrectly, resulting in unexpected "Could not
    # parse object" errors. See
    # https://github.com/bazelbuild/bazel/issues/10292.
    maybe(
        git_repository,
        name = "com_github_google_glog",
        # 2020/5/12
        commit = "0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
        remote = "https://github.com/google/glog.git",
    )

    #---------------------------------------------------------------------------
    # Google BoringSSL
    #
    # Optional; only used if the system is compiled with POLARIS_USE_SSL
    # defined.
    maybe(
        git_repository,
        name = "boringssl",
        # 2024/3/25
        commit = "54f64295218630fd869b3a079d9dfb5abfe61324",
        remote = "https://boringssl.googlesource.com/boringssl",
        shallow_since = "1711401451 +0000",
    )
