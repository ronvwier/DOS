.AUTODEPEND

#		*Translator Definitions*
CC = tcc +DMDLG.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = D:\TC\LIB
INCLUDEPATH = D:\TC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 dmdlg.obj \
 dmaktie.obj \
 dmcd.obj \
 dmgroep.obj \
 dmkeuze.obj \
 dmmenu.obj \
 dmparm.obj \
 dialoog.obj \
 geluid.obj \
 kleur.obj \
 muis.obj \
 scherm.obj \
 taak.obj \
 tijd.obj \
 tekst.obj \
 toets.obj \
 dmfind.obj

#		*Explicit Rules*
dmdlg.exe: dmdlg.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
dmdlg.obj+
dmaktie.obj+
dmcd.obj+
dmgroep.obj+
dmkeuze.obj+
dmmenu.obj+
dmparm.obj+
dialoog.obj+
geluid.obj+
kleur.obj+
muis.obj+
scherm.obj+
taak.obj+
tijd.obj+
tekst.obj+
toets.obj+
dmfind.obj
dmdlg
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
dmdlg.obj: dmdlg.cfg dmdlg.cpp 

dmaktie.obj: dmdlg.cfg dmaktie.cpp 

dmcd.obj: dmdlg.cfg dmcd.cpp 

dmgroep.obj: dmdlg.cfg dmgroep.cpp 

dmkeuze.obj: dmdlg.cfg dmkeuze.cpp 

dmmenu.obj: dmdlg.cfg dmmenu.cpp 

dmparm.obj: dmdlg.cfg dmparm.cpp 

dialoog.obj: dmdlg.cfg dialoog.cpp 

geluid.obj: dmdlg.cfg geluid.cpp 

kleur.obj: dmdlg.cfg kleur.cpp 

muis.obj: dmdlg.cfg muis.cpp 

scherm.obj: dmdlg.cfg scherm.cpp 

taak.obj: dmdlg.cfg taak.cpp 

tijd.obj: dmdlg.cfg tijd.cpp 

tekst.obj: dmdlg.cfg tekst.cpp 

toets.obj: dmdlg.cfg toets.cpp 

dmfind.obj: dmdlg.cfg dmfind.cpp 

#		*Compiler Configuration File*
dmdlg.cfg: dmdlg.mak
  copy &&|
-ml
-K
-v
-O
-Z
-d
-vi-
-Ff=50
-w-ret
-w-nci
-w-inl
-wpin
-wamb
-wamp
-w-par
-wasm
-wcln
-w-cpt
-wdef
-w-dup
-w-pia
-wsig
-wnod
-w-ill
-w-sus
-wstv
-wucp
-wuse
-w-ext
-w-ias
-w-ibc
-w-pre
-w-nst
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DMSDOS
| dmdlg.cfg


