@echo off
echo off
rem --------------------------------------------------------------------------
rem VTBEL: voorbeeld batch procedure voor bellen VideoTex BBS
rem
rem �� COM poort nummer (1..4 of ?)
rem �  �� lijnsnelheid (1200..9600)
rem �  �   �� centrale (T=Toon, P=Puls)
rem �  �   � �� telefoonnummer
rem �  �   � �   �� schermoptie (/S0../S2)
rem �  �   � �   �  �� extra opties�����Ŀ
rem v  v   v v   v  v  v  v  v  v  v  v  v
 vt ? 2400 T %1 /S1 %2 %3 %4 %5 %6 %7 %8 %9 %VTCFG%
