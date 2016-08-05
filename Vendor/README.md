# Vendor Code

To build the IR LED firmware for the STM8S003K3 microcontroller, you will need to download the STM8 standard peripheral library from ST and extract some of the files to this directory.
(The `.gitignore` file has been set to ignore them, so you don't have to worry about accidentally committing them or deleting them with `git clean` unless you use the `-x` option.)

Do not commit these libraries to the codebase or copy code from them or from other files under the same "MCD-ST Liberty SW License Agreement V2" license.

We've used the 2.2.0 release of the libraries from here: <http://www.st.com/web/en/catalog/tools/FM147/CL1794/SC1807/SS1754/PF258009>

1. You'll download a file called `en.stsw-stm8069.zip`. Extract it.
2. Move the `STM8S-StdPeriph_Driver` directory (from inside the `Libraries` directory extracted from the zip) into this `Vendor` directory.
3. Move the `STM8S_StdPeriph_Template` directory (from inside the `Project` directory extracted from the zip) into this `Vendor` directory. (We actually only need the two header files in that directory, but it's easiest to just grab the whole directory.)