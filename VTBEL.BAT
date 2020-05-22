@echo off
echo off
rem --------------------------------------------------------------------------
rem VTBEL: voorbeeld batch procedure voor bellen VideoTex BBS
rem
rem ÚÄ COM poort nummer (1..4 of ?)
rem ³  ÚÄ lijnsnelheid (1200..9600)
rem ³  ³   ÚÄ centrale (T=Toon, P=Puls)
rem ³  ³   ³ ÚÄ telefoonnummer
rem ³  ³   ³ ³   ÚÄ schermoptie (/S0../S2)
rem ³  ³   ³ ³   ³  ÚÄ extra optiesÄÄÄÄÄÄ¿
rem v  v   v v   v  v  v  v  v  v  v  v  v
 vt ? 2400 T %1 /S1 %2 %3 %4 %5 %6 %7 %8 %9 %VTCFG%
