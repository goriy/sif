$ErrorActionPreference = 'Stop';

$packageName = 'sif'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$fileLocation   = Join-Path $toolsDir 'sif2.zip'
$fileLocation64 = Join-Path $toolsDir 'sif2_x64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  file          = $fileLocation
  file64        = $fileLocation64
  checksum      = 'E563A26417455102D5C71F0052D64DAC035AD668795FA329285703D04F53196E'
  checksumType  = 'sha256'
  checksum64    = 'B77EB86D8CBEA924463CDDD953773302C21AAAACD0E374BCA6577F24B5F6BD0B'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
