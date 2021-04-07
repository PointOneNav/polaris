package(default_visibility = ["//visibility:public"])

# Polaris client C library - all messages and supporting code - _without_ TLS
# enabled (not recommended).
cc_library(
    name = "polaris_client_no_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
)

# Polaris client C library with TLS enabled.
cc_library(
    name = "polaris_client_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DPOLARIS_USE_TLS=1"],
    includes = ["src"],
    deps = [
        "@boringssl//:ssl",
    ],
)
