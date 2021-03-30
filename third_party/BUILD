licenses(["notice"])  # Septentrio free use.

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "septentrio",
    srcs = [
        "septentrio/crc.c",
        "septentrio/sbfread.c",
        "septentrio/sbfread_meas.c",
        "septentrio/sbfsvid.c",
    ],
    hdrs = [
        "septentrio/crc.h",
        "septentrio/measepoch.h",
        "septentrio/measepochconfig.h",
        "septentrio/sbfdef.h",
        "septentrio/sbfread.h",
        "septentrio/sbfsigtypes.h",
        "septentrio/sbfsvid.h",
        "septentrio/ssntypes.h",
        "septentrio/sviddef.h",
    ],
    copts = [
        "-DNO_DECRYPTION",
    ],
    includes = ["."],
)
