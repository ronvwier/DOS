.AUTODEPEND

#		*Translator Definitions*
CC = tcc +VT.CFG
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
 vtdisld.obj \
 dialoog.obj \
 vtesc.obj \
 patroon.obj \
 taak.obj \
 tijd.obj \
 kleur.obj \
 bioscomm.obj \
 fossil.obj \
 wekker.obj \
 videotex.obj \
 muis.obj \
 toets.obj \
 geluid.obj \
 scherm.obj \
 vtlaad.obj \
 vt.obj \
 vtdis.obj \
 modem.obj \
 commhw.obj \
 commport.obj \
 ioport.obj \
 hwcomm.obj

#		*Explicit Rules*
vt.exe: vt.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
vtdisld.obj+
dialoog.obj+
vtesc.obj+
patroon.obj+
taak.obj+
tijd.obj+
kleur.obj+
bioscomm.obj+
fossil.obj+
wekker.obj+
videotex.obj+
muis.obj+
toets.obj+
geluid.obj+
scherm.obj+
vtlaad.obj+
vt.obj+
vtdis.obj+
modem.obj+
commhw.obj+
commport.obj+
ioport.obj+
hwcomm.obj
vt
		# no map file
cl.lib
|


#		*Individual File Dependencies*
vtdisld.obj: vt.cfg vtdisld.cpp 

dialoog.obj: vt.cfg dialoog.cpp 

vtesc.obj: vt.cfg vtesc.cpp 

patroon.obj: vt.cfg patroon.cpp 

taak.obj: vt.cfg taak.cpp 

tijd.obj: vt.cfg tijd.cpp 

kleur.obj: vt.cfg kleur.cpp 

bioscomm.obj: vt.cfg bioscomm.cpp 

fossil.obj: vt.cfg fossil.cpp 

wekker.obj: vt.cfg wekker.cpp 

videotex.obj: vt.cfg videotex.cpp 

muis.obj: vt.cfg muis.cpp 

toets.obj: vt.cfg toets.cpp 

geluid.obj: vt.cfg geluid.cpp 

scherm.obj: vt.cfg scherm.cpp 

vtlaad.obj: vt.cfg vtlaad.cpp 

vt.obj: vt.cfg vt.cpp 

vtdis.obj: vt.cfg vtdis.cpp 

modem.obj: vt.cfg modem.cpp 

commhw.obj: vt.cfg commhw.cpp 

commport.obj: vt.cfg commport.cpp 

ioport.obj: vt.cfg ioport.cpp 

hwcomm.obj: vt.cfg hwcomm.cpp 

#		*Compiler Configuration File*
vt.cfg: vt.mak
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
| vt.cfg


