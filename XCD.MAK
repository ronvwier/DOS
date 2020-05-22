.AUTODEPEND

#		*Translator Definitions*
CC = tcc +XCD.CFG
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
 dosdrive.obj \
 scherm.obj \
 dialoog.obj \
 kleur.obj \
 toets.obj \
 taak.obj \
 geluid.obj \
 muis.obj \
 doserror.obj \
 tijd.obj \
 xcd.obj

#		*Explicit Rules*
xcd.exe: xcd.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
dosdrive.obj+
scherm.obj+
dialoog.obj+
kleur.obj+
toets.obj+
taak.obj+
geluid.obj+
muis.obj+
doserror.obj+
tijd.obj+
xcd.obj
xcd
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
dosdrive.obj: xcd.cfg dosdrive.cpp 

scherm.obj: xcd.cfg scherm.cpp 

dialoog.obj: xcd.cfg dialoog.cpp 

kleur.obj: xcd.cfg kleur.cpp 

toets.obj: xcd.cfg toets.cpp 

taak.obj: xcd.cfg taak.cpp 

geluid.obj: xcd.cfg geluid.cpp 

muis.obj: xcd.cfg muis.cpp 

doserror.obj: xcd.cfg doserror.cpp 

tijd.obj: xcd.cfg tijd.cpp 

xcd.obj: xcd.cfg xcd.cpp 

#		*Compiler Configuration File*
xcd.cfg: xcd.mak
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
| xcd.cfg


