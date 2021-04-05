package(default_visibility = ["//visibility:public"])

# Polaris client C library - all messages and supporting code.
cc_library(
    name = "polaris_client",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
)

# Polaris client C library with TLS enabled.
cc_library(
    name = "polaris_client_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DPOLARIS_USE_SSL=1"],
    includes = ["src"],
    deps = [
        "@boringssl//:ssl",
    ],
)
