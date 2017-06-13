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
  checksum      = 'CDD37A2D50792014ECD4B655D9762008CC208413E0ED1FBA5C82E55C04618156'
  checksumType  = 'sha256'
  checksum64    = 'F8FE76D9EB4440C0B450D6616DE14944466B2CEEF7CA05237679E8131DE317E3'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
