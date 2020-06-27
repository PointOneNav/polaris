package(default_visibility = ["//visibility:public"])

# Base Polaris protocol library. Header only.
cc_library(
    name = "polaris",
    hdrs = [
        "include/point_one/polaris.h",
        "include/point_one/polaris_internal.h",
    ],
    includes = [
        "include",
    ],
)

# Client written using Boost.Asio and Glog.
# Could make clients using other networking libraries.
cc_library(
    name = "polaris_asio_client",
    hdrs = [
        "include/point_one/polaris_asio_client.h",
        "include/point_one/polaris_asio_embedded_client.h",
    ],
    includes = [
        "include",
    ],
    deps = [
        "//:polaris",
        "//external:glog",
        "@boost//:asio",
        "@boost//:bind",
        "@boost//:property_tree",
        "@boost//:system",
        "@boringssl//:ssl",
    ],
)
