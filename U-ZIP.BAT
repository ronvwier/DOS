@echo off

pkzip -u plmtr11D palmtris.exe palmtris.doc info-rvw.doc scherm.doc
pkzip -u plmtr11O info-rvw.doc
pkzip -u plmtr11S @palmtris.src

pkzip -u staaf12D staaf.exe staaf.doc info-rvw.doc scherm.doc
pkzip -u staaf12O info-rvw.doc
pkzip -u staaf12S @staaf.src

pkzip -u schad21D schade.exe schade.doc schade.tab info-rvw.doc scherm.doc
pkzip -u schad21O info-rvw.doc
pkzip -u schad21S @schade.src

pkzip -u dm35D    dm*.exe dm*.doc info-rvw.doc scherm.doc
pkzip -u dm35O    info-rvw.doc
pkzip -u dm35S    @dm.src @dmdlg.src

pkzip -u vt23D    vt.exe vtplay.exe vt*.doc vt*.bat modemtst.exe at.exe info-rvw.doc scherm.doc
pkzip -u vt22O    info-rvw.doc
pkzip -u vt23S    @vt.src @vtplay.src @modemtst.src @at.src

pkzip -u vtl23D   vt*.exe vt*.doc vt*.bat modemtst.exe at.exe info-rvw.doc scherm.doc
pkzip -u vtl23S   vtl.cpp vtdisl.cpp @vt.src @vtplay.src @modemtst.src @at.src

pkzip -u pmvt03O  info-rvw.doc

pkzip -u xcd13D   xcd.exe xcd.doc info-rvw.doc scherm.doc
pkzip -u xcd13O   info-rvw.doc
pkzip -u xcd13S   @xcd.src

pkzip -u zie27D   zie.exe zie.doc info-rvw.doc scherm.doc
pkzip -u zie27O   info-rvw.doc
pkzip -u zie27S   @zie.src

pkzip -u waitmon  waitmon.exe waitmon.doc
pkzip -u waitmon  @waitmon.src
