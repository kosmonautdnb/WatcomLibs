gpp -c -O3 -o_imgui.obj imgui.cpp
gpp -c -O3 -o_glimpl.obj glimpl.cpp
gpp -c -O3 -o_imguigl.obj imguigl.cpp
gpp -c -O3 -o_imgdmo.obj imguidmo.cpp
gpp -o imguidmo.exe _imgui.obj _glimpl.obj _imguigl.obj _imgdmo.obj
@echo off
del _imgui.obj
del _glimpl.obj
del _imguigl.obj
del _imgdmo.obj
imguidmo.exe
del imguidmo.exe
