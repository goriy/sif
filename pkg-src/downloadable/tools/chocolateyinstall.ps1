$ErrorActionPreference = 'Stop';

$packageName = 'sif'
$toolsDir    = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url         = 'https://github.com/goriy/sif/releases/download/v.2.0-rc9/sif2.zip'
$url64       = 'https://github.com/goriy/sif/releases/download/v.2.0-rc9/sif2_x64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  checksum      = 'B202227A57FB9D1F90DF83DCECE041413542D2394F1BE988CEDE7DE38FD38DB9'
  checksumType  = 'sha256'
  checksum64    = 'C49362A67A01C50629C5FB42349FEA8AB9E658DC4C1144E2911C81249D506607'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
