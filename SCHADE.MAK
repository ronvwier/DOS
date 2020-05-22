.AUTODEPEND

#		*Translator Definitions*
CC = tcc +SCHADE.CFG
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
 schade.obj \
 schade1.obj \
 schade2.obj \
 schade3.obj \
 scherm.obj \
 dialoog.obj \
 kleur.obj \
 tijd.obj \
 toets.obj \
 muis.obj \
 taak.obj \
 geluid.obj

#		*Explicit Rules*
schade.exe: schade.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
schade.obj+
schade1.obj+
schade2.obj+
schade3.obj+
scherm.obj+
dialoog.obj+
kleur.obj+
tijd.obj+
toets.obj+
muis.obj+
taak.obj+
geluid.obj
schade
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
schade.obj: schade.cfg schade.cpp 

schade1.obj: schade.cfg schade1.cpp 

schade2.obj: schade.cfg schade2.cpp 

schade3.obj: schade.cfg schade3.cpp 

scherm.obj: schade.cfg scherm.cpp 

dialoog.obj: schade.cfg dialoog.cpp 

kleur.obj: schade.cfg kleur.cpp 

tijd.obj: schade.cfg tijd.cpp 

toets.obj: schade.cfg toets.cpp 

muis.obj: schade.cfg muis.cpp 

taak.obj: schade.cfg taak.cpp 

geluid.obj: schade.cfg geluid.cpp 

#		*Compiler Configuration File*
schade.cfg: schade.mak
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
| schade.cfg


