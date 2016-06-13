# Vendor Code

To build the IR LED firmware for the STM8S003K3 microcontroller, you will need to download the STM8 standard peripheral library from ST and extract some of the files to this directory.
(The `.gitignore` file has been set to ignore them, so you don't have to worry about accidentally committing them or deleting them with `git clean` unless you use the `-x` option.)

Do not commit these libraries to the codebase or copy code from them or from other files under the same "MCD-ST Liberty SW License Agreement V2" license.

We've used the 2.2.0 release of the libraries from here: <http://www.st.com/web/en/catalog/tools/FM147/CL1794/SC1807/SS1754/PF258009>

1. You'll download a file called `en.stsw-stm8069.zip`. Extract it.
2. Move the `STM8S-StdPeriph_Driver` directory (from inside the `Libraries` directory extracted from the zip) into this `Vendor` directory.
3. Move the `STM8S_StdPeriph_Template` directory (from inside the `Project` directory extracted from the zip) into this `Vendor` directory. (We actually only need the two header files in that directory, but it's easiest to just grab the whole directory.)

## Note to Cosmic compiler users
If you're using the Cosmic compiler, you'll need to use the `Makefile` in this directory to pre-build these sources into a library.

- A typical GNU Make will work fine - if you're on Windows, try using [Chocolatey](http://chocolatey.org) and run `choco install -y make` to get a system-wide install (of 3.81, as of this writing).
- Or, at least my copy of ST Visual Develop came with a `gmake` (GNU Make) binary in `"C:\Program Files (x86)\STMicroelectronics\st_toolset\stvd\gmake.exe"` (version 3.78.1 - older, but still works with this makefile) - you can use that, just replacing the word `make` below with the full path to `gmake`.

Then, just run `make`/`gmake` here (passing alternate, double-quoted paths to `cxstm8.exe` as CC= and `clib.exe` as LIB= on the command line if the defaults in the `Makefile` don't match your system.)

It will spit out a `StdPeriph_Driver0.sm8` file in this directory, in addition to the intermediate files in `Release` that can be ignored (and that are gitignored).

