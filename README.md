# TemplateLib_WatcomC
A very very basic and reduced STL like library for WatcomC

Some parts may need https://github.com/kosmonautdnb/OpenGLForDos-WatcomC.git

# WatcomC with 256 MB of memory instead of the 32 MB with Dos4gw

Maybe use PMODE/W (you can utilize around 256MB of memory this way)

PMODE/W is public domain since manyears.

PMODE/W config.sys:

DEVICE=C:\_DOS_\BIN\HIMEMX.exe

DEVICE=C:\_DOS_\BIN\JEMM386.exe  /SB /MAX=262144

/MAX=262144 means 256MB may be available.

pmwsetup.exe to patch your PMODE/W program (V64 and A64 are maximum and mean 256 MB memory accessible):

pmwsetup.exe /M1 /B0 /A64 /V64 /P1024 myprg.exe

Here a pmodew.lnk file:

system begin pmodew
    option osname='PMODE/W'
    libpath %WATCOM%\lib386
    libpath %WATCOM%\lib386\dos
    op stub=_build\pmodew.exe
    format os2 le
end

to be used this way:

wlink @pmodew.lnk @mylink.lnk OPTION QUIET OPTION STack = 65536 NAME myexe.exe

