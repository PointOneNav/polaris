package(default_visibility = ["//visibility:public"])

# Polaris client C++ library.
#
# TLS support is enabled by default, but may be disabled with the following
# command-line argument:
#   --//c:polaris_enable_tls=False
cc_library(
    name = "polaris_client",
    srcs = [
        "src/point_one/polaris/polaris_client.cc",
        "src/point_one/polaris/polaris_interface.cc",
    ],
    hdrs = [
        "src/point_one/polaris/polaris_client.h",
        "src/point_one/polaris/polaris_interface.h",
    ],
    # Note: POLARIS_ENABLE_TLS is inherited from //c:polaris_client.
    includes = [
        "src",
    ],
    deps = [
        "//c:polaris_client",
        "@com_github_google_glog//:glog",
    ],
)
