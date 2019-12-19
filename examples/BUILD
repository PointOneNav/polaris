package(default_visibility = ["//visibility:public"])

# An example of using boost asio to connect to Polaris and forward corrections
# to a serial port.
cc_binary(
    name = "asio_example",
    srcs = ["asio_example.cc"],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:gflags",
        "//:polaris_asio_client",
        ":simple_serial_port",
    ],
)

# Example that starts the septentrio receiver.
cc_binary(
    name = "septentrio_example",
    srcs = ["septentrio_example.cc"],
    deps = [
        "//external:gflags",
        "//external:glog",
        "//:polaris_asio_client",
        ":septentrio_service",
    ],
)

# Example that starts the septentrio receiver.
cc_binary(
    name = "simple_polaris_client",
    srcs = ["simple_polaris_client.cc"],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:gflags",
        "//external:glog",
        "//:polaris_asio_client",
    ],
)

# Simple asio sepetentrio client uisng septentrio's official library.
cc_library(
    name = "septentrio_service",
    hdrs = [
        "septentrio_service.h",
        "sbf_framer.h",
        "septentrio_interface.h",
    ],
    linkopts = ["-lboost_system"],
    deps = [
        ":simple_serial_port",
        "//external:glog",
        "//third_party/septentrio",
    ],
)

# Uses boost asio to make a Polaris client.
cc_library(
    name = "simple_serial_port",
    hdrs = [
        "simple_asio_serial_port.h",
    ],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:glog",
    ],
)
