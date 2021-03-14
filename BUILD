package(default_visibility = ["//visibility:public"])

# Polaris client C++ library.
cc_library(
    name = "polaris_client",
    srcs = [
        "src/point_one/polaris/polarispp.cc",
        "src/point_one/polaris/polarispp_interface.cc",
    ],
    hdrs = [
        "src/point_one/polaris/polarispp.h",
        "src/point_one/polaris/polarispp_interface.h",
    ],
    includes = [
        "src",
    ],
    deps = [
        "//c:polaris_client",
    ],
)
