# Change Log

## [v.2.0-rc10](https://github.com/goriy/sif/releases/tag/v.2.0-rc10) (Not released yet)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc9...v.2.0-rc10)

**Improvements:**

**Fixed bugs:**

**Internal:**

## [v.2.0-rc9](https://github.com/goriy/sif/releases/tag/v.2.0-rc9) (2018-09-11)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc8...v.2.0-rc9)

**Fixed bugs:**

* line numbers are updated as one SetWindowText call to avoid
  annoying audible 'bells'

## [v.2.0-rc8](https://github.com/goriy/sif/releases/tag/v.2.0-rc8) (2017-06-14)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc7...v.2.0-rc8)

**Fixed bugs:**

* unsupported yet checkbox "Re" (for regex search) is in disabled state

**Internal:**

* fixed some chocolatey package Guidelines violations

## [v.2.0-rc7](https://github.com/goriy/sif/releases/tag/v.2.0-rc7) (2017-06-11)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc6...v.2.0-rc7)

**Improvements:**

* added line numbers to file preview box


## [v.2.0-rc6](https://github.com/goriy/sif/releases/tag/v.2.0-rc6) (2017-03-23)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc5...v.2.0-rc6)

**Improvements:**

* skipped too frequent file preview refresh/redraw when choosing search results.
  Too frequent file preview redraws (on every event from search results,
  for example) led to undesired flicker (especially on slow PC).
* do not preview files during search procedure.
* added overall searched bytes counter to status bar.

**Internal:**

* cross-compilation support (can be compiled by mingw under Linux)


## [v.2.0-rc5](https://github.com/goriy/sif/releases/tag/v.2.0-rc5) (2016-12-05)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc4...v.2.0-rc5)

**Fixed bugs:**

* file loaded for preview was not reloaded during next search. That next search
  results were empty if search was case insensitive (cached data was not properly
  prepared for search).

## [v.2.0-rc4](https://github.com/goriy/sif/releases/tag/v.2.0-rc4) (2016-12-04)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc3...v.2.0-rc4)

**Fixed bugs:**

* program was unable to find anything in non-empty last line of file

## [v.2.0-rc3](https://github.com/goriy/sif/releases/tag/v.2.0-rc3) (2016-03-01)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc2...v.2.0-rc3)

**Improvements:**

* main window dimensions are stored in configuration file
* minimal possible window width and height are decreased (to use on low resolution displays)
* added support for multiple file masks (comma or semicolon separated)

## [v.2.0-rc2](https://github.com/goriy/sif/releases/tag/v.2.0-rc2) (2015-10-16)

[Full Changelog](https://github.com/goriy/sif/compare/v.2.0-rc1...v.2.0-rc2)

**Fixed bugs:**

* wrong length of file to save
* small UI bugs

## [v.2.0-rc1](https://github.com/goriy/sif/releases/tag/v.2.0-rc1) (2015-10-05)

* initial release
