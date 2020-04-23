package(default_visibility = ["//visibility:public"])

# Test ntrip client.
cc_binary(
    name = "ntrip_example_client",
    srcs = ["ntrip_example_test_client.cc"],
    deps = [
        "//external:gflags",
        "//external:glog",
        "@boost//:asio",
        "@boost//:system",
    ],
)

# An example of using boost asio to connect to Polaris and forward corrections
# to an ntrip server.
cc_binary(
    name = "ntrip_example",
    srcs = ["ntrip_example.cc"],
    data = ["index.html"],
    deps = [
        ":ntrip_server_lib",
        "//:polaris_asio_client",
        "//external:gflags",
        "//external:glog",
        "@boost//:asio",
        "@boost//:system",
    ],
)

# Simple ntrip server library.
cc_library(
    name = "ntrip_server_lib",
    srcs = [
        "connection.cc",
        "connection_manager.cc",
        "mime_types.cc",
        "ntrip_server.cc",
        "reply.cc",
        "request_handler.cc",
        "request_parser.cc",
    ],
    hdrs = [
        "connection.h",
        "connection_manager.h",
        "header.h",
        "mime_types.h",
        "ntrip_server.h",
        "reply.h",
        "request.h",
        "request_handler.h",
        "request_parser.h",
    ],
    data = ["index.html"],
    deps = [
        "//external:gflags",
        "//external:glog",
        "@boost//:asio",
        "@boost//:date_time",
        "@boost//:system",
    ],
)
