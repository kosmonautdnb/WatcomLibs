wpp386 /mf /W10 -foa.obj app_scal.cpp
wpp386 /mf -fob.obj image.cpp
wlink FILE a.obj FILE b.obj OPTION QUIET OPTION STack = 65536 NAME APP_SCAL.EXE
del a.obj
del b.obj
