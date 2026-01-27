wcl386.exe /c /mf -fospk.obj app_spkt.cpp
wcl386.exe /c /mf -fokeys.obj app_keys.cpp
wcl386.exe /c /mf -fovid.obj app_vid.cpp
wcl386.exe /c /mf -fomp3.obj app_mp3.cpp
wcl386.exe /c /mf -fosb.obj sndblstr.cpp
wcl386.exe /c /mf -fogl.obj glimpl.cpp
wcl386.exe /c /mf -foi1.obj imgui.cpp
wcl386.exe /c /mf -foi2.obj imguigl.cpp
wcl386.exe /c /mf -foo.obj object.cpp
wcl386.exe /xr /c /mf -fos.obj string.cpp
wcl386.exe /c /mf -fosp.obj speaker.cpp
wcl386.exe /c /mf -fod.obj dos.cpp
wcl386.exe /c /mf -fof.obj filedlg.cpp
wcl386.exe /c /mf -foe.obj glega.cpp
wcl386.exe /c /mf -foc.obj cursor.cpp
wcl.exe APP_SPPL.C
wcl386 /c /mf APP_PHYS.CPP
wlink @appkeys.lnk OPTION QUIET OPTION STack = 65536 NAME APP_KEYS.EXE
wlink @appvid.lnk OPTION QUIET OPTION STack = 65536 NAME APP_VID.EXE
wlink @appspkt.lnk OPTION QUIET OPTION STack = 65536 NAME APP_SPKT.EXE
wlink FILE GL.OBJ FILE APP_PHYS.OBJ OPTION QUIET OPTION STack = 65536 NAME APP_PHYS.EXE
wlink FILE SB.OBJ FILE MP3.OBJ FILE C.OBJ OPTION QUIET OPTION STack = 65536 NAME APP_MP3.EXE
@echo off
del keys.obj
del spk.obj
del vid.obj
del mp3.obj
del sb.obj
del gl.obj
del i1.obj
del i2.obj
del o.obj
del s.obj
del sp.obj
del d.obj
del f.obj
del e.obj
del app_sppl.obj
del app_phys.obj
del c.obj
