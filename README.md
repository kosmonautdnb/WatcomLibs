# TemplateLib_WatcomC
A very very basic and reduced STL like library for WatcomC (WatcomC++ is different on some points, all here works well with Watcom but e.g. not with VisualC++, it's just supposed for WatcomC++)  
The libraries needed some tweaks, because of "different" WatcomC++ mechanics.  

## OpenGL for DOS
https://github.com/kosmonautdnb/OpenGLForDos-WatcomC.git  
GL.h  - an OpenGL subset with fixed function pipeline, except stencil buffer and projective textures, with some glu functions and Vesa/VGA support  
GLIMPL.cpp - complete OpenGL implementation  

## ImGui v1.43 included and working
(needs OpenGLForDos)  
https://github.com/ocornut/imgui  
IMCONFIG.H  
IMGUI.CPP  
IMGUI.H  
IMGUIGL.CPP  
IMGUIGL.HPP  
FileDLG.hpp - a DOS file dialog window written for IMGUI  

## Some basic types
Types.hpp - a bit like "stdint"   
Object.hpp - a bit like "java"  
Array.hpp - a bit like std::vector  
HashMap.hpp - a bit like std:map   
Set.hpp - a bit like std::set
String.hpp - a bit like std::string  
Pair.hpp - a bit like std::pair  
Vector.hpp - some 3d vector stuff (w is handled differently than xyz) with virtual function table overhead (Object.hpp)  
Matrix.hpp - some 3d matrix stuff with virtual function table overhead (Object.hpp)  
Random.hpp - some basic "dependend random"

## Some useful classes
Charset.hpp - a tiny font to be rendered per pixel  
Dos.hpp - easy access to DOS functionality, comes with long file name support  
FXAA.hpp - full screen post process anti aliasing  
Jsonread.hpp - a json reader  
Jsonwrt.hpp - a json writer  
Keymtrx.hpp - direct access to the keyboard matrix  
PLMPEG.hpp - MPEG1 (Video/Sound) Replay Single Header library  
SMPLOBJL.hpp - a basic/tiny wavefront .OBJ/.MTL mesh loader  
SPRITE.hpp - a sprite (and 3D sphereParticle/ball) renderer compatible with OpenGL  
STBIMGRS.hpp - a image rescale library (many filters)  
STBRECT.hpp - Texture atlas generator  
STBTEXT.hpp - Text editor basics  
STBTTF.hpp - Truetype text/font renderer  
TINYEXPR.hpp - a math expression solver (string input)  
Image.hpp - Image loading/saving/resizing  
minimp3.hpp - A mp3 decoder  
mpeg1.hpp - mpeg1 wrapper for PLMPEG.hpp  
psdimage.hpp - simple psd loader featuring layers  
speaker.hpp - speaker sample playback, and current time lookup with finer granularity  
stbimage.hpp - loader for different image formats. Wrapped in image.hpp  
stbimgwr.hpp - writer for different image formats. Wrapped in image.hpp  
textures.hpp - a basic "texture cache"  
truetype.hpp - true type font rendering (also in OpenGL). Wrapped in Truetype.hpp  
ufbx.hpp - an .fbx loader, however too big for WatcomC to diggest/compile  

### WatcomC with 256 MB of memory instead of the 32 MB with Dos4gw

Maybe use PMODE/W (you can utilize around 256MB of memory this way)

PMODE/W is public domain since many years.

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

