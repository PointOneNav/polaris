load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

def load_dependencies():
    # Imports:
    # - bazel_skylib (v0.9.0 - 2019/7/12)
    # - net_zlib_zlib (v1.2.11 - 2017/1/15)
    # - org_bzip_bzip2 (v1.0.6 - 2018/11/3)
    # - org_lzma_lzma (v5.2.3)
    # - boost (v1.68.0 - 2018/8/9)
    boost_deps()
