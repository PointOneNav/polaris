package(default_visibility = ["//visibility:public"])

# An example NTRIP server that forwards corrections from Polaris to connected
# NTRIP clients.
cc_binary(
    name = "ntrip_example",
    srcs = ["ntrip_example.cc"],
    data = ["index.html"],
    deps = [
        ":ntrip_server_lib",
        "//:polaris_client",
        "@boost//:asio",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

# Simple NTRIP client meant for testing the NTRIP server example.
cc_binary(
    name = "ntrip_example_client",
    srcs = ["ntrip_example_test_client.cc"],
    deps = [
        "@boost//:asio",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)

# Simple NTRIP server library.
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
        "@boost//:asio",
        "@boost//:date_time",
        "@boost//:system",
        "@boost//:tribool",
        "@com_github_gflags_gflags//:gflags",
        "@com_github_google_glog//:glog",
    ],
)
