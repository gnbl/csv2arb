Forked from https://github.com/MatCat/csv2arb to compile for Win32 with MinGW.

*No warranty.*


csv2arb
-------
csv2arb converts the <input> text file with one value per line to 16 bit binary data (little endian) written to <output>.
<input> should be normalized (-1.0 ... 1.0)
<output> is padded to 32 kB with 0V Values.
See the [source](src/csv2arb.c) comments for more info.


csv2arb
-------
csv2arb converts the <input> .arb file with 16 bit binary data (little endian) to ASCII float values written to <output>, one value per line.
It detects and removes Point markers in the data.
See the [source](src/arb2csv.c) comments for more info.


Binaries
--------
I compiled binaries with MinGW for win32 and packed them with UPX:
* [csv2arb.exe](bin/csv2arb.exe)
* [arb2csv.exe](bin/arb2csv.exe)


arb file format
---------------
This was tested with a Rigol DS1000Z-S model.
* 32768 bytes (16384 values as little endian 16 bit words)
* 0x0000 .. 0x3FFFF -> -V .. +V
* most significant bit 0x8000 is Point marker
* it seems at least 2 Points (= datum with marker) are required, data interpolation might be tested ?

Sample .arb files
----------------
A number of valid and invalid files, plus a Python sine data generator are in [/data](data/).