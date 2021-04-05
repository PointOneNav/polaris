package(default_visibility = ["//visibility:public"])

# Polaris client C++ library.
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
    includes = [
        "src",
    ],
    deps = [
        "//c:polaris_client_tls",
        "@com_github_google_glog//:glog",
    ],
)
