package(default_visibility = ["//visibility:public"])

# Simple client example with no connection error detection logic.
cc_binary(
    name = "simple_polaris_client",
    srcs = ["simple_polaris_client.c"],
    deps = [
        "//:polaris_client",
    ],
)

# Example application including connection timeout and retry logic.
cc_binary(
    name = "connection_retry",
    srcs = ["connection_retry.c"],
    deps = [
        "//:polaris_client",
    ],
)
