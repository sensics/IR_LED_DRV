[CmdletBinding()]
Param(
    [Parameter(Position=1, Mandatory=$True)]$HexFile,
    $STVPDir = "${env:ProgramFiles(x86)}\STMicroelectronics\st_toolset\stvp",
    $Tool = "ST-LINK"
)

# Requires "ST Visual Programmer" installation, uses the default install location but can be overridden with -STVPDir argument.
# Download from http://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stvp-stm32.html

# Similarly, it requires a programming tool for the STM8: defaults to the ST-LINK family (including the ST-LINK v2 dongle), but that too can be overridden.
# Pass the combined (flash and eeprom) hex file as the first or -HexFile argument. Ignore the warnings about address out of range: that's from passing the same hex
# as both the flash file and the eeprom file.

$STVPApp = Join-Path $STVPDir "STVP_CmdLine.exe"

$STVPArgs = @(
    "-BoardName=$Tool"
    '-Device=STM8S003K3',
    '-log',
    '-verif',
    "-FileProg=""$HexFile""",
    "-FileData=""$HexFile""")
Write-Host "Arguments to STVP_CmdLine: $STVPArgs"

Write-Host "Press space to exit the programming loop."


Start-Process $STVPApp -ArgumentList $STVPArgs -NoNewWindow -Wait
cls