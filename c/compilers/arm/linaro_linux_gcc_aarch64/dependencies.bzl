load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def cross_compiler_dependencies(**kwargs):
    aarch64_name = "org_linaro_components_toolchain_gcc_aarch64"
    aarch64_sha266 = "27f1dc2c491ed61ae8f0d4b0c11de59cd2f7dd9c94761ee7153006fcac1bf9ab"
    aarch64_prefix = "gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu"
    aarch64_url = "https://releases.linaro.org/components/toolchain/binaries/7.4-2019.02/aarch64-linux-gnu/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu.tar.xz"

    if aarch64_name not in native.existing_rules():
        http_archive(
            name = aarch64_name,
            strip_prefix = aarch64_prefix,
            url = aarch64_url,
            sha256 = aarch64_sha266,
            build_file_content = """
package(default_visibility = ["//visibility:public"])

filegroup(
    name = "gcc",
    srcs = [
        "bin/aarch64-linux-gnu-gcc",
    ],
)

filegroup(
    name = "ar",
    srcs = [
        "bin/aarch64-linux-gnu-ar",
    ],
)

filegroup(
    name = "ld",
    srcs = [
        "bin/aarch64-linux-gnu-ld",
    ],
)

filegroup(
    name = "nm",
    srcs = [
        "bin/aarch64-linux-gnu-nm",
    ],
)

filegroup(
    name = "objcopy",
    srcs = [
        "bin/aarch64-linux-gnu-objcopy",
    ],
)

filegroup(
    name = "objdump",
    srcs = [
        "bin/aarch64-linux-gnu-objdump",
    ],
)

filegroup(
    name = "strip",
    srcs = [
        "bin/aarch64-linux-gnu-strip",
    ],
)

filegroup(
    name = "as",
    srcs = [
        "bin/aarch64-linux-gnu-as",
    ],
)

filegroup(
    name = "compiler_pieces",
    srcs = glob([
        "aarch64-linux-gnu/**",
        "libexec/**",
        "lib/gcc/aarch64-linux-gnu/**",
        "aarch64-linux-gnu/libc/usr/include/**",
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
