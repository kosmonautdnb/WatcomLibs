# WatcomLibs
A very simple and stripped-down STL-like library for WatcomC (WatcomC++ differs in some respects; everything here works well with Watcom, but not with VisualC++, for example, because it's "designed" only for WatcomC++).

All the libraries had to be slightly adapted due to the "different" WatcomC++ mechanics.

This should primarily serve as a starting point for your WatcomC++ FreeDOS experiments/projects. There aren't many WatcomC++ + FreeDOS sources available online yet. I've tried to gather some new and useful content and promote my DOS OpenGL implementation "WatcomGL".

## WatcomGL / OpenGL
https://github.com/kosmonautdnb/WatcomGL
- GL.h  - an OpenGL subset with fixed function pipeline, except stencil buffer and projective textures, with some glu functions and Vesa/VGA support  
- GLIMPL.cpp - complete OpenGL implementation  

## ImGui v1.43 included and working
(needs OpenGLForDos)  
https://github.com/ocornut/imgui  
- IMCONFIG.H  
- IMGUI.CPP  
- IMGUI.H  
- IMGUIGL.CPP  
- IMGUIGL.HPP  
- FileDLG.hpp - a DOS file dialog window written for IMGUI  

## Some basic types
- Types.hpp - a bit like "stdint"   
- Object.hpp - a bit like "java"  
- Array.hpp - a bit like std::vector  
- HashMap.hpp - a bit like std:map   
- Set.hpp - a bit like std::set
- String.hpp - a bit like std::string  
- Pair.hpp - a bit like std::pair  
- Vector.hpp - some 3d vector stuff (w is handled differently than xyz) with virtual function table overhead (Object.hpp)  
- Matrix.hpp - some 3d matrix stuff with virtual function table overhead (Object.hpp)  
Random.hpp - some basic "dependend random"

## Some useful classes
- Charset.hpp - a tiny font to be rendered per pixel
- CGLTF - a gltf loader https://github.com/jkuhlmann/cgltf 
- Dos.hpp - easy access to DOS functionality, comes with long file name support  
- Jsonread.hpp - a json reader https://github.com/rafagafe/tiny-json  
- Jsonwrt.hpp - a json writer https://github.com/rafagafe/tiny-json  
- Parson.hpp - another json reader/writer https://github.com/kgabis/parson
- Keymtrx.hpp - direct access to the keyboard matrix  
- PLMPEG.hpp - MPEG1 (Video/Sound) Replay Single Header library https://github.com/phoboslab/pl_mpeg  
- SMPLOBJL.hpp - a basic/tiny wavefront .OBJ/.MTL mesh loader  
- SPRITE.hpp - a sprite (and 3D sphereParticle/ball) renderer compatible with OpenGL  
- STBIMGRS.hpp - a image rescale library (many filters)  https://github.com/nothings/obbg/tree/master/stb  
- STBRECT.hpp - Texture atlas generator  https://github.com/nothings/obbg/tree/master/stb  
- STBTEXT.hpp - Text editor basics  https://github.com/nothings/obbg/tree/master/stb  
- STBTTF.hpp - Truetype text/font renderer  https://github.com/nothings/obbg/tree/master/stb  
- TINYEXPR.hpp - a math expression solver (string input)  https://github.com/codeplea/tinyexpr  
- TSpline.hpp - a advanced spline evaluation/rendering library https://github.com/msteinbeck/tinyspline
-  Image.hpp - Image loading/saving/resizing  
- Minimp3.hpp - A mp3 decoder  https://github.com/lieff/minimp3  
- Mpeg1.hpp - mpeg1 wrapper for PLMPEG.hpp
- OpenGLFX.hpp - Some post processing OpenGL effects like antialiasing(FXAA(https://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/ )), toon shader, motion blur
- Psdimage.hpp - simple psd loader featuring layers  (PSD image loader by DrPetter)  
- Quaternn.hpp - a simple Quaternion class, mainly for interpolated rotations.  
- Speaker.hpp - speaker sample playback, and current time lookup with finer granularity  
- Stbimage.hpp - loader for different image formats. Wrapped in image.hpp  https://github.com/nothings/obbg/tree/master/stb  
- Stbimgwr.hpp - writer for different image formats. Wrapped in image.hpp  https://github.com/nothings/obbg/tree/master/stb  
- Textures.hpp - a basic "texture cache"  
- Truetype.hpp - true type font rendering (also in OpenGL). Wrapped in Truetype.hpp  
- Ufbx.hpp - an .fbx loader, however too big for WatcomC to diggest/compile https://github.com/ufbx/ufbx   

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

