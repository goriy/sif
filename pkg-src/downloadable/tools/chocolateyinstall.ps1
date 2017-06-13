$ErrorActionPreference = 'Stop';

$packageName = 'sif'
$toolsDir    = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url         = 'https://github.com/goriy/sif/releases/download/v.2.0-rc8/sif2.zip'
$url64       = 'https://github.com/goriy/sif/releases/download/v.2.0-rc8/sif2_x64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  checksum      = 'CDD37A2D50792014ECD4B655D9762008CC208413E0ED1FBA5C82E55C04618156'
  checksumType  = 'sha256'
  checksum64    = 'F8FE76D9EB4440C0B450D6616DE14944466B2CEEF7CA05237679E8131DE317E3'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
