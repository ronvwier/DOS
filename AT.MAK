.AUTODEPEND

#		*Translator Definitions*
CC = tcc +AT.CFG
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
 at.obj \
 commhw.obj \
 modem.obj \
 strings.obj \
 cmdline.obj \
 ioport.obj \
 commport.obj \
 hwcomm.obj \
 bioscomm.obj \
 wekker.obj \
 fossil.obj \
 taak.obj

#		*Explicit Rules*
at.exe: at.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
at.obj+
commhw.obj+
modem.obj+
strings.obj+
cmdline.obj+
ioport.obj+
commport.obj+
hwcomm.obj+
bioscomm.obj+
wekker.obj+
fossil.obj+
taak.obj
at
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
at.obj: at.cfg at.cpp 

commhw.obj: at.cfg commhw.cpp 

modem.obj: at.cfg modem.cpp 

strings.obj: at.cfg strings.cpp 

cmdline.obj: at.cfg cmdline.cpp 

ioport.obj: at.cfg ioport.cpp 

commport.obj: at.cfg commport.cpp 

hwcomm.obj: at.cfg hwcomm.cpp 

bioscomm.obj: at.cfg bioscomm.cpp 

wekker.obj: at.cfg wekker.cpp 

fossil.obj: at.cfg fossil.cpp 

taak.obj: at.cfg taak.cpp 

#		*Compiler Configuration File*
at.cfg: at.mak
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
| at.cfg


