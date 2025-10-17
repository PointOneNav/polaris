![Polaris Build](https://github.com/PointOneNav/polaris/workflows/Polaris%20Build/badge.svg?branch=master)

[![Total alerts](https://img.shields.io/lgtm/alerts/g/PointOneNav/polaris.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PointOneNav/polaris/alerts/)

[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/PointOneNav/polaris.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PointOneNav/polaris/context:cpp)


# Point One Navigation Polaris Service #

This repo provides C and C++ libraries for interacting with the Point One Navigation Polaris Service to receive GNSS RTK
corrections. Compatible GNSS receivers can achieve better than 10 cm accuracy when used with the service.

Polaris corrections are distributed using the
[RTCM 10403 standard](https://rtcm.myshopify.com/collections/differential-global-navigation-satellite-dgnss-standards/products/rtcm-10403-2-differential-gnss-global-navigation-satellite-systems-services-version-3-february-1-2013).
They may be used with a [Point One Atlas](https://pointonenav.com/devkits) device, or with any RTCM-compatible GNSS
receiver (currently tested with [Septentrio](https://www.septentrio.com/en), [Novatel](https://novatel.com/), and
[u-Blox](https://www.u-blox.com/en) receivers).

Documentation on the protocol used by the Polaris Service can be found at https://pointonenav.github.io/docs.

### Table Of Contents ###

* [Polaris API Key and Unique ID](#polaris-api-key-and-unique-id)
* [Polaris C Client](#polaris-c-client)
  * [Requirements](#requirements)
  * [Installation](#installation)
    * [CMake](#cmake)
      * [Including In Your CMake Project](#including-in-your-cmake-project)
      * [Compiling From Source](#compiling-from-source)
    * [Bazel](#bazel)
      * [Including In Your Bazel Project](#including-in-your-bazel-project)
      * [Compiling From Source](#compiling-from-source-1)
    * [GNU Make](#gnu-make)
  * [Using Polaris C Client](#using-polaris-c-client)
  * [Example Applications](#example-applications)
    * [Simple Polaris Client](#simple-polaris-client)
    * [Connection Retry](#connection-retry)
* [Polaris C++ Client](#polaris-c-client-1)
  * [Requirements](#requirements-1)
  * [Installation](#installation-1)
    * [CMake](#cmake-1)
      * [Including In Your CMake Project](#including-in-your-cmake-project-1)
      * [Compiling From Source](#compiling-from-source-2)
    * [Bazel](#bazel-1)
      * [Including In Your Bazel Project](#including-in-your-bazel-project-1)
      * [Compiling From Source](#compiling-from-source-3)
      * [Cross-Compiling With Bazel](#cross-compiling-with-bazel)
    * [Building On Mac OS](#building-on-mac-os)
  * [Using Polaris C++ Client](#using-polaris-c-client-1)
  * [Example Applications](#example-applications)
    * [Simple Polaris Client](#simple-polaris-client-1)
    * [Generic Serial Receiver Example](#generic-serial-receiver-example)
    * [NTRIP Server Example](#ntrip-server-example)
    * [Septentrio Example](#septentrio-example)
      * [Hardware Setup](#hardware-setup)
      * [Configure AsteRx-m2](#configure-asterx-m2)
      * [Serial Permissions in Ubuntu](#serial-permissions-in-ubuntu)
      * [Running the example](#running-the-example)
      * [Verifying Corrections](#verifying-corrections)

## Polaris API Key and Unique ID ##

To establish a connection, you must provide a valid Polaris API key. You can easily get one at https://app.pointonenav.com.

Each time you connect to Polaris, you must provide both your assigned API key.  Unique IDs are an _optional_ string used to
identify the connection and must be unique across all Polaris sessions using your API key.

**Important: if two connections use the same unique ID, they will conflict and will not work correctly.**

Unique IDs have the following requirements:
- Maximum of 36 characters long
- A mixture of uppercase and lowercase letters and numbers
- May include the following special characters: `-` and `_`

## Polaris C Client ##

### Requirements ###

- [Bazel](https://bazel.build/) 4.2+, or [CMake](https://cmake.org/) 3.6+ and
  [GNU Make](https://www.gnu.org/software/make/)
  - Note: CMake 3.18 or newer required when including the C library using `FetchContent_Declare()`
- [OpenSSL](https://www.openssl.org/) or [BoringSSL](https://boringssl.googlesource.com/boringssl/) (optional; required
  for TLS support (strongly recommended))

### Installation

The Polaris C Client can be built with [Bazel](https://bazel.build/), [CMake](https://cmake.org/), or
[GNU Make](https://www.gnu.org/software/make/). Follow the instructions below for build system of your choice.

#### CMake

##### Install Requirements

Before building the Polaris C Client library, you must install the following requirements:

```bash
sudo apt install libssl-dev
```

OpenSSL (`libssl-dev`) is required by default and strongly recommended, but may be disabled by specifying
`-DPOLARIS_ENABLE_TLS=OFF` to the `cmake` command below.

##### Including In Your CMake Project

To include this library as part of your CMake project, we recommend using the CMake `FetchContent` feature as shown
below, rather than compiling and installing the library manually as in the sections above:
```cmake
# Download the specified version of the FusionEngine client library and make it
# available.
include(FetchContent)
FetchContent_Declare(
    polaris
    GIT_REPOSITORY https://github.com/PointOneNav/polaris.git
    GIT_TAG v1.3.1
    SOURCE_SUBDIR c
)
set(POLARIS_BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(polaris)

# Define your application and add a dependency for the Polaris client C library.
add_executable(example_app main.c)
target_link_libraries(example_app PUBLIC polaris_client)
```

> Note that the `SOURCE_SUBDIR` argument tells CMake to include only the Polaris client C library. This argument was
> introduced in CMake 3.18, so you must use CMake 3.18 or newer when building a C-only project.

Note that we strongly recommend using a specific version of the library in your code by specifying a git tag (e.g.,
`GIT_TAG v1.3.1`), and updating that as new versions are released. That way, you can be sure that your code is always
built with a known version of Polaris client. If you prefer, however, you can tell CMake to track the latest
changes by using `GIT_TAG master` instead.

See [c/examples/external_cmake_project/CMakeLists.txt](c/examples/external_cmake_project/CMakeLists.txt) for more
details.

##### Compiling From Source

Use the following steps to compile and install the Polaris C Client library using CMake:

```
mkdir c/build
cd c/build
cmake ..
make
sudo make install
```

This will generate `libpolaris_client.so`, and install the library and header files on your system. By default,
this will also build the [example applications](#example-applications). You can disable the example applications by
specifying `cmake -DPOLARIS_BUILD_EXAMPLES=OFF ..`.

#### Bazel

##### Including In Your Bazel Project

To use this library in an existing Bazel project, add the following to your project's `WORKSPACE` file:

```python
git_repository(
    name = "polaris",
    remote = "git@github.com:PointOneNav/polaris.git",
    tag = "v1.3.1",
)
```

Note that we strongly recommend using a specific version of the library in your code by specifying a git tag (e.g.,
`tag = "v1.3.1"`), and updating that as new versions are released. That way, you can be sure that your code is always
built with a known version of fusion-engine-client. If you prefer, however, you can tell Bazel to track the latest
changes by using `branch = "master"` instead.

After declaring the repository in your `WORKSPACE` file, you can add the following dependency to any `cc_library()` or
`cc_binary()` definitions in your project's `BAZEL` files:

```python
cc_binary(
    name = "my_application",
    srcs = ["main.cc"],
    deps = ["@_polaris//c:polaris_client"],
)
```

The `polaris_client` target enables TLS support by default, which is strongly recommended. If necessary, you can use the
`polaris_client_no_tls` target to connect without TLS, or specify `--//c:polaris_enable_tls=False` to Bazel on the
command line.

> Note that you do not need to manually clone the Polaris repository when including it in your Bazel application. Bazel
will clone it automatically when you build your application.

##### Compiling From Source

In general, it is strongly recommended that you let Bazel import and compile the library using `git_repository()` as
shown above. However, if you would like to compile the example applications, you can run the following command:

```
bazel build -c opt //c/examples:*
```

The generated example applications will be stored within the Bazel cache directory
(`bazel-bin/examples/<APPLICATION_NAME>`). You may run them directly from the `bazel-bin/` directory if you wish, but
they are more commonly run using the `bazel run` command. For example, to run the
[Simple Polaris Client](#simple-polaris-client) example application, run the following:

```bash
bazel run -c opt //c/examples:simple_polaris_client -- <POLARIS_API_KEY> [<UNIQUE_ID>]
```

See [Simple Polaris Client](#simple-polaris-client) for more details.

#### GNU Make

> Note: The C client does not currently have any external dependencies.

1. Clone the Polaris source code and navigate to the `c/` source directory:
   ```bash
   git clone https://github.com/PointOneNav/polaris.git
   cd polaris/c
   ```
3. Compile the Polaris source code and example applications:
   ```bash
   make
   ```

The generated example applications will be located at `examples/<APPLICATION NAME>` in the current directory
(`polaris/c/`). For example, to run the [Simple Polaris Client](#simple-polaris-client) example application, run the
following:

```bash
./examples/simple_polaris_client <POLARIS_API_KEY> [<UNIQUE_ID>]
```

See [Simple Polaris Client](#simple-polaris-client) for more details.

### Using Polaris C Client ###

1. Include `polaris.h`:
   ```c
   #include <point_one/polaris/polaris.h>
   ```
2. Create a `PolarisContext_t` context object, which contains the data send/receive buffers, socket for communicating
   with Polaris, and other configuration details.
   ```c
   PolarisContext_t context;
   Polaris_Init(&context);
   ```
3. Authenticate with Polaris, providing your assigned API key and a unique ID for this connection instance. This will
   generate an _authentication token_, which will be used to receive corrections data.
   ```c
   Polaris_Authenticate(&context, api_key, unique_id);
   ```
   - Note that the ID you specify must be unique across all Polaris sessions using your API key. If two connections use
     the same unique ID, they will conflict and will not work correctly.
   - If you already have a valid authentication token that you wish to use, you can skip the authentication step by
     calling `Polaris_SetAuthToken()`.
4. Provide a callback function, which will be called when new data is received.
   ```c
   void MyDataHandler(void* info, PolarisContext_t* context, const uint8_t* buffer, size_t size_bytes) {}
   ```
   ```c
   Polaris_SetRTCMCallback(&context, MyDataHandler, NULL);
   ```
5. Connect to the Polaris corrections stream using your authentication token.
   ```c
   Polaris_Connect(&context);
   ```
6. Send an initial receiver position to start receiving corrections for that location.
   ```c
   Polaris_SendLLAPosition(&context, 37.773971, -122.430996, -0.02);
   ```
7. Finally, call the run function, which will block indefinitely and receive data, calling the callback function you
   provided each time data is received.
   ```c
   Polaris_Run(&context, 30000);
   ```
8. When finished, call `Polaris_Disconnect()` from somewhere in your code to disconnect from Polaris and return from
   `Polaris_Run()`.
   ```c
   Polaris_Disconnect(&context);
   ```
9. Finally, call `Polaris_Free()` to free memory held by the context.
   ```c
   Polaris_Free(&context);
   ```

If desired, you can use the `Polaris_Work()` function instead of `Polaris_Run()` to perform non-blocking data receive
operations.

### Example Applications ###

#### Simple Polaris Client ####

A small example of establishing a Polaris connection and receiving RTCM corrections data.

To run the application, run the following command:
```
bazel run //c/examples:simple_polaris_client -- <POLARIS_API_KEY> [<UNIQUE_ID>]
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

#### Connection Retry ####

An extension of the simple example, adding error detection and connection retry logic consistent with how Polaris might
be used in a real-time application.

To run the application, run the following command:
```
bazel run //c/examples:connection_retry -- <POLARIS_API_KEY> [<UNIQUE_ID>]
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

## Polaris C++ Client ##

### Requirements ###

- [Bazel](https://bazel.build/) 4.2+, or [CMake](https://cmake.org/) 3.6+ and
  [GNU Make](https://www.gnu.org/software/make/)
- [Google gflags 2.2.2+](https://github.com/gflags/gflags)
- [Google glog 0.4.0+](https://github.com/google/glog)
- [Boost 1.81+](https://www.boost.org/) (for building example applications only)
- [OpenSSL](https://www.openssl.org/) or [BoringSSL](https://boringssl.googlesource.com/boringssl/) (optional; required
  for TLS support (strongly recommended))

### Installation

The Polaris C++ Client can be built with [Bazel](https://bazel.build/), or [CMake](https://cmake.org/). Follow the
instructions below for build system of your choice.

#### CMake

##### Install Requirements

Before building the Polaris C++ Client library, you must install the following requirements:

```bash
sudo apt install libssl-dev libgflags-dev libgoogle-glog-dev libboost-all-dev
```

OpenSSL (`libssl-dev`) is required by default and strongly recommended, but may be disabled by specifying
`-DPOLARIS_ENABLE_TLS=OFF` to the `cmake` command below.

##### Including In Your CMake Project

To include this library as part of your CMake project, we recommend using the CMake `FetchContent` feature as shown
below, rather than compiling and installing the library manually as in the sections above:
```cmake
# Download the specified version of the FusionEngine client library and make it
# available.
include(FetchContent)
FetchContent_Declare(
    polaris
    GIT_REPOSITORY https://github.com/PointOneNav/polaris.git
    GIT_TAG v1.3.1
)
set(POLARIS_BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(polaris)

# Define your application and add a dependency for the Polaris client C library.
add_executable(example_app main.c)
target_link_libraries(example_app PUBLIC polaris_cpp_client)
```

Note that we strongly recommend using a specific version of the library in your code by specifying a git tag (e.g.,
`GIT_TAG v1.3.1`), and updating that as new versions are released. That way, you can be sure that your code is always
built with a known version of Polaris client. If you prefer, however, you can tell CMake to track the latest
changes by using `GIT_TAG master` instead.

See [examples/external_cmake_project/CMakeLists.txt](examples/external_cmake_project/CMakeLists.txt) for more details.

##### Compiling From Source

Use the following steps to compile and install the Polaris C Client library using CMake:

```
mkdir build
cd build
cmake ..
make
sudo make install
```

This will generate `libpolaris_cpp_client.so`, and install the library and header files on your system. By default,
this will also build the [example applications](#example-applications). You can disable the example applications by
specifying `cmake -DPOLARIS_BUILD_EXAMPLES=OFF ..`.

#### Bazel

##### Including In Your Bazel Project

To use this library in an existing Bazel project, add the following to your project's `WORKSPACE` file:

```python
git_repository(
    name = "polaris",
    remote = "git@github.com:PointOneNav/polaris.git",
    tag = "v1.3.1",
)
```

Note that we strongly recommend using a specific version of the library in your code by specifying a git tag (e.g.,
`tag = "v1.3.1"`), and updating that as new versions are released. That way, you can be sure that your code is always
built with a known version of fusion-engine-client. If you prefer, however, you can tell Bazel to track the latest
changes by using `branch = "master"` instead.

After declaring the repository in your `WORKSPACE` file, you can add the following dependency to any `cc_library()` or
`cc_binary()` definitions in your project's `BAZEL` files:

```python
cc_binary(
    name = "my_application",
    srcs = ["main.cc"],
    deps = ["@_polaris//:polaris_client"],
)
```

The `polaris_client` target enables TLS support by default, which is strongly recommended. If necessary, you can use the
`polaris_client_no_tls` target to connect without TLS, or specify `--//:polaris_enable_tls=False` to Bazel on the
command line.

> Note that you do not need to manually clone the Polaris repository when including it in your Bazel application. Bazel
will clone it automatically when you build your application.

##### Compiling From Source

In general, it is strongly recommended that you let Bazel import and compile the library using `git_repository()` as
shown above. However, if you would like to compile the example applications, you can run the following command:

```
bazel build -c opt //examples:*
```

The generated example applications will be stored within the Bazel cache directory
(`bazel-bin/examples/<APPLICATION_NAME>`). You may run them directly from the `bazel-bin/` directory if you wish, but
they are more commonly run using the `bazel run` command. For example, to run the
[Simple Polaris Client](#simple-polaris-client-1) example application, run the following:

```bash
bazel run -c opt //examples:simple_polaris_cpp_client -- --polaris_api_key=<POLARIS_API_KEY>
```

See [Simple Polaris Client](#simple-polaris-client-1) for more details.

##### Cross-Compiling With Bazel

The Bazel build flow supports cross-compilation for 32- and 64-bit ARM architectures. To build for either architecture,
specify the `--config` argument to Bazel with one of the following values:
- `armv7hf` - 32-bit ARM v7 (e.g., Raspberry Pi v1/2/Zero)
- `aarch64` - 64-bit ARM (e.g., Raspberry Pi v3+, Nvidia Jetson/Pegasus, Variscite DART-MX8M-MINI)

For example:
```
bazel build --config=aarch64 //examples:simple_polaris_cpp_client
```

#### Building On Mac OS

The example applications can be built on Mac OS using CMake or Bazel and Clang. Assure that you have a C++ toolchain and
the required libraries installed. These can be installed with
[Homebrew](http://osxdaily.com/2018/03/07/how-install-homebrew-mac-os/):
```bash
brew install cmake boost gflags glog
```

> Note: When using CMake, you may need to use a directory other than `build` as there is a `BUILD` file in the root of
> the repo. The default Mac OS filesystem, HFS+, is not case-sensitive.

### Using Polaris C++ Client ###

1. Include `polaris_client.h`:
   ```c++
   #include <point_one/polaris/polaris_client.h>

   using namespace point_one::polaris;
   ```
2. Create a `PolarisClient` object, specifying your assigned API key and a unique ID for this connection instance. These
   will be used for authentication when `Run()` is called below. The authentication process will generate an
   _authentication token_, which will be used to receive corrections data.
   ```c++
   PolarisClient client(api_key, unique_id);
   ```
   - Note that the ID you specify must be unique across all Polaris sessions using your API key. If two connections use
     the same unique ID, they will conflict and will not work correctly.
   - If you already have a valid authentication token that you wish to use, you can skip the authentication step by
     calling `SetAuthToken()`.
3. Provide a callback function, which will be called when new data is received.
   ```c++
   void MyDataHandler(const uint8_t* buffer, size_t size_bytes) {}
   ```
   ```c++
   client.SetRTCMCallback(MyDataHandler);
   ```
4. Set an initial receiver position to start receiving corrections for that location.
   ```c++
   client.SendLLAPosition(&context, 37.773971, -122.430996, -0.02);
   ```
5. Finally, call the `Run()` function, which will block indefinitely and receive data, calling the callback function you
   provided each time data is received.
   ```c++
   client.Run();
   ```
6. When finished, call `Disconnect()` from somewhere in your code to disconnect from Polaris and return from `Run()`.
   ```c++
   client.Disconnect();
   ```

The `Run()` function handles authentication, connecting to Polaris, sending position updates, and receiving data. It is
also responsible for handling error conditions including automatically reconnecting or reauthenticating as needed.

If desired, you can use the `RunAsync()` function to launch `Run()` in a separate thread, returning control to your
function immediately.

### Example Applications ###

#### Simple Polaris Client ####

A small example of establishing a Polaris connection and receiving RTCM corrections data.

To run the application, run the following command:
```
bazel run //examples:simple_polaris_cpp_client -- --polaris_api_key=<POLARIS_API_KEY>
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

#### Generic Serial Receiver Example ####

This example relays incoming corrections data to a receiver over a serial connection. The receiver should be configured
to send NMEA `$GPGGA` position updates over the connection. They will then be forwarded to Polaris to associate the
receiver with an appropriate corrections stream.

##### Bazel
To run the application, run the following command:
```
bazel run //examples:serial_port_example -- --polaris_api_key=<POLARIS_API_KEY> --device=/dev/ttyACM0
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

##### CMake

CMake requires [dependencies](#requirements-1) are installed ahead of time.

On Linux:
```bash
sudo apt install libssl-dev libgflags-dev libgoogle-glog-dev libboost-all-dev
```

```
mkdir build && cd build && cmake .. && make && \
./examples/serial_port_client \
    --polaris_api_key=<POLARIS_API_KEY> \
    --receiver_serial_baud 460800 \
    --receiver_serial_port=/dev/ttyACM0
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

#### NTRIP Server Example ####

Receive incoming Polaris corrections data and relay it as a simple NTRIP caster for use with devices that take NTRIP
corrections. Receiver positions sent to the NTRIP server (via a NMEA `$GPGGA` message) will be forwarded to Polaris to
associate the receiver with an appropriate corrections stream.

For example, to run an NTRIP server on TCP port 2101 (the standard NTRIP port), run the following command:
```
bazel run -c opt examples/ntrip:ntrip_server_example -- --polaris_api_key=<POLARIS_API_KEY> 0.0.0.0 2101 examples/ntrip
```
where `<POLARIS_API_KEY>` is the Polaris key assigned to you by Point One.

Any GNSS receiver that supports an NTRIP connection can then connect to the computer running this application to receive
corrections, connecting to the NTRIP endpoint `/Polaris`. The receiver should be configured to send NMEA `$GPGGA`
messages at a regular cadence (typically 1hz).

Note that the NTRIP server example application is not a full NTRIP server, and only supports a limited set of features.
In particular, it does not support handling multiple connected receivers at a time.

#### Septentrio Receiver Example ####

Similar to the [Generic Serial Receiver Example](#generic-serial-receiver-example), this application relays RTK
corrections to a Septentrio GNSS receiver over a serial connection, and forward positions from the receiver to Polaris.
Positions are translated from Septentrio Binary Format (SBF) as needed.


This has been tested on Septentrio's
[AsteRx-m2](https://www.septentrio.com/en/products/gnss-receivers/rover-base-receivers/oem-receiver-boards/asterx-m2)
receiver. The example can be easily modified for receivers that may receive corrections over other interfaces.

##### Hardware Setup #####

Assure that the receiver is plugged into the computer via USB and that the receiver is connected to an antenna that
supports L1/L2/L5 GPS frequencies.

##### Configure AsteRx-m2 #####

For the example to work the receiver must be configured to stream binary out and to accept RTCMv3 corrections input.
There is an example configuration in `examples/data` that can be applied using Septentrio's tools.

On Linux, the AsteRx-m2 presents itself as a wired ethernet interface. In most network configurations this should be
`192.168.3.1`. The receiver has an HTTP interface on port 80 that can be used to configure the receiver. Use a browser
to visit http://192.168.3.1 to access the interface.

Download your current receiver configuration in the case that you would like to revert your receiver config
(`Current > Download Configuration`). Upload the example configuration `examples/data/AsteRx_m2_config.txt`
(`Current > Upload Configuration`). You can set this to Boot or a User profile by using `the Copy Configuration Pane`.

The import receiver commands in the above example are:
```
setDataInOut, USB1, , SBF
setSBFOutput, Stream1, , MeasEpoch+MeasExtra+EndOfMeas+GPSRawCA+GPSRawL2C+GPSRawL5+GLORawCA+GALRawFNAV+GALRawINAV+GEORawL1+GEORawL5+GPSNav+GPSAlm+GPSIon+GPSUtc+GLONav+GLOAlm+GLOTime+GALNav+GALAlm+GALIon+GALUtc+GALGstGps+GALSARRLM+GEOMT00+GEOPRNMask+GEOFastCorr+GEOIntegrity+GEOFastCorrDegr+GEONav+GEODegrFactors+GEONetworkTime+GEOAlm+GEOIGPMask+GEOLongTermCorr+GEOIonoDelay+GEOServiceLevel+GEOClockEphCovMatrix+BaseVectorGeod+PVTGeodetic+PosCovGeodetic+VelCovGeodetic+ReceiverTime+BDSRaw+QZSRawL1CA+QZSRawL2C+QZSRawL5+PosLocal+IRNSSRaw
```

Specifically, the code requires SBF output of message PVTGeodetic in order to read the position of the receiver.

![Septentrio Configuration](/docs/septentrio_configuration.png?raw=true "Septentrio Configuration")

##### Serial Permissions In Ubuntu #####

The AstrRx-m2 should create two serial interfaces when plugged into your computer via USB. If you have no other serial
interfaces plugged in, these will likely be `/dev/ttyACM0` and `/dev/ttyACM1`.

If you're running into issues, you may not have read/write access to the serial port on your machine. This is a common
issue when using the serial port in Linux. To fix it, run:

```bash
sudo usermod -a -G dialout $USER
```

This will add the current user to the `dialout` group, giving you serial port access (to `/dev/ttyACM0` for instance).
Make sure to log out and login after running this command for it to take effect.

##### Running The Example #####

Build the code using the example above and run the binary.

Using Bazel from repo root directory:
```
bazel run //examples:septentrio_example -- --polaris_api_key=<POLARIS_API_KEY> --device=/dev/ttyACM0
```

Using CMake from repo root directory:
```
./build/septentrio_example --polaris_api_key=<POLARIS_API_KEY> --device=/dev/ttyACM0
```

##### Verifying Corrections #####

You can view the corrections status on the Septentrio HTTP interface http://192.168.3.1. If the corrections are being
received, it should be noted in `Corrections -> Corrections input` tab. After receiving corrections for some time, the
receiver should enter RTK float or RTK fixed modes.

![Septentrio Rtk Status](/docs/septentrio_rtk_fix.png?raw=true "Septentrio Rtk Status")

`Corrections -> Corrections input`

![Septentrio Corrections Status](/docs/septentrio_corrections.png?raw=true "Septentrio Corrections")

You may also use Septentrio's `RxTools` application, which is distributed as part as the Septentrio receiver software
utilities, to view the receiver status. This can be accomplished by connecting RxTools to the second serial port
`/dev/ttyACM1` and viewing the differential corrections.

`RxTools > View > DiffCorr Info View`

![RxTools > View > DiffCorr Info View](/docs/septentrio_rxtools_example.png?raw=true "Septentrio Rxtools example")
