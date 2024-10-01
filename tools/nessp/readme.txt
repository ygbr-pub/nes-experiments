NES Space Checker

This small tool is intended to help you with controlling free ROM space 
in NES development, if you don't provided with this information by your 
toolchain. The tool loads a NES file and generates a simple visual graph 
that shows how much memory in the PRG and CHR banks is used up. 

For PRG ROMs it first finds out a value that is considered 'empty', 
by counting sequences of repeating values that are at least 8 bytes 
long. If there is some free space filled with the same byte, this works. 
This many not work properly if the free space is filled with random data,
or if  there is not much of free space left. For CHR ROMs it consider
non-zero bytes  as empty, this does not provide accurace information,
but gives the general idea. 

You can open a NES file using Open item of the main menu, or through the 
command line, or with drag and drop of the file to the program window. 

You can enable watching for changes in last opened file, the program 
will update the graph when the file is changed. 

You can save the graph as a PNG file. 

You also can to use this tool to estimate how much of empty space is
in some other file types, just by selecting the file type in file open
dialog to 'binary' or 'any files'. In this case contents of a file
will be just treated like a set of 16K data chunks.



History:

v1.3  22.01.20 - Scrolling for large ROMs, save to PNG, percentage free
v1.2  26.04.18 - Support for other binary files
v1.1  01.11.15 - Empty value selection
v1.02 05.09.14 - Flicker fix
v1.0  30.12.11 - Initial release



mailto:shiru@mail.ru
http://shiru.untergrund.net/