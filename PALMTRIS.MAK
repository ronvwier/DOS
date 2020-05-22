.AUTODEPEND

#		*Translator Definitions*
CC = tcc +PALMTRIS.CFG
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
 palmtris.obj \
 geluid.obj \
 muis.obj \
 toets.obj \
 score.obj \
 tijd.obj \
 scherm.obj \
 kleur.obj \
 taak.obj \
 dialoog.obj

#		*Explicit Rules*
palmtris.exe: palmtris.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
palmtris.obj+
geluid.obj+
muis.obj+
toets.obj+
score.obj+
tijd.obj+
scherm.obj+
kleur.obj+
taak.obj+
dialoog.obj
palmtris
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
palmtris.obj: palmtris.cfg palmtris.cpp 

geluid.obj: palmtris.cfg geluid.cpp 

muis.obj: palmtris.cfg muis.cpp 

toets.obj: palmtris.cfg toets.cpp 

score.obj: palmtris.cfg score.cpp 

tijd.obj: palmtris.cfg tijd.cpp 

scherm.obj: palmtris.cfg scherm.cpp 

kleur.obj: palmtris.cfg kleur.cpp 

taak.obj: palmtris.cfg taak.cpp 

dialoog.obj: palmtris.cfg dialoog.cpp 

#		*Compiler Configuration File*
palmtris.cfg: palmtris.mak
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
| palmtris.cfg


