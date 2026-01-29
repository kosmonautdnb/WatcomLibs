wpp386 /mf -fospk.obj app_spkt.cpp
wpp386 /mf -fokeys.obj app_keys.cpp
wpp386 /mf -fovid.obj app_vid.cpp
wpp386 /mf -fomp3.obj app_mp3.cpp
wpp386 /mf app_phys.cpp
wpp386 /mf /xr app_gltf.cpp
wpp386 /mf /xr -fos.obj string.cpp
wpp386 /mf /xr cgltfa.cpp
wpp386 /mf /xr vector.cpp
wpp386 /mf /xr matrix.cpp
wpp386 /mf /xr quaternn.cpp
wpp386 /mf -fosb.obj sndblstr.cpp
wpp386 /mf -fogl.obj glimpl.cpp
wpp386 /mf -foi1.obj imgui.cpp
wpp386 /mf -foi2.obj imguigl.cpp
wpp386 /mf -foo.obj object.cpp
wpp386 /mf -fosp.obj speaker.cpp
wpp386 /mf -fod.obj dos.cpp
wpp386 /mf -fof.obj filedlg.cpp
wpp386 /mf -foe.obj glega.cpp
wpp386 /mf -foc.obj cursor.cpp
wpp386 /mf image.cpp
wpp386 /mf charset.cPP
wpp386 /mf keymtrix.cpp
wcl.exe APP_SPPL.C
wlink @appkeys.lnk OPTION QUIET OPTION STack = 65536 NAME APP_KEYS.EXE
wlink @appvid.lnk OPTION QUIET OPTION STack = 65536 NAME APP_VID.EXE
wlink @appspkt.lnk OPTION QUIET OPTION STack = 65536 NAME APP_SPKT.EXE
wlink @appgltf.lnk OPTION QUIET OPTION STack = 65536 NAME APP_GLTF.EXE
wlink FILE gl.obj FILE app_phys.obj OPTION QUIET OPTION STack = 65536 NAME APP_PHYS.EXE
wlink FILE sb.obj FILE mp3.obj FILE c.obj OPTION QUIET OPTION STack = 65536 NAME APP_MP3.EXE
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
del c.obj
del app_sppl.obj
del app_phys.obj
del app_gltf.obj
del cgltfa.obj
del vector.obj
del matrix.obj
del quaternn.obj
del image.obj
del charset.obj
del keymtrix.obj
