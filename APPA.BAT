gpp -c -O3 -Wall -Wextra -Wpedantic -oa.obj app_scal.cpp
gpp -c -O3 -ob.obj image.cpp
gpp -o APP_SCAL.EXE a.obj b.obj
@echo off
del a.obj
del b.obj
