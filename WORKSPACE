workspace(name = "polaris")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

#----------- Cross-Compilation Support ---------------
load("//compilers:dependencies.bzl", "cross_compiler_dependencies")
cross_compiler_dependencies()

#----------- gflags ---------------
http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

bind(
    name = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)

#----------- glog ---------------
git_repository(
    name = "com_google_glog",
    commit = "781096619d3dd368cfebd33889e417a168493ce7",
    remote = "https://github.com/google/glog.git",
    shallow_since = "1542766478 +0900",
)

#---------------ssl --------------
git_repository(
    name = "boringssl",
    commit = "87f3087d6343b89142d1191388a5885d74459df2",
    remote = "https://boringssl.googlesource.com/boringssl",
    shallow_since = "1586306564 +0000",
)

bind(
    name = "glog",
    actual = "@com_google_glog//:glog",
)

#----------- boost ---------------
git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1570056263 -0700",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
