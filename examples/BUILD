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

# An example of obtaining receiver positions from a serial NMEA-0183 stream and
# forwarding incoming Polaris corrections over the same serial connection.
cc_binary(
    name = "serial_port_example",
    srcs = ["serial_port_example.cc"],
    deps = [
        ":simple_serial_port",
        "//:polaris_client",
        "@boost//:asio",
        "@com_github_gflags_gflags//:gflags",
    ],
)

# Asynchronous serial port support using the Boost.Asio library.
cc_library(
    name = "simple_serial_port",
    hdrs = [
        "simple_asio_serial_port.h",
    ],
    deps = [
        "@boost//:asio",
        "@boost//:system",
        "@com_github_google_glog//:glog",
    ],
)
