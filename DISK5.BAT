@echo off

if (%1)==() goto einde
cls
format b: /f:360 /u /v:RVWTOOLS

if not (%1)==() pkunzip %1 b:
if not (%2)==() pkunzip %2 b:
if not (%3)==() pkunzip %3 b:
if not (%4)==() pkunzip %4 b:
if not (%5)==() pkunzip %5 b:
if not (%6)==() pkunzip %6 b:
if not (%7)==() pkunzip %7 b:
if not (%8)==() pkunzip %8 b:
if not (%9)==() pkunzip %9 b:

copy install.bat b:

:einde
