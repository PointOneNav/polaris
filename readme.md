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

* [Polaris C Client](#polaris-c-client)
  * [Requirements](#requirements)
  * [Building From Source](#building-from-source)
    * [Bazel](#bazel)
      * [Compiling Example Applications With Bazel](#compiling-example-applications-with-bazel)
    * [CMake](#cmake)
    * [GNU Make](#gnu-make)
  * [Using Polaris C Client](#using-polaris-c-client)
* [Polaris C++ Client](#polaris-c-client-1)
  * [Requirements](#requirements-1)
  * [Building From Source](#building-from-source-1)
    * [Bazel](#bazel-1)
      * [Compiling Example Applications With Bazel](#compiling-example-applications-with-bazel-1)
      * [Cross-Compiling With Bazel](#cross-compiling-with-bazel)
    * [CMake](#cmake-1)
    * [Building On Mac OS](#building-on-mac-os)
  * [Using Polaris C++ Client](#using-polaris-c-client-1)
  * [Example Applications](#example-applications)
    * [Simple Polaris Client](#simple-polaris-client)
    * [Generic Serial Receiver Example](#generic-serial-receiver-example)
    * [NTRIP Server Example](#ntrip-server-example)
    * [Septentrio Example](#septentrio-example)
      * [Hardware Setup](#hardware-setup)
      * [Configure AsteRx-m2](#configure-asterx-m2)
      * [Serial Permissions in Ubuntu](#serial-permissions-in-ubuntu)
      * [Running the example](#running-the-example)
      * [Verifying Corrections](#verifying-corrections)

### Polaris API Key ###

To establish a connection, you must provide a valid Polaris API key. Please contact the administrator of your Point One
Navigation contract or sales@pointonenav.com if you do not have one.

## Polaris C Client ##

### Requirements ###

- [Bazel](https://bazel.build/) 3.3+, or [CMake](https://cmake.org/) 3.3+ and
  [GNU Make](https://www.gnu.org/software/make/)
- [OpenSSL](https://www.openssl.org/) or [BoringSSL](https://boringssl.googlesource.com/boringssl/) (optional; required
  for TLS support (recommended))

### Building From Source ###

The Polaris C Client can be built with [Bazel](https://bazel.build/), [CMake](https://cmake.org/), or
[GNU Make](https://www.gnu.org/software/make/). Follow the instructions below for build system of your choice.

#### Bazel ####

To use the Polaris C Client within a Bazel project, add the following to your `WORKSPACE` file:

```bazel
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "p1_polaris",
    strip_prefix = "polaris-1.0.0",
    urls = ["https://github.com/PointOneNav/polaris/archive/v1.0.0.tar.gz"],
)
```

Then you can add `@p1_polaris//c:polaris_client` to the `deps` section of a `cc_binary()` or `cc_library()` rule in your
project, and add `#include <point_one/polaris/polaris.h>` to your source code. For example:

```bazel
cc_binary(
    name = "my_application",
    srcs = ["main.c"],
    deps = ["@p1_polaris//c:polaris_client"],
)
```

> Note that you do not need to clone the Polaris repository when using Bazel. Bazel will clone it automatically when you
build your application.

##### Compiling Example Applications With Bazel ####

To compile and run the included example applications using Bazel, follow these steps:

1. Clone the Polaris source code and navigate to the `c/` source directory:
   ```bash
   git clone https://github.com/PointOneNav/polaris.git
   cd polaris
   ```
2. Compile the Polaris source code and example applications:
   ```bash
   bazel build -c opt //c/examples:*
   ```

#### CMake ####

> Note: The C client does not currently have any external dependencies.

1. Clone the Polaris source code and navigate to the `c/` source directory:
   ```bash
   git clone https://github.com/PointOneNav/polaris.git
   cd polaris/c
   ```
2. Create a `build/` directory and run CMake to configure the build tree:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
3. Compile the Polaris source code and example applications:
   ```bash
   make
   ```

The generated example applications will be located in `build/examples/<APPLICATION NAME>`.

#### GNU Make ###

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

The generated example applications will be located in `examples/<APPLICATION NAME>`.

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

If desired, you can use the `Polaris_Work()` function instead of `Polaris_Run()` to perform non-blocking data receive
operations.

## Polaris C++ Client ##

### Requirements ###

- [Bazel](https://bazel.build/) 3.3+, or [CMake](https://cmake.org/) 3.3+ and
  [GNU Make](https://www.gnu.org/software/make/)
- [Google gflags 2.2.2+](https://github.com/gflags/gflags)
- [Google glog 0.4.0+](https://github.com/google/glog)
- [Boost 1.58+](https://www.boost.org/) (for building example applications only)
- [OpenSSL](https://www.openssl.org/) or [BoringSSL](https://boringssl.googlesource.com/boringssl/) (optional; required
  for TLS support (recommended))

### Building From Source ###

The Polaris C++ Client can be built with [Bazel](https://bazel.build/), or [CMake](https://cmake.org/). Follow the
instructions below for build system of your choice.

#### Bazel ####

To use the Polaris C++ Client within a Bazel project, add the following to your `WORKSPACE` file:

```bazel
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "p1_polaris",
    strip_prefix = "polaris-1.0.0",
    urls = ["https://github.com/PointOneNav/polaris/archive/v1.0.0.tar.gz"],
)

load("@p1_polaris//bazel:repositories.bzl", polaris_dependencies = "dependencies")

polaris_dependencies()
```

This will automatically download and import all requirements for the Polaris C++ Client.

Then you can add `@p1_polaris//:polaris_client` to the `deps` section of a `cc_binary()` or `cc_library()` rule in your
project, and add `#include <point_one/polaris/polaris_client.h>` to your source code. For example:

```bazel
cc_binary(
    name = "my_application",
    srcs = ["main.cc"],
    deps = ["@p1_polaris//:polaris_client"],
)
```

> Note that you do not need to clone the Polaris repository when using Bazel. Bazel will clone it automatically when you
build your application.

##### Compiling Example Applications With Bazel ####

To compile and run the included example applications using Bazel, follow these steps:

1. Clone the Polaris source code:
   ```bash
   git clone https://github.com/PointOneNav/polaris.git
   cd polaris
   ```
2. Compile the Polaris source code and example applications:
   ```bash
   bazel build -c opt //examples:*
   ```

##### Cross-Compiling With Bazel #####

The Bazel build flow supports cross-compilation for 32- and 64-bit ARM architectures. To build for either architecture,
specify the `--config` argument to Bazel with one of the following values:
- `armv7hf` - 32-bit ARM v7 (e.g., Raspberry Pi v1/2/Zero)
- `aarch64` - 64-bit ARM (e.g., Raspberry Pi v3+, Nvidia Jetson/Pegasus, Variscite DART-MX8M-MINI)

For example:
```
bazel build --config=aarch64 //examples:simple_polaris_client
```

#### CMake ####

1. Install all required libraries:
   ```bash
   sudo apt install libgflags-dev libgoogle-glog-dev libboost-all-dev
   ```
2. Clone the Polaris source code:
   ```bash
   git clone https://github.com/PointOneNav/polaris.git
   cd polaris
   ```
3. Create a `build/` directory and run CMake to configure the build tree:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
4. Compile the Polaris source code and example applications:
   ```bash
   make
   ```

The generated example applications will be located in `build/examples/<APPLICATION NAME>`.

#### Building On Mac OS ####

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

#### Generic Serial Receiver Example ####

This example relays incoming corrections data to a receiver over a serial connection. The receiver should be configured
to send NMEA `$GPGGA` position updates over the connection. They will then be forwarded to Polaris to associate the
receiver with an appropriate corrections stream.

To run the application, run the following command:
```
bazel run //examples:serial_port_example -- --polaris_api_key=<POLARIS_API_KEY> --device=/dev/ttyACM0
```

#### NTRIP Server Example ####

Receive incoming Polaris corrections data and relay it as a simple NTRIP caster for use with devices that take NTRIP
corrections. Receiver positions sent to the NTRIP server (via a NMEA `$GPGGA` message) will be forwarded to Polaris to
associate the receiver with an appropriate corrections stream.

For example, to run an NTRIP server on TCP port 2101 (the standard NTRIP port), run the following command:
```
bazel run -c opt examples/ntrip:ntrip_example -- --polaris_api_key=<POLARIS_API_KEY> 0.0.0.0 2101 examples/ntrip
```

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
