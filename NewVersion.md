# New version release

- [ ] Check version in `include/version.h`
- [ ] Check version changes info in `CHANGELOG.md` 
- [ ] Check year in `LICENSE` file 
- [ ] Create and push repository tag
- [ ] Delete `bin` directory
- [ ] Build release (call `create_bin_release.bat`)
- [ ] Create release on https://github.com/goriy/sif/releases
      and upload both zip files from `bin` directory
- [ ] Edit version and year in 

  * `pkg-src/embedded/sif.nuspec` 
  * `pkg-src/downloadable/sif.nuspec`

- [ ] Get sha-256 checksums from `bin/sif2.sha256`, convert to uppercase and
      put them into files:
  
  * `pkg-src/embedded/tools/chocolateyinstall.ps1` 
  * `pkg-src/embedded/tools/VERIFICATION.txt` 
  * `pkg-src/downloadable/tools/chocolateyinstall.ps1` 
  * `pkg-src/downloadable/tools/VERIFICATION.txt` 

- [ ] Check year in

  * `pkg-src/embedded/tools/LICENSE.txt` 
  * `pkg-src/downloadable/tools/LICENSE.txt` 

- [ ] Change dir to `/pkg-src/embedded` and call `choco pack`
- [ ] Change dir to `/pkg-src/downloadable` and call `choco pack`

- [ ] Login to https://chocolatey.org and upload file `/pkg-src/downloadable/__<<filename>>__.nupkg`
      to https://chocolatey.org/packages/upload
