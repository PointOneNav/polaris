package(default_visibility = ["//visibility:public"])

# Async Septentrio receiver support using Septentrio's official library.
cc_library(
    name = "septentrio_service",
    hdrs = [
        "sbf_framer.h",
        "septentrio_interface.h",
        "septentrio_service.h",
    ],
    deps = [
        "//examples:simple_serial_port",
        "//third_party/septentrio",
        "@com_github_google_glog//:glog",
    ],
)
