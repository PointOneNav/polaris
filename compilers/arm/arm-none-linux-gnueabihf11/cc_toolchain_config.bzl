#
# Copyright 2019 The Bazel Authors. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Tests compiling using an external Linaro toolchain on a Linux machine
#

"""armv7 (ARM 32-bit) C/C++ toolchain."""

load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "action_config",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
    "with_feature_set",
)

def _impl(ctx):
    toolchain_identifier = "arm-none-linux-gnueabihf"
    host_system_name = "arm-none-linux-gnueabihf"
    target_system_name = "arm_a15"
    target_cpu = "arm-none-linux-gnueabihf"
    target_libc = "gcc"
    compiler = "gcc"
    abi_version = "eabi"
    abi_libc_version = "2.35"
    cc_target_os = None

    # Specify the sysroot.
    builtin_sysroot = None
    sysroot_feature = feature(
        name = "sysroot",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.preprocess_assemble,
                    ACTION_NAMES.linkstamp_compile,
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_module_codegen,
                    ACTION_NAMES.lto_backend,
                    ACTION_NAMES.clif_match,
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = ["--sysroot=%{sysroot}"],
                        expand_if_available = "sysroot",
                    ),
                ],
            ),
        ],
    )

    # Support both opt and dbg compile modes (flags defined below).
    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")

    all_compile_actions = [
        ACTION_NAMES.assemble,
        ACTION_NAMES.preprocess_assemble,
        ACTION_NAMES.linkstamp_compile,
        ACTION_NAMES.c_compile,
        ACTION_NAMES.cpp_compile,
        ACTION_NAMES.cpp_header_parsing,
        ACTION_NAMES.cpp_module_compile,
        ACTION_NAMES.cpp_module_codegen,
        ACTION_NAMES.lto_backend,
        ACTION_NAMES.clif_match,
    ]

    # Compiler flags that are _not_ filtered by the `nocopts` rule attribute.
    unfiltered_compile_flags_feature = feature(
        name = "unfiltered_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )

    # Default compiler settings (in addition to unfiltered settings above).
    default_compile_flags_feature = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "--sysroot=external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/libc",
                            # "-mfloat-abi=hard",
                            "-nostdinc",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/lib/gcc/arm-none-linux-gnueabihf/11.2.1/include",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/lib/gcc/arm-none-linux-gnueabihf/11.2.1/include-fixed",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/include/c++/11.2.1",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/libc/usr/include",
                            "-U_FORTIFY_SOURCE",
                            "-fstack-protector",
                            "-fPIE",
                            "-Wno-psabi",
                            "-fdiagnostics-color=always",
                            "-Wall",
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = ["-g"])],
                with_features = [with_feature_set(features = ["dbg"])],
            ),
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-g0",
                            "-O2",
                            "-DNDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                ],
                with_features = [with_feature_set(features = ["opt"])],
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.linkstamp_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_module_codegen,
                    ACTION_NAMES.lto_backend,
                    ACTION_NAMES.clif_match,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/include/c++/11.2.1/arm-none-linux-gnueabihf",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/include/c++/11.2.1",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/include/c++/11.2.1/arm-none-linux-gnueabihf",
                            "-isystem",
                            "external/arm-none-linux-gnueabihf11/include/c++/11.2.1",
                        ],
                    ),
                ],
            ),
        ],
    )

    # User compile flags specified through --copt/--cxxopt/--conlyopt.
    user_compile_flags_feature = feature(
        name = "user_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = ["%{user_compile_flags}"],
                        iterate_over = "user_compile_flags",
                        expand_if_available = "user_compile_flags",
                    ),
                ],
            ),
        ],
    )

    # Define linker settings.
    all_link_actions = [
        ACTION_NAMES.cpp_link_executable,
        ACTION_NAMES.cpp_link_dynamic_library,
        ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ]

    default_link_flags_feature = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "--sysroot=external/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/libc",
                            "-lstdc++",
                            "-latomic",
                            "-lm",
                            "-lpthread",
                            "-Ltools/arm/arm-none-linux-gnueabihf11/clang_more_libs",
                            "-Lexternal/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/lib",
                            "-Lexternal/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/libc/lib",
                            "-Lexternal/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/libc/usr/lib",
                            "-Bexternal/arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf/bin",
                            "-Wl,--dynamic-linker=/lib/ld-linux-armhf.so.3",
                            "-no-canonical-prefixes",
                            "-Wl,-z,relro,-z,now",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-pie",
                            "-Wl,-z,relro,-z,now",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = all_link_actions,
                flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
                with_features = [with_feature_set(features = ["opt"])],
            ),
        ],
    )

    # Define objcopy settings.
    objcopy_embed_data_action = action_config(
        action_name = "objcopy_embed_data",
        enabled = True,
        tools = [
            tool(path = "arm-none-linux-gnueabihf11/arm-none-linux-gnueabihf-objcopy"),
        ],
    )

    action_configs = [objcopy_embed_data_action]

    objcopy_embed_flags_feature = feature(
        name = "objcopy_embed_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = ["objcopy_embed_data"],
                flag_groups = [flag_group(flags = ["-I", "binary"])],
            ),
        ],
    )

    # Define build support flags.
    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)
    supports_pic_feature = feature(name = "supports_pic", enabled = True)

    asan_feature = feature(
        name = "asan",
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [flag_group(flags = [
                    "-g",
                    "-fsanitize=address",
                ])],
            ),
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [flag_group(flags = [
                    "-fsanitize=address",
                ])],
            ),
        ],
    )

    # Create the list of all available build features.
    features = [
        default_compile_flags_feature,
        default_link_flags_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        objcopy_embed_flags_feature,
        opt_feature,
        dbg_feature,
        user_compile_flags_feature,
        sysroot_feature,
        unfiltered_compile_flags_feature,
        asan_feature,
    ]

    # Specify the built-in include directories (set with -isystem).
    cxx_builtin_include_directories = [
        "%package(@arm-none-linux-gnueabihf11//include)%",
        "%package(@arm-none-linux-gnueabihf11//arm-none-linux-gnueabihf/libc/usr/include)%",
        "%package(@arm-none-linux-gnueabihf11//arm-none-linux-gnueabihf/libc/usr/lib/include)%",
        "%package(@arm-none-linux-gnueabihf11//arm-none-linux-gnueabihf/libc/lib/gcc/arm-none-linux-gnueabihf/11.2.1/include-fixed)%",
        "%package(@arm-none-linux-gnueabihf11//include)%/c++/11.2.1",
        "%package(@arm-none-linux-gnueabihf11//arm-none-linux-gnueabihf/libc/lib/gcc/arm-none-linux-gnueabihf/11.2.1/include)%",
        "%package(@arm-none-linux-gnueabihf11//lib/gcc/arm-none-linux-gnueabihf/11.2.1/include)%",
        "%package(@arm-none-linux-gnueabihf11//lib/gcc/arm-none-linux-gnueabihf/11.2.1/include-fixed)%",
        "%package(@arm-none-linux-gnueabihf11//arm-none-linux-gnueabihf/include)%/c++/11.2.1",
    ]

    # List the names of any generated artifacts.
    artifact_name_patterns = []

    # Set any variables to be passed to make.
    make_variables = []

    # Specify the location of all the build tools.
    tool_paths = [
        tool_path(
            name = "ar",
            path = "arm-none-linux-gnueabihf-ar",
        ),
        tool_path(
            name = "compat-ld",
            path = "arm-none-linux-gnueabihf-ld",
        ),
        tool_path(
            name = "cpp",
            path = "arm-none-linux-gnueabihf-gcc",
        ),
        tool_path(
            name = "gcc",
            path = "arm-none-linux-gnueabihf-gcc",
        ),
        tool_path(
            name = "ld",
            path = "arm-none-linux-gnueabihf-ld",
        ),
        tool_path(
            name = "nm",
            path = "arm-none-linux-gnueabihf-nm",
        ),
        tool_path(
            name = "objcopy",
            path = "arm-none-linux-gnueabihf-objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "arm-none-linux-gnueabihf-objdump",
        ),
        tool_path(
            name = "strip",
            path = "arm-none-linux-gnueabihf-strip",
        ),
        # Note: These don't actually exist and don't seem to get used, but they
        # are required by cc_binary() anyway.
        tool_path(
            name = "dwp",
            path = "arm-none-linux-gnueabihf-dwp",
        ),
        tool_path(
            name = "gcov",
            path = "arm-frc-linux-gnueabi/arm-frc-linux-gnueabi-gcov-4.9",
        ),
    ]

    # Not sure what this is for but everybody's doing it (including the default
    # built-in toolchain config @
    # https://github.com/bazelbuild/bazel/blob/master/tools/cpp/cc_toolchain_config.bzl)
    # so...
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.write(out, "Fake executable")

    # Create and return the CcToolchainConfigInfo object.
    return [
        cc_common.create_cc_toolchain_config_info(
            ctx = ctx,
            features = features,
            action_configs = action_configs,
            artifact_name_patterns = artifact_name_patterns,
            cxx_builtin_include_directories = cxx_builtin_include_directories,
            toolchain_identifier = toolchain_identifier,
            host_system_name = host_system_name,
            target_system_name = target_system_name,
            target_cpu = target_cpu,
            target_libc = target_libc,
            compiler = compiler,
            abi_version = abi_version,
            abi_libc_version = abi_libc_version,
            tool_paths = tool_paths,
            make_variables = make_variables,
            builtin_sysroot = builtin_sysroot,
            cc_target_os = cc_target_os,
        ),
        DefaultInfo(
            executable = out,
        ),
    ]

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "cpu": attr.string(mandatory = True, values = ["arm-none-linux-gnueabihf"]),
        "compiler": attr.string(mandatory = False, values = ["gcc"]),
    },
    provides = [CcToolchainConfigInfo],
    executable = True,
)
