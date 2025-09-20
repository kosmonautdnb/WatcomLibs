@echo off
wcl386.exe /c /mf imguidmo.cpp  -fo_imguido.obj
wcl386.exe /c /mf imgui.cpp  -fo_imgui.obj
wcl386.exe /c /mf imguigl.cpp  -fo_imguigl.obj
wcl386.exe /c /mf glimpl.cpp  -fo_glimpl.obj
wlink @imguidmo.lnk OPTION QUIET OPTION STack = 65536 NAME imguidmo.exe
del _imguido.obj
del _imgui.obj
del _imguigl.obj
del _glimpl.obj
imguidmo.exe
del imguidmo.exe
