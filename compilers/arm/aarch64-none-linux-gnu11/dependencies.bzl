load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def cross_compiler_dependencies(**kwargs):
    aarch64_name = "aarch64-none-linux-gnu11"
    aarch64_sha266 = "52dbac3eb71dbe0916f60a8c5ab9b7dc9b66b3ce513047baa09fae56234e53f3"
    aarch64_prefix = "gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu"
    aarch64_urls = [
        "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu.tar.xz",
        "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu.tar.xz",
    ]

    if aarch64_name not in native.existing_rules():
        http_archive(
            name = aarch64_name,
            urls = aarch64_urls,
            strip_prefix = aarch64_prefix,
            sha256 = aarch64_sha266,
            build_file_content = """
package(default_visibility = ['//visibility:public'])

filegroup(
  name = 'toolchain',
  srcs = glob([
    '**',
  ]),
)
""",
        )
