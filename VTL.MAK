.AUTODEPEND

#		*Translator Definitions*
CC = tcc +VTL.CFG
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
 hwcomm.obj \
 dialoog.obj \
 patroon.obj \
 taak.obj \
 tijd.obj \
 kleur.obj \
 wekker.obj \
 videotex.obj \
 muis.obj \
 toets.obj \
 geluid.obj \
 scherm.obj \
 vtl.obj \
 vtdisl.obj \
 modem.obj \
 commport.obj \
 ioport.obj

#		*Explicit Rules*
vtl.exe: vtl.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
hwcomm.obj+
dialoog.obj+
patroon.obj+
taak.obj+
tijd.obj+
kleur.obj+
wekker.obj+
videotex.obj+
muis.obj+
toets.obj+
geluid.obj+
scherm.obj+
vtl.obj+
vtdisl.obj+
modem.obj+
commport.obj+
ioport.obj
vtl
		# no map file
cl.lib
|


#		*Individual File Dependencies*
hwcomm.obj: vtl.cfg hwcomm.cpp 

dialoog.obj: vtl.cfg dialoog.cpp 

patroon.obj: vtl.cfg patroon.cpp 

taak.obj: vtl.cfg taak.cpp 

tijd.obj: vtl.cfg tijd.cpp 

kleur.obj: vtl.cfg kleur.cpp 

wekker.obj: vtl.cfg wekker.cpp 

videotex.obj: vtl.cfg videotex.cpp 

muis.obj: vtl.cfg muis.cpp 

toets.obj: vtl.cfg toets.cpp 

geluid.obj: vtl.cfg geluid.cpp 

scherm.obj: vtl.cfg scherm.cpp 

vtl.obj: vtl.cfg vtl.cpp 

vtdisl.obj: vtl.cfg vtdisl.cpp 

modem.obj: vtl.cfg modem.cpp 

commport.obj: vtl.cfg commport.cpp 

ioport.obj: vtl.cfg ioport.cpp 

#		*Compiler Configuration File*
vtl.cfg: vtl.mak
  copy &&|
-ml
-f-
-K
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
| vtl.cfg


