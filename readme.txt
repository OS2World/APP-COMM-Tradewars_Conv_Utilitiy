This program does nothing but read in a TWBFind v0.13 binary database, and
write it out to a v0.14 and later database.

The usual disclaimer applies:  I'm not responsible for anything that
happens before, during, or after running this on your computer.

The usage is simple:

cvt.exe <infile> <outfile>

Contents:

cvt.c       - source for conversion program
cvtdos.exe  - 16-bit DOS executable (8086 instruction set)
cvtdpmi.exe - 32-bit DPMI executable (DPMI server required)
cvtos2.exe  - 32-bit OS/2 executable
cvtlin      - 32-bit Linux ELF executable
cvtw32.exe  - 32-bit Win32 console executable

E-mail <thannymeister@yahoo.com> for any problem reports.
