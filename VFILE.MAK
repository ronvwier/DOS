.AUTODEPEND

#		*Translator Definitions*
CC = tcc +VFILE.CFG
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
 vfile.obj

#		*Explicit Rules*
vfile.exe: vfile.cfg $(EXE_dependencies)
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
vfile.obj
vfile
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
vfile.obj: vfile.cfg vfile.cpp 

#		*Compiler Configuration File*
vfile.cfg: vfile.mak
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
| vfile.cfg


