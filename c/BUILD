package(default_visibility = ["//visibility:public"])

# Polaris client C library - all messages and supporting code.
cc_library(
    name = "polaris_client",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
)

# Polaris client C library - all messages and supporting code.
cc_library(
    name = "polaris_client_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DUSE_SSL"],
    includes = ["src"],
    deps = [
        "@boringssl//:ssl",
    ],
)
