# Point One Navigation Polaris Service #

This repo provides a library for interacting with the Point One Navigation Polaris Service to receive GNSS RTK corrections. Compatible GNSS receivers can acheive better than 10cm accuracy when used with the service. Supported receivers are currently available from Septentrio and u-Blox. We also offer a hardware SDK you can request by emailing [info@pointonenav.com](mailto:info@pointonenav.com)

Documentation on the protocol used by the Polaris Service can be found at https://pointonenav.github.io/docs.

* [Usage](#usage)
    * [Polaris API Key](#polaris-api-key)
    * [Dependencies](#dependencies)
    * [Building with CMake](#building-with-cmake)
    * [Building With Bazel](#building-with-bazel)
    * [Building On Mac](#building-on-mac)
* [Examples](#examples)
    * [Septentrio Example](#septentrio-example)
        * [Hardware Setup](#hardware-setup)
        * [Configure AsteRx-m2](#configure-asterx-m2)
        * [Serial Permissions in Ubuntu](#serial-permissions-in-ubuntu)
        * [Running the example](#running-the-example)
        * [Verifying Corrections](#verifying-corrections)



## Usage ##

The Polaris Client can be built by Bazel or CMake. Follow instructions below for build system of your choice.

### Polaris API Key ###

To establish a connection, you must provide a valid Polaris API key. Please contact the administrator of your Point One Navigation contract or sales@pointonenav.com if you do not have one.

### Dependencies ###

The example code uses the [Boost.ASIO](http://www.boost.org/libs/asio) library for both TCP and serial connections. Boost is quite a large package and may already be installed on your machine. If not, it can be built from source using the following [instructions](https://www.boost.org/doc/libs/1_58_0/more/getting_started/unix-variants.html), or can be installed using your system package manager. For example:

```
sudo apt-get install libboost-all-dev
```

The sample application was tested against Boost 1.58.0 but other versions may work.

The sample code also uses [gflags](https://gflags.github.io/gflags/) command-line argument support and [glog](https://github.com/google/glog/blob/master/cmake/INSTALL.md) for logging support. If you are using Bazel, these will download automatically. If you are using CMake, you must install them using your system's package manager or build them from source.

### Building with CMake ###

To build using CMake install the dependencies if you do not have them already.

```
sudo apt-get install build-essential cmake libgflags-dev libgoogle-glog-dev
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

[Bazel](https://docs.bazel.build/versions/master/install.html "Install Bazel") is a build system developed by Google. It provides an alternative to make based build systems. 


To build and run the example using Bazel, from the root of the repo:
```
bazel run examples:septentrio_example -- --help
```

### Building On Mac ###

The example can be built on mac using CMake or Bazel and Clang. Assure that you have a c++ toolchain and the associated dependencies.

These can be installed with [homebrew](http://osxdaily.com/2018/03/07/how-install-homebrew-mac-os/):
```brew install cmake boost gflags glog```

When using CMake, you will need to use a directory other than `build` as there is a BUILD file in the root of the repo and the likely disk formatting of your Mac is HFS+ which does not include letter case when determining file uniqueness.

## Examples ##

### Septentrio Example ###

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
