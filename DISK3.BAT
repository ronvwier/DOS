@echo off

if (%1)==() goto einde
cls
rem format a: /f:720 /v:RVWTOOLS
echo RVWTOOLS|label a:
md a:\dos
md a:\os2
pkunzip %1D a:\dos
pkunzip %1O a:\os2
copy instdos.bat a:
copy instos2.cmd a:

:einde
