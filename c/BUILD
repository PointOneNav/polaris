package(default_visibility = ["//visibility:public"])

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

# Option to enable/disable TLS support (enabled by default):
#   --//:polaris_enable_tls       [enabled; default]
#   --//:polaris_enable_tls=True  [enabled; default]
#   --//:polaris_enable_tls=False [disabled]
#   --no//:polaris_enable_tls     [disabled]
bool_flag(
    name = "polaris_enable_tls",
    build_setting_default = True,
)

config_setting(
    name = "tls_enabled",
    flag_values = {
        ":polaris_enable_tls": "True",
    },
)

# Polaris client C library - all messages and supporting code.
#
# TLS support is enabled by default, but may be disabled with the following
# command-line argument:
#   --//:polaris_enable_tls=False
cc_library(
    name = "polaris_client",
    deps = select({
        ":tls_enabled": [":polaris_client_tls"],
        "//conditions:default": [":polaris_client_no_tls"],
    }),
)

# Polaris client C library - all messages and supporting code -  with TLS
# enabled.
cc_library(
    name = "polaris_client_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DPOLARIS_USE_TLS=1"],
    includes = ["src"],
    deps = ["@boringssl//:ssl"],
)

# Polaris client C library - all messages and supporting code - _without_ TLS
# enabled (not recommended).
cc_library(
    name = "polaris_client_no_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    includes = ["src"],
)
