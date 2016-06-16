[CmdletBinding()]
Param(
    [Parameter(Position=1, Mandatory=$True)]$HexFile,
    $STVPDir = "${env:ProgramFiles(x86)}\STMicroelectronics\st_toolset\stvp",
    $Tool = "ST-LINK"
)

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