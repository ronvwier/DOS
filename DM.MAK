.AUTODEPEND

#		*Translator Definitions*
CC = tcc +DM.CFG
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
 dm.obj

#		*Explicit Rules*
dm.exe: dm.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
dm.obj
dm
		# no map file
cl.lib
|


#		*Individual File Dependencies*
dm.obj: dm.cfg dm.cpp 

#		*Compiler Configuration File*
dm.cfg: dm.mak
  copy &&|
-ml
-f-
-K
-O
-Z
-k-
-d
-vi-
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
| dm.cfg


