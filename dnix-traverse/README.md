## dnix-traverse
Quick tool to extract all files from a DNIX or ABCenix filesystem. Files are extracted to current working directory.

Compiling: 
c++ dnix-traverse.cpp -o dnix-traverse

usage:
dnix-traverse -d image-file [-s] [-h]

options:
- -d image-file: DNIX/ABCenix disk image file
- -s: use sequential block addressing
- -h: show help


