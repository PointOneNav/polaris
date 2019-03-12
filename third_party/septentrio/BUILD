licenses(["notice"])  # Septentrio free use.

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "septentrio",
    srcs = [
        "crc.c",
        "sbfread.c",
        "sbfread_meas.c",
        "sbfsvid.c",
    ],
    hdrs = [
        "crc.h",
        "measepoch.h",
        "measepochconfig.h",
        "sbfdef.h",
        "sbfread.h",
        "sbfsigtypes.h",
        "sbfsvid.h",
        "ssntypes.h",
        "sviddef.h",
    ],
    copts = [
        "-DNO_DECRYPTION",
    ],
)
