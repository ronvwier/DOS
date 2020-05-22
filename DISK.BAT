@echo off

if (%1)==() goto einde
cls
if not (%1)==() pkunzip %1 a:
if not (%2)==() pkunzip %2 a:
if not (%3)==() pkunzip %3 a:
if not (%4)==() pkunzip %4 a:
if not (%5)==() pkunzip %5 a:
if not (%6)==() pkunzip %6 a:
if not (%7)==() pkunzip %7 a:
if not (%8)==() pkunzip %8 a:
if not (%9)==() pkunzip %9 a:
copy install.bat a:

:einde
