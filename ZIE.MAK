.AUTODEPEND

#		*Translator Definitions*
CC = tcc +ZIE.CFG
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
 doserror.obj \
 zie.obj \
 zielees.obj \
 dialoog.obj \
 bestand.obj \
 dosdir.obj \
 muis.obj \
 dosdrive.obj \
 dosfind.obj \
 diropen.obj \
 geluid.obj \
 kleur.obj \
 scherm.obj \
 taak.obj \
 tekst.obj \
 toets.obj \
 tijd.obj

#		*Explicit Rules*
zie.exe: zie.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
doserror.obj+
zie.obj+
zielees.obj+
dialoog.obj+
bestand.obj+
dosdir.obj+
muis.obj+
dosdrive.obj+
dosfind.obj+
diropen.obj+
geluid.obj+
kleur.obj+
scherm.obj+
taak.obj+
tekst.obj+
toets.obj+
tijd.obj
zie
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
doserror.obj: zie.cfg doserror.cpp 

zie.obj: zie.cfg zie.cpp 

zielees.obj: zie.cfg zielees.cpp 

dialoog.obj: zie.cfg dialoog.cpp 

bestand.obj: zie.cfg bestand.cpp 

dosdir.obj: zie.cfg dosdir.cpp 

muis.obj: zie.cfg muis.cpp 

dosdrive.obj: zie.cfg dosdrive.cpp 

dosfind.obj: zie.cfg dosfind.cpp 

diropen.obj: zie.cfg diropen.cpp 

geluid.obj: zie.cfg geluid.cpp 

kleur.obj: zie.cfg kleur.cpp 

scherm.obj: zie.cfg scherm.cpp 

taak.obj: zie.cfg taak.cpp 

tekst.obj: zie.cfg tekst.cpp 

toets.obj: zie.cfg toets.cpp 

tijd.obj: zie.cfg tijd.cpp 

#		*Compiler Configuration File*
zie.cfg: zie.mak
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
-w-cpt
-wdef
-w-dup
-w-pia
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
| zie.cfg


