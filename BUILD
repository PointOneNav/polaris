package(default_visibility = ["//visibility:public"])

# Base Polaris protocol library. Header only.
cc_library(
    name = "polaris",
    hdrs = [
        "include/point_one/polaris_internal.h",
        "include/point_one/polaris.h",
    ],
    includes = [
        "include",
    ]
)

# Client written using Boost.Asio and Glog.
# Could make clients using other networking libraries.
cc_library(
    name = "polaris_asio_client",
    hdrs = [
        "include/point_one/polaris_asio_client.h",
    ],
    includes = [
        "include",
    ],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:glog",
        "//:polaris",
    ],
)
