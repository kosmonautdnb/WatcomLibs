# WatcomLibs
Some useful libraries converted to WatcomC++.
All the libraries had to be slightly adapted due to the "different" WatcomC++ mechanics.

This should primarily serve as a starting point for your WatcomC++ FreeDOS experiments/projects. There aren't many WatcomC++ + FreeDOS sources available online yet. I've tried to gather some new and useful content and promote my DOS OpenGL implementation "WatcomGL".

This repository lives at: https://github.com/kosmonautdnb/WatcomLibs.git  

## WatcomGL / OpenGL
https://github.com/kosmonautdnb/WatcomGL
- GL.h  - an OpenGL subset with fixed function pipeline, except stencil buffer and projective textures, with some glu functions and Vesa/VGA support  
- GLIMPL.cpp - complete OpenGL implementation  

## ImGui v1.43 included and working
(needs WatcomGL)  
https://github.com/ocornut/imgui  
- IMCONFIG.H  
- IMGUI.CPP  
- IMGUI.H  
- IMGUIGL.CPP  
- IMGUIGL.HPP  
- FILEDLG.HPP - a DOS file dialog window written for IMGUI  

#### &emsp;ImGui howto:
&emsp;#include "gl.h"  
&emsp;#include "imguigl.hpp"  

&emsp;// be aware to take mouseX, mouseY, mouseButtons. currentKey and so on from imguigl.hpp  

&emsp;void main() {  
&emsp;&emsp;glVesa(640,480,32);  
&emsp;&emsp;InitImGui(640, 480);  
&emsp;&emsp;while(true) {  
&emsp;&emsp;&emsp;ImGuiNewFrame();  
&emsp;&emsp;&emsp;ImGui::Begin("default window");  
&emsp;&emsp;&emsp;if (ImGui::Button("button1##b1")) {} ImGui::SameLine();  if (ImGui::Button("button2##b2")) {}  
&emsp;&emsp;&emsp;ImGui::End();  
&emsp;&emsp;&emsp;glClearColor(0.1f,0.2f,0.3f,1);  
&emsp;&emsp;&emsp;glClear(GL_COLOR_BUFFER_BIT);  
&emsp;&emsp;&emsp;ImGui::Render();  
&emsp;&emsp;&emsp;glRefresh();  
&emsp;&emsp;}  
&emsp;&emsp;glDone();  
&emsp;}  

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
- CHARSET.HPP - a tiny font to be rendered per pixel
- CGLTF - a gltf loader https://github.com/jkuhlmann/cgltf
- CGLTF_EX.MPL - an example of displaying bones and skinning in OpenGL for CGLTF 
- GTFLOG.CPP - just a temporary / very simple logger for CGLTF file structure.
- DOS.HPP - easy access to DOS functionality, comes with long file name support
- GENANN.HPP - a simple neural net. https://github.com/codeplea/genann
- JSONREAD.HPP - a json reader https://github.com/rafagafe/tiny-json  
- JSONWRT.HPP - a json writer https://github.com/rafagafe/tiny-json  
- PARSON.HPP - another json reader/writer https://github.com/kgabis/parson
- KEYMTRX.HPP - direct access to the keyboard matrix  
- MARCHING.HPP - Marching Cubes and Marching Tetraeders.
- PLMPEG.HPP - MPEG1 (Video/Sound) Replay Single Header library https://github.com/phoboslab/pl_mpeg  
- SMPLOBJL.HPP - a basic/tiny wavefront .OBJ/.MTL mesh loader  
- SPRITE.HPP - a sprite (and 3D sphereParticle/ball) renderer compatible with OpenGL  
- STBIMGRS.HPP - a image rescale library (many filters)  https://github.com/nothings/obbg/tree/master/stb  
- STBRECT.HPP - Texture atlas generator  https://github.com/nothings/obbg/tree/master/stb  
- STBTEXT.HPP - Text editor basics  https://github.com/nothings/obbg/tree/master/stb  
- STBTTF.HPP - Truetype text/font renderer  https://github.com/nothings/obbg/tree/master/stb  
- TINYEXPR.HPP - a math expression solver (string input)  https://github.com/codeplea/tinyexpr  
- TSPLINE.HPP - a advanced spline evaluation/rendering library https://github.com/msteinbeck/tinyspline
- IMAGE.HPP - Image loading/saving/resizing  
- MINIMP3.HPP - A mp3 decoder  https://github.com/lieff/minimp3  
- MPEG1.HPP - mpeg1 wrapper for PLMPEG.hpp
- OPENGLFX.HPP - Some post processing OpenGL effects like antialiasing(FXAA(https://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/ )), toon shader, motion blur
- PSDIMAGE.HPP - simple psd loader featuring layers  (PSD image loader by DrPetter)  
- QUATERNN.HPP - a simple Quaternion class, mainly for interpolated rotations.  
- SPEAKER.HPP - speaker sample playback, and current time lookup with finer granularity  
- STBIMAGE.HPP - loader for different image formats. Wrapped in image.hpp  https://github.com/nothings/obbg/tree/master/stb  
- STBIMGWR.HPP - writer for different image formats. Wrapped in image.hpp  https://github.com/nothings/obbg/tree/master/stb  
- TEXTURES.HPP - a basic "texture cache"  
- TRUETYPE.HPP - true type font rendering (also in OpenGL). Wrapped in Truetype.hpp  
- UFBX.HPP - an .fbx loader, however too big for WatcomC to diggest/compile https://github.com/ufbx/ufbx   

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

