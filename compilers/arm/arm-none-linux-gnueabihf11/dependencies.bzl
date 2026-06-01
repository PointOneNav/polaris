load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def cross_compiler_dependencies(**kwargs):
    armv7_name = "arm-none-linux-gnueabihf11"
    armv7_sha256 = "c254f7199261fe76c32ef42187502839bda7efad0a66646cf739d074eff45fad"
    armv7_prefix = "gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf"
    armv7_urls = [
        "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz",
        "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf.tar.xz",
    ]

    if armv7_name not in native.existing_rules():
        http_archive(
            name = armv7_name,
            urls = armv7_urls,
            strip_prefix = armv7_prefix,
            sha256 = armv7_sha256,
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
