package(default_visibility = ["//visibility:public"])

# Test access to the library internals.
#
# Compiled as C, matching how the library ships, so that the code under test is
# not reinterpreted by a C++ compiler. Built once per TLS configuration since
# the connection code differs between them.
cc_library(
    name = "polaris_test_shim_tls",
    testonly = True,
    srcs = ["polaris_test_shim.c"],
    hdrs = ["polaris_test_shim.h"],
    copts = ["-DPOLARIS_ENABLE_TLS=1"],
    defines = ["POLARIS_ENABLE_TLS=1"],
    deps = [
        "//c:polaris_client_test_sources",
        "@boringssl//:ssl",
    ],
)

cc_library(
    name = "polaris_test_shim_no_tls",
    testonly = True,
    srcs = ["polaris_test_shim.c"],
    hdrs = ["polaris_test_shim.h"],
    copts = ["-DPOLARIS_ENABLE_TLS=0"],
    defines = ["POLARIS_ENABLE_TLS=0"],
    deps = ["//c:polaris_client_test_sources"],
)

cc_test(
    name = "polaris_c_test",
    size = "small",
    srcs = ["polaris_c_test.cc"],
    deps = [
        ":polaris_test_shim_tls",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "polaris_c_no_tls_test",
    size = "small",
    srcs = ["polaris_c_test.cc"],
    deps = [
        ":polaris_test_shim_no_tls",
        "@com_google_googletest//:gtest_main",
    ],
)

# Unit tests for the C++ client.
#
# These exercise the reconnect behavior against a stand-in endpoint on loopback,
# so they take a few seconds of wall time.
cc_test(
    name = "polaris_client_test",
    size = "medium",
    srcs = ["polaris_client_test.cc"],
    deps = [
        "//:polaris_client",
        "@com_google_googletest//:gtest_main",
    ],
)
