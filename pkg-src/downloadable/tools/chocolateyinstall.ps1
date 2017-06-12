$ErrorActionPreference = 'Stop';

$packageName = 'sif'
$toolsDir    = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url         = 'https://github.com/goriy/sif/releases/download/v.2.0-rc7/sif2.zip'
$url64       = 'https://github.com/goriy/sif/releases/download/v.2.0-rc7/sif2_x64.zip'

$packageArgs = @{
  packageName   = $packageName
  unzipLocation = $toolsDir
  url           = $url
  url64bit      = $url64
  checksum      = 'E563A26417455102D5C71F0052D64DAC035AD668795FA329285703D04F53196E'
  checksumType  = 'sha256'
  checksum64    = 'B77EB86D8CBEA924463CDDD953773302C21AAAACD0E374BCA6577F24B5F6BD0B'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs

