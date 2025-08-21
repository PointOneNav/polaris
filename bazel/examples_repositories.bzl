load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def dependencies():
    #---------------------------------------------------------------------------
    # Boost
    maybe(
        git_repository,
        name = "com_github_nelhage_rules_boost",
        # 2022/12/21 = Boost v1.81.0
        commit = "18006d8df898d3010d5355774933f451cd4117fa",
        remote = "https://github.com/nelhage/rules_boost",
        repo_mapping = {"@net_zlib_zlib": "@p1_zlib"},
        shallow_since = "1570056263 -0700",
    )
