.AUTODEPEND

#		*Translator Definitions*
CC = tcc +CLOCK.CFG
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
 clock.obj

#		*Explicit Rules*
clock.exe: clock.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
clock.obj
clock
		# no map file
cl.lib
|


#		*Individual File Dependencies*
clock.obj: clock.cfg clock.cpp 

#		*Compiler Configuration File*
clock.cfg: clock.mak
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
| clock.cfg


