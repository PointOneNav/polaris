load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

package(default_visibility = ["//visibility:public"])

# Option to disable _all_ print messages:
#   --//:polaris_enable_print       [enabled; default]
#   --//:polaris_enable_print=True  [enabled; default]
#   --//:polaris_enable_print=False [disabled]
#   --no//:polaris_enable_print     [disabled]
bool_flag(
    name = "polaris_enable_print",
    build_setting_default = True,
)

config_setting(
    name = "print_enabled",
    flag_values = {
        ":polaris_enable_print": "True",
    },
)

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

# Polaris client C library sources, exposed for the unit tests.
#
# The tests #include polaris.c directly so they can exercise its file-static
# functions and interpose on the socket calls it makes. The sources are exported
# as textual headers rather than compiled here so that including them does not
# also link a second copy.
cc_library(
    name = "polaris_client_test_sources",
    testonly = True,
    includes = ["src"],
    textual_hdrs = glob([
        "src/**/*.c",
        "src/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

# Polaris client C library - all messages and supporting code -  with TLS
# enabled.
cc_library(
    name = "polaris_client_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DPOLARIS_ENABLE_TLS=1"] +
            select({
                ":print_enabled": [],
                "//conditions:default": ["-DPOLARIS_NO_PRINT"],
            }),
    includes = ["src"],
    # pthread_sigmask() is used to block SIGPIPE around TLS socket writes on
    # Linux.
    linkopts = ["-pthread"],
    deps = ["@boringssl//:ssl"],
)

# Polaris client C library - all messages and supporting code - _without_ TLS
# enabled (not recommended).
cc_library(
    name = "polaris_client_no_tls",
    srcs = glob(["src/**/*.c"]),
    hdrs = glob(["src/**/*.h"]),
    copts = ["-DPOLARIS_ENABLE_TLS=0"] +
            select({
        ":print_enabled": [],
        "//conditions:default": ["-DPOLARIS_NO_PRINT"],
    }),
    includes = ["src"],
)
