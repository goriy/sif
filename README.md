Search In Files
===============

This small tool is intended to search text in multiple files.
Search results are not only file name, line and column numbers,
but additionally the whole line from file.

Click on search result loads whole file into edit control with highlight of found text.
Text could be edited and saved right there.

This tool understands both cp1251 and utf-8 text encodings and both CRLF and LF line endings.
It supports command line argument and path/file drag'n'drop.
It automatically creates shortcut in SendTo folder to simplify its usage from windows explorer.

It works on Windows platform (XP, Vista, 7).
Also it should work on Windows 2000 and Windows 8, but such case has not been tested yet.

This tool is written in C programming language with no"bold" framework (MFC, .NET, ...) involved.
Raw WinAPI only.
Due to this it has small size, it has no complex dependencies and thus it can work
without any installation procedures (other than simple exe-file copy).
