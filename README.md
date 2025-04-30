# TemplateLib_WatcomC
A very very basic and reduced STL like library for WatcomC

Some parts may need https://github.com/kosmonautdnb/OpenGLForDos-WatcomC.git

Maybe use PMODE/W (you can utilize around 256MB of memory this way)

PMODE/W config.sys:

DEVICE=C:\_DOS_\BIN\HIMEMX.exe

DEVICE=C:\_DOS_\BIN\JEMM386.exe  /SB /MAX=262144

/MAX=262144 means 256MB may be available.

pmwsetup.exe to patch your PMODE/W program (V64 and A64 are maximum and mean 256 MB memory accessible):

pmwsetup.exe /M1 /B0 /A64 /V64 /P1024 myprg.exe

