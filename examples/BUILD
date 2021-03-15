package(default_visibility = ["//visibility:public"])

# Simple example of connecting to the Polaris service.
cc_binary(
    name = "simple_polaris_client",
    srcs = ["simple_polaris_client.cc"],
    deps = [
        "//:polaris_client",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

## An example of using boost asio to connect to Polaris and forward corrections
## to a serial port.
#cc_binary(
#    name = "asio_example",
#    srcs = ["asio_example.cc"],
#    deps = [
#        ":simple_serial_port",
#        "//:polaris_asio_client",
#        "//external:gflags",
#        "@boost//:asio",
#        "@boost//:system",
#    ],
#)
#
## Simple example of connecting to the Polaris service using the embedded
## keying method.
#cc_binary(
#    name = "simple_embedded_client",
#    srcs = ["simple_embedded_client.cc"],
#    deps = [
#        "//:polaris_asio_client",
#        "//external:gflags",
#        "//external:glog",
#    ],
#)
#
## Example of forwarding RTCM corrections to a Septentrio receiver over serial.
#cc_binary(
#    name = "septentrio_example",
#    srcs = ["septentrio_example.cc"],
#    deps = [
#        ":septentrio_service",
#        "//:polaris_asio_client",
#        "@com_github_gflags_gflags//:gflags",
#        "@com_github_google_glog//:glog",
#    ],
#)
#
## Simple asio Septentrio client uisng Septentrio's official library.
#cc_library(
#    name = "septentrio_service",
#    hdrs = [
#        "sbf_framer.h",
#        "septentrio_interface.h",
#        "septentrio_service.h",
#    ],
#    deps = [
#        ":simple_serial_port",
#        "//third_party/septentrio",
#        "@com_github_google_glog//:glog",
#    ],
#)
#
## Uses boost asio to make a Polaris client.
#cc_library(
#    name = "simple_serial_port",
#    hdrs = [
#        "simple_asio_serial_port.h",
#    ],
#    deps = [
#        "//external:glog",
#        "@boost//:asio",
#        "@boost//:system",
#    ],
#)
