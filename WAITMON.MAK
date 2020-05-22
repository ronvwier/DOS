.AUTODEPEND

#		*Translator Definitions*
CC = tcc +WAITMON.CFG
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
 waitmon.obj

#		*Explicit Rules*
waitmon.exe: waitmon.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
waitmon.obj
waitmon
		# no map file
cl.lib
|


#		*Individual File Dependencies*
waitmon.obj: waitmon.cfg waitmon.c 

#		*Compiler Configuration File*
waitmon.cfg: waitmon.mak
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
| waitmon.cfg


