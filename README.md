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

It works on Windows platform (XP, Vista, 7). It should work also on Windows 2000 and Windows 8, but it was never tested yet.

This tool is written in C programming language and doesn't use any "bold" framework (MFC, .NET, ...) - only raw Win32API.
Due to this it has small size, no complex dependencies and can work without any installation procedure (other than simple exe-file copy).
