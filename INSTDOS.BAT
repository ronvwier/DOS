echo off
cls
rem ------------------------------------------------------------------------
rem INSTDOS    : Plaatsen van software vanaf floppy op harde schijf en
rem              eventueel afdrukken van de documentatie.
rem
rem parameters : 1. (eventueel) naam van de drive/directory waar de software
rem                 geplaatst dient te worden. Indien afwezig wordt de
rem                 software in C:\RVWTOOLS geplaatst
rem
rem (c) 1991   : R.A. van Wier RI, Amsterdam.
rem -------------------------------------------------------------------------

if not exist \instdos.bat goto flopfout

echo ------------------------------------------------------------------------
echo Alle software en documentatie wordt nu gecopieerd
echo ------------------------------------------------------------------------

cd \dos

if (%1)==() goto default

md %1 > nul
copy *.* %1\*.*
cd %1
echo ------------------------------------------------------------------------
echo Alle software en documentatie is geplaatst in %1
echo ------------------------------------------------------------------------
goto printen

:default

md C:\RVWTOOLS > nul
copy *.* C:\RVWTOOLS\*.*
cd C:\RVWTOOLS
echo ------------------------------------------------------------------------
echo Alle software en documentatie is geplaatst in C:\RVWTOOLS
echo ------------------------------------------------------------------------

goto printen

:printen

echo ------------------------------------------------------------------------
echo U kunt nu eventueel alle documentatie afdrukken door het intoetsen van
echo het volgende commando gevolgd door een enter/return
echo    COPY *.DOC LPT1
echo ------------------------------------------------------------------------

goto einde

:flopfout
echo ------------------------------------------------------------------------
echo FOUT: INSTDOS niet gestart vanaf de huidige drive
echo ------------------------------------------------------------------------
echo Toets A: in indien de floppy in drive A: zit of B: indien de floppy in
echo drive B: zit gevolgd door een enter/return.
echo Toets daarna INSTDOS in gevolgd door de naam van de doeldirectory en
echo gevolgd door een enter/return.
echo Bijvoorbeeld:
echo   A:
echo   INSTDOS C:\RVWTOOLS
echo ------------------------------------------------------------------------
goto einde
:einde
