.AUTODEPEND

#		*Translator Definitions*
CC = tcc +DS.CFG
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
 ds.obj \
 geluid.obj \
 scherm.obj \
 dialoog.obj \
 kleur.obj \
 tijd.obj \
 toets.obj \
 muis.obj \
 taak.obj \
 dbase.obj \
 diapulse.obj \
 diropen.obj \
 doserror.obj \
 wekker.obj \
 dosfind.obj \
 dosdir.obj \
 dosdrive.obj \
 tekst.obj

#		*Explicit Rules*
ds.exe: ds.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
ds.obj+
geluid.obj+
scherm.obj+
dialoog.obj+
kleur.obj+
tijd.obj+
toets.obj+
muis.obj+
taak.obj+
dbase.obj+
diapulse.obj+
diropen.obj+
doserror.obj+
wekker.obj+
dosfind.obj+
dosdir.obj+
dosdrive.obj+
tekst.obj
ds
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
ds.obj: ds.cfg ds.cpp 

geluid.obj: ds.cfg geluid.cpp 

scherm.obj: ds.cfg scherm.cpp 

dialoog.obj: ds.cfg dialoog.cpp 

kleur.obj: ds.cfg kleur.cpp 

tijd.obj: ds.cfg tijd.cpp 

toets.obj: ds.cfg toets.cpp 

muis.obj: ds.cfg muis.cpp 

taak.obj: ds.cfg taak.cpp 

dbase.obj: ds.cfg dbase.cpp 

diapulse.obj: ds.cfg diapulse.cpp 

diropen.obj: ds.cfg diropen.cpp 

doserror.obj: ds.cfg doserror.cpp 

wekker.obj: ds.cfg wekker.cpp 

dosfind.obj: ds.cfg dosfind.cpp 

dosdir.obj: ds.cfg dosdir.cpp 

dosdrive.obj: ds.cfg dosdrive.cpp 

tekst.obj: ds.cfg tekst.cpp 

#		*Compiler Configuration File*
ds.cfg: ds.mak
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
-w-rpt
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
| ds.cfg


