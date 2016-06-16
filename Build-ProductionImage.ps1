[CmdletBinding()]
Param(
    [Parameter(Position=1)]$IARRoot = "${env:ProgramFiles(x86)}\IAR Systems\Embedded Workbench 7.3"
)

# NOTE: Also requires the "strings" tool from sysinternals on your path
# choco install -y sysinternals is an easy way to make that happen.

$PSScriptRoot = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

# Build with IAR
Start-Process (Join-Path $IARRoot "common\bin\IarBuild.exe") -ArgumentList @('Project.ewp', '-build', 'Production') -WorkingDirectory $PSScriptRoot -NoNewWindow -Wait

# Generate the config file by grepping for the build string in the elf file.
# Couldn't find any better way to extract this from the object files or final elf file...
$BuildString = (strings -q (Join-Path $PSScriptRoot 'Production\Exe\Project.out') | Select-String -Pattern '\[Sensics')
Out-File (Join-Path $PSScriptRoot 'Production\Exe\Config.txt') -InputObject $BuildString

Write-Host 'Completed build identified as:'
Write-Host $BuildString
Write-Host "See the .hex and Config.txt files in $(Join-Path $PSScriptRoot 'Production\Exe')"
