.AUTODEPEND

#		*Translator Definitions*
CC = tcc +MODEMTST.CFG
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
 commhw.obj \
 modemtst.obj \
 ioport.obj \
 commport.obj \
 modem.obj \
 muis.obj \
 hwcomm.obj \
 fossil.obj \
 bioscomm.obj \
 taak.obj \
 wekker.obj

#		*Explicit Rules*
modemtst.exe: modemtst.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
commhw.obj+
modemtst.obj+
ioport.obj+
commport.obj+
modem.obj+
muis.obj+
hwcomm.obj+
fossil.obj+
bioscomm.obj+
taak.obj+
wekker.obj
modemtst
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
commhw.obj: modemtst.cfg commhw.cpp 

modemtst.obj: modemtst.cfg modemtst.cpp 

ioport.obj: modemtst.cfg ioport.cpp 

commport.obj: modemtst.cfg commport.cpp 

modem.obj: modemtst.cfg modem.cpp 

muis.obj: modemtst.cfg muis.cpp 

hwcomm.obj: modemtst.cfg hwcomm.cpp 

fossil.obj: modemtst.cfg fossil.cpp 

bioscomm.obj: modemtst.cfg bioscomm.cpp 

taak.obj: modemtst.cfg taak.cpp 

wekker.obj: modemtst.cfg wekker.cpp 

#		*Compiler Configuration File*
modemtst.cfg: modemtst.mak
  copy &&|
-ml
-K
-v
-O
-Z
-d
-vi-
-H=MODEMTST.SYM
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
| modemtst.cfg


