@echo off

if (%1)==() goto einde
cls
echo RVWTOOLS|label w:
md w:\dos
md w:\os2
pkunzip %1D w:\dos
pkunzip %1O w:\os2
copy instdos.bat w:
copy instos2.cmd w:

:einde
