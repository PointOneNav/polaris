load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def dependencies():
    #---------------------------------------------------------------------------
    # Boost
    maybe(
        git_repository,
        name = "com_github_nelhage_rules_boost",
        # 2019/10/2 = Boost v1.68.0
        commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
        remote = "https://github.com/nelhage/rules_boost",
        repo_mapping = {"@net_zlib_zlib": "@p1_zlib"},
        shallow_since = "1570056263 -0700",
    )
