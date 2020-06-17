[![Total alerts](https://img.shields.io/lgtm/alerts/g/PointOneNav/polaris.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PointOneNav/polaris/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/PointOneNav/polaris.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/PointOneNav/polaris/context:cpp)


# Point One Navigation Polaris Service #

This repo provides a library for interacting with the Point One Navigation Polaris Service to receive GNSS RTK corrections. Compatible GNSS receivers can acheive better than 10cm accuracy when used with the service. Supported receivers are currently available from Septentrio and u-Blox. We also offer a hardware SDK you can request by emailing [info@pointonenav.com](mailto:info@pointonenav.com)

Documentation on the protocol used by the Polaris Service can be found at https://pointonenav.github.io/docs.

* [Usage](#usage)
    * [Polaris API Key](#polaris-api-key)
    * [Dependencies](#dependencies)
    * [Building with CMake](#building-with-cmake)
    * [Building With Bazel](#building-with-bazel)
    * [Building On Mac](#building-on-mac)
* [Example Applications](#example-applications)
    * [NTRIP Proxy Example](#ntrip-proxy-example)
    * [Septentrio Example](#septentrio-example)
    *   [Hardware Setup](#hardware-setup)
    *   [Configure AsteRx-m2](#configure-asterx-m2)
    *   [Serial Permissions in Ubuntu](#serial-permissions-in-ubuntu)
    *   [Running the example](#running-the-example)
    *   [Verifying Corrections](#verifying-corrections)
    * [Generic Serial Receiver Example](#generic-serial-receiver-example)




## Usage ##

The Polaris Client can be built by Bazel or CMake. Follow instructions below for build system of your choice.

### Polaris API Key ###

To establish a connection, you must provide a valid Polaris API key. Please contact the administrator of your Point One Navigation contract or sales@pointonenav.com if you do not have one.


### Building with CMake ###

To build using CMake, install the dependencies if you do not have them already.

```
sudo apt-get install libboost-all-dev build-essential cmake libgflags-dev libgoogle-glog-dev
```

From the root of the repo execute:
```
mkdir build && cd build
cmake ..
make
```

This will produce a binary `septentrio_example` which can be executed.
```
# Get list of arguments
./septentrio_example --help
```

### Building With Bazel ###

All dependencies are automatically built when using [Bazel](https://docs.bazel.build/versions/master/install.html "Install Bazel").

To build and run the example using Bazel, from the root of the repo:
```
bazel run examples:septentrio_example -- --help
```

#### Cross-Compiling With Bazel ####

The Bazel build flow supports cross-compilation for 32- and 64-bit ARM architectures. To build for either architecture,
specify the `--config` argument to Bazel with one of the following values:
- `armv7hf` - 32-bit ARM v7
- `aarch64` - 64-bit ARM

For example:
```
bazel build --config=aarch64 examples:septentrio_example
```

### Building On Mac ###

The example can be built on mac using CMake or Bazel and Clang. Assure that you have a c++ toolchain and the associated dependencies.

These can be installed with [homebrew](http://osxdaily.com/2018/03/07/how-install-homebrew-mac-os/):
```brew install cmake boost gflags glog```

When using CMake, you will need to use a directory other than `build` as there is a BUILD file in the root of the repo and the likely disk formatting of your Mac is HFS+ which does not include letter case when determining file uniqueness.

## Example Applications ##

### NTRIP Proxy Example ###

The example binary `ntrip_example` proxies Polaris RTK corrections as a simple NTRIP caster for use with devices that take NTRIP corrections.

Any receiver that uses a NTRIP connection can simply connect to the computer running this application on port 2101 using the following command:
```
bazel run -c opt examples/ntrip:ntrip_example -- --polaris_api_key=${MY_POLARIS_APP_KEY} 0.0.0.0 2101 examples/ntrip
```

Your receiver should be configured to send NMEA GGA messages at a regaulr cadence (usually 1hz) so it is properly receiving corrections for the right region. The default mount point for the examples is `/Polaris` and no username or password is required.

The examples directory also contains a simple `ntrip-proxy.service` that can be used to turn the example into a standard Linux service.

### Septentrio Receiver Example ###

The example binary `septentrio_example` streams RTK corrections to a Septentrio receiver over serial and reports back the receivers PVT message to the console. This has been tested on Septentrio's [AsteRx-m2](https://www.septentrio.com/en/products/gnss-receivers/rover-base-receivers/oem-receiver-boards/asterx-m2). The example can be easily modified for receivers that may receive corrections over other interfaces.

#### Hardware Setup ####

Assure that the receiver is plugged into the computer via USB and that the receiver is connected to an antenna that supports L1/L2 GPS frequencies.

#### Configure AsteRx-m2 ####

For the example to work the receiver must be configured to stream binary out and to accept RTCMv3 corrections input. There is an example configuration in `examples/data` that can be applied using Septentrio's tools.

On Linux, the AsteRx-m2 presents itself as a wired ethernet interface. In most network configurations this should be 192.168.3.1. The receiver has an HTTP interface on port 80 that can be used to configure the receiver. Use a browser to visit http://192.168.3.1 to access the interface.

Download your current receiver configuration in the case that you would like to revert your receiver config. `Current > Download Configuration`
Upload the example configuration `examples/data/AsteRx_m2_config.txt` `Current > Upload Configuration`
You can set this to Boot or a User profile by using `the Copy Configuration Pane`

The import receiver commands in the above example are:
```
setDataInOut, USB1, , SBF
setSBFOutput, Stream1, , MeasEpoch+MeasExtra+EndOfMeas+GPSRawCA+GPSRawL2C+GPSRawL5+GLORawCA+GALRawFNAV+GALRawINAV+GEORawL1+GEORawL5+GPSNav+GPSAlm+GPSIon+GPSUtc+GLONav+GLOAlm+GLOTime+GALNav+GALAlm+GALIon+GALUtc+GALGstGps+GALSARRLM+GEOMT00+GEOPRNMask+GEOFastCorr+GEOIntegrity+GEOFastCorrDegr+GEONav+GEODegrFactors+GEONetworkTime+GEOAlm+GEOIGPMask+GEOLongTermCorr+GEOIonoDelay+GEOServiceLevel+GEOClockEphCovMatrix+BaseVectorGeod+PVTGeodetic+PosCovGeodetic+VelCovGeodetic+ReceiverTime+BDSRaw+QZSRawL1CA+QZSRawL2C+QZSRawL5+PosLocal+IRNSSRaw
```
Specifically, the code requires SBF output of message PVTGeodetic in order to read the position of the receiver.

![Septentrio Configuration](/docs/septentrio_configuration.png?raw=true "Septentrio Configuration")

#### Serial Permissions in Ubuntu ####

The AstrRx-m2 should create two serial interfaces when plugged into your computer via USB. If you have no other serial interfaces plugged in, these will
likely be /dev/ttyACM0 and /dev/ttyACM1.

If you're running into issues, you may not have read/write access to the serial port on your machine. This is a common issue when using the serial port in Linux. To fix it, run:

```
sudo usermod -a -G dialout $USER
```

This will add the current user to the dialout group, giving you serial port access (to /dev/ttyACM0 for instance). Make sure to log out and login after running this command for it to take effect.

#### Running the example ####

Build the code using the example above and run the binary.

Using Bazel from repo root directory:
```
bazel run examples:septentrio_example -- --logtostderr --polaris_api_key=MYAPPKEY1234 --device=/dev/ttyACM0
```

Using CMake from repo root directory:
```
./build/septentrio_example --logtostderr --polaris_api_key=MYAPPKEY1234 --device=/dev/ttyACM0
```

The executable will authenticate using `--polaris_api_key` and forward corrections to serial port `--device`. The results will be logged to the console (`--logtostderr`)


#### Verifying Corrections ####

You can view the corrections status on the septentrio HTTP interface http://192.168.3.1. If the corrections are being received, it should be noted in `Corrections -> Corrections input` tab. After receiving corrections for some time, the receiver should enter RTK float or RTK fixed modes.

![Septentrio Rtk Status](/docs/septentrio_rtk_fix.png?raw=true "Septentrio Rtk Status")

`Corrections -> Corrections input`

![Septentrio Corrections Status](/docs/septentrio_corrections.png?raw=true "Septentrio Corrections")

You may also use RxTools, which is distributed as part as the septentrio receiver software utilities to view the receiver status. This can be accomplished by connecting RxTools to the second serial port /dev/ttyACM1 and viewing the differential corrections.

`RxTools > View > DiffCorr Info View`

![RxTools > View > DiffCorr Info View](/docs/septentrio_rxtools_example.png?raw=true "Septentrio Rxtools example")

### Generic Serial Receiver Example ###

The example binary `asio_example` can be used to connect to a receiver that can take rtcm corrections over serial.  If the receiver is configured to send ascii nmea GPGGA strings, the example will update the polaris server to assure the best rtk baseline possible.

To run the application:
```bazel run examples:asio -- --logtostderr --polaris_api_key=MYAPPKEY1234 --device=/dev/ttyACM0```


