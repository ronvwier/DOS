.AUTODEPEND

#		*Translator Definitions*
CC = tcc +PLAKKER.CFG
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
 plakker.obj \
 geluid.obj \
 scherm.obj \
 dialoog.obj \
 kleur.obj \
 tijd.obj \
 toets.obj \
 muis.obj \
 taak.obj

#		*Explicit Rules*
plakker.exe: plakker.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
plakker.obj+
geluid.obj+
scherm.obj+
dialoog.obj+
kleur.obj+
tijd.obj+
toets.obj+
muis.obj+
taak.obj
plakker
		# no map file
cl.lib
|


#		*Individual File Dependencies*
plakker.obj: plakker.cfg plakker.cpp 

geluid.obj: plakker.cfg geluid.cpp 

scherm.obj: plakker.cfg scherm.cpp 

dialoog.obj: plakker.cfg dialoog.cpp 

kleur.obj: plakker.cfg kleur.cpp 

tijd.obj: plakker.cfg tijd.cpp 

toets.obj: plakker.cfg toets.cpp 

muis.obj: plakker.cfg muis.cpp 

taak.obj: plakker.cfg taak.cpp 

#		*Compiler Configuration File*
plakker.cfg: plakker.mak
  copy &&|
-ml
-f-
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
| plakker.cfg


