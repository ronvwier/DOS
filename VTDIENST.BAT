@echo off
echo off

rem --------------------------------------------------------------------------
rem VTDIENST: Oproepen van een VideoTex nummer
rem
rem Parm's  : 1. Telefoonnummer
rem           2. Naam van de dienst zoals:
rem
rem              TELEGIDS  Telefoongids van Nederland op 06-7400
rem              POSTCODE  Postcodegids van Nederland op 06-7400
rem              PRIMAFOON Informatie over primafoonwinkels op 06-7100
rem
rem --------------------------------------------------------------------------

echo naam^%2#> temp.vta
vtbel %1 /atemp.vta %3 %4 %5 %6 %7 %8 %9
