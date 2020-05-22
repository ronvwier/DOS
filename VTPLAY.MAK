.AUTODEPEND

#		*Translator Definitions*
CC = tcc +VTPLAY.CFG
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
 vtplay.obj \
 geluid.obj \
 scherm.obj \
 vtdis.obj \
 videotex.obj \
 toets.obj \
 tijd.obj \
 kleur.obj \
 taak.obj

#		*Explicit Rules*
vtplay.exe: vtplay.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
vtplay.obj+
geluid.obj+
scherm.obj+
vtdis.obj+
videotex.obj+
toets.obj+
tijd.obj+
kleur.obj+
taak.obj
vtplay
		# no map file
cl.lib
|


#		*Individual File Dependencies*
vtplay.obj: vtplay.cfg vtplay.cpp 

geluid.obj: vtplay.cfg geluid.cpp 

scherm.obj: vtplay.cfg scherm.cpp 

vtdis.obj: vtplay.cfg vtdis.cpp 

videotex.obj: vtplay.cfg videotex.cpp 

toets.obj: vtplay.cfg toets.cpp 

tijd.obj: vtplay.cfg tijd.cpp 

kleur.obj: vtplay.cfg kleur.cpp 

taak.obj: vtplay.cfg taak.cpp 

#		*Compiler Configuration File*
vtplay.cfg: vtplay.mak
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
| vtplay.cfg


