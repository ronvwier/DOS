.AUTODEPEND

#		*Translator Definitions*
CC = tcc +STAAF.CFG
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
 staaf.obj \
 geluid.obj \
 scherm.obj \
 dialoog.obj \
 kleur.obj \
 tijd.obj \
 toets.obj \
 muis.obj \
 taak.obj \
 score.obj

#		*Explicit Rules*
staaf.exe: staaf.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
staaf.obj+
geluid.obj+
scherm.obj+
dialoog.obj+
kleur.obj+
tijd.obj+
toets.obj+
muis.obj+
taak.obj+
score.obj
staaf
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
staaf.obj: staaf.cfg staaf.cpp 

geluid.obj: staaf.cfg geluid.cpp 

scherm.obj: staaf.cfg scherm.cpp 

dialoog.obj: staaf.cfg dialoog.cpp 

kleur.obj: staaf.cfg kleur.cpp 

tijd.obj: staaf.cfg tijd.cpp 

toets.obj: staaf.cfg toets.cpp 

muis.obj: staaf.cfg muis.cpp 

taak.obj: staaf.cfg taak.cpp 

score.obj: staaf.cfg score.cpp 

#		*Compiler Configuration File*
staaf.cfg: staaf.mak
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
| staaf.cfg


