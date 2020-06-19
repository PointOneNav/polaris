load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def cross_compiler_dependencies(**kwargs):
    armv7_name = "org_linaro_components_toolchain_gcc_armv7"
    armv7_sha256 = "3c951cf1941d0fa06d64cc0d5e88612b209d8123b273fa26c16d70bd7bc6b163"
    armv7_prefix = "gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf"
    armv7_url = "https://releases.linaro.org/components/toolchain/binaries/7.4-2019.02/arm-linux-gnueabihf/gcc-linaro-7.4.1-2019.02-x86_64_arm-linux-gnueabihf.tar.xz"

    if armv7_name not in native.existing_rules():
        http_archive(
            name = armv7_name,
            strip_prefix = armv7_prefix,
            url = armv7_url,
            sha256 = armv7_sha256,
            build_file_content = """
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "gcc",
    srcs = [
        "bin/arm-linux-gnueabihf-gcc",
    ],
)

filegroup(
    name = "ar",
    srcs = [
        "bin/arm-linux-gnueabihf-ar",
    ],
)

filegroup(
    name = "ld",
    srcs = [
        "bin/arm-linux-gnueabihf-ld",
    ],
)

filegroup(
    name = "nm",
    srcs = [
        "bin/arm-linux-gnueabihf-nm",
    ],
)

filegroup(
    name = "objcopy",
    srcs = [
        "bin/arm-linux-gnueabihf-objcopy",
    ],
)

filegroup(
    name = "objdump",
    srcs = [
        "bin/arm-linux-gnueabihf-objdump",
    ],
)

filegroup(
    name = "strip",
    srcs = [
        "bin/arm-linux-gnueabihf-strip",
    ],
)

filegroup(
    name = "as",
    srcs = [
        "bin/arm-linux-gnueabihf-as",
    ],
)

filegroup(
    name = "compiler_pieces",
    srcs = glob([
        "arm-linux-gnueabihf/**",
        "libexec/**",
        "lib/gcc/arm-linux-gnueabihf/**",
        "include/**",
    ]),
)

filegroup(
    name = "compiler_components",
    srcs = [
        ":ar",
        ":as",
        ":gcc",
        ":ld",
        ":nm",
        ":objcopy",
        ":objdump",
        ":strip",
    ],
)
""",
        )
