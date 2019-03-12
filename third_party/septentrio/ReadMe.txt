
                           "sbf2asc" Program

                           -----------------

 Introduction
 ------------
 "sbf2asc" is a simple command-line program that converts the SBF data
 produced by Septentrio's GNSS receivers, into a column-based plain
 text file. "sbf2asc" and its sources are delivered by Septentrio as
 a help for the users of its GNSS receivers. It contains an "sbfdef.h"
 C header file, with structure definitions to ease the reading of SBF
 messages.

 "sbf2asc" is given "as is", without warranty.

 Compiling
 ---------
 Recompiling "sbf2asc" requires:
   1. A C compiler and library
   2. Preferably, a "make" utility

 "sbf2asc" has been successfully compiled and tested on
 the following systems, all with x86 processors:

   Microsoft Visual C++ Toolkit 2003 on Windows2000

   Microsoft Visual C++ 2005 Express Edition on Windows XP

   MinGW on Windows2000  (MSYS v1.0.10, w32api v3.1, mingw-runtime v3.5,
                         gcc v3.4.2, binutils v2.15.91-20040904-1)

   Cygwin on Windows2000 (v1.5.5-1, gcc v3.3.1, ld v2.14.90,
                         bash v2.05b.0(1), make v3.80)

   Linux Red Hat 6.2     (Linux v2.2.14-5.0, gcc v.egcs-2.91.66, ld v2.9.5,
                         bash v2.03.8(1), make v3.78.1, glibc v2.1.3-15)

   Linux Fedora Core 2   (Linux v2.6.9-1.6_FC2, gcc v3.3.3, ld v2.15.90.0.3,
                         bash v2.05b.0(1), make v3.80, glibc v2.3.3-27.1)

   Linux Fedora 7        (Linux v2.6.22.9-91.fc7, gcc v4.1.2, ld v2.17.50.0.12-4,
                         bash v3.2.9(1)-release, GNU make v3.81, glibc v2.6-4)

 To recompile "sbf2asc" with "make":

   1. Copy the "sbf2asc/" directory and its content from the CD-Rom on a
      local disc.
   2. "cd sbf2asc" and compile with the "make" command.

 To compile "sbf2asc" with Microsoft Visual C++ Toolkit 2003:

   1. Open the C++ Toolkit command window.
   2. "cd sbf2asc" and compile with the "build.bat" command.
 

 To compile "sbf2asc" with Microsoft Visual C++ 6.0:

   1. Open Visual C++ and create a project of type "Win32 Console
      Application". Select an empty project, without any default
      sources.

   2. Add the following C sources to the project:
      "sbf2asc.c", "sbfread.c", "ssngetop.c", "crc.c"

   3. Make sure to compile with structures aligned on 4-bytes boundary:
      3.1 Open Projects -> Settings
      3.2 Select tab "C/C++"
      3.3 Select category: "Code generation"
      3.4 Select "Struct member alignment": 4 byte

   4. Build the project.

   5. Use the executable in a "cmd.exe" windows, (a.k.a. a "DOS box")

 To compile with other compilers, make sure the compiler does not align
 "double"s on addresses multiple of 8 ("8 bytes alignment") but on
 addresses multiple of 4.

 Usage
 -----
 Type "sbf2asc" without arguments to see a short summary
 of options and arguments.

 Known Limitations
 -----------------
 "sbf2asc" has been written as an example and preferred readability to
 portability. It is not highly portable and will fail to parse an SBF
 file if compiled for big-endian processors, for instance, or for
 processors with different alignment requirements. They have been
 tested on 32-bit x86 processors only.

 References
 ----------
 Contact:   Septentrio NV        Tel:    +32.16.300.800
            Greenhill Campus     Fax:    +32.16.22.16.40
            Interleuvenlaan 15i
            3001 Leuven          e-mail: support@septentrio.com
            Belgium              Web:    http://www.septentrio.com

 (c) Copyright 2002-2015 Septentrio NV/SA. All rights reserved.
