package(default_visibility = ["//visibility:public"])

# Test ntrip client.
cc_binary(
    name = "ntrip_example_client",
    srcs = ["ntrip_example_test_client.cc"],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:gflags",
        "//external:glog",
    ],
)

# An example of using boost asio to connect to Polaris and forward corrections
# to an ntrip server.
cc_binary(
    name = "ntrip_example",
    srcs = ["ntrip_example.cc"],
    data = ["index.html"],
    linkopts = ["-lboost_system"],
    deps = [
        "//external:gflags",
        "//external:glog",
        ":ntrip_server_lib",
        "//:polaris_asio_client",

    ],
)

# Simple ntrip server library.
cc_library(
    name = "ntrip_server_lib",
    data = ["index.html"],
    hdrs = ["ntrip_server.h", "connection.h", "connection_manager.h", "reply.h", "request.h", "request_handler.h", "request_parser.h" ,"header.h", "mime_types.h"],
    srcs = ["ntrip_server.cc", "connection.cc", "connection_manager.cc", "reply.cc", "request_handler.cc", "request_parser.cc", "mime_types.cc"],
    linkopts = ["-lboost_system", "-lboost_date_time"],
    deps = [
        "//external:gflags",
        "//external:glog",
    ],
)
