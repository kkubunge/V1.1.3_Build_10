# Microsoft Developer Studio Generated NMAKE File, Based on Hook.dsp
!IF "$(CFG)" == ""
CFG=Hook - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Hook - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Hook - Win32 Release" && "$(CFG)" != "Hook - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Hook.mak" CFG="Hook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Hook - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Hook - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Hook - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\Hook.dll" "$(OUTDIR)\Hook.bsc"


CLEAN :
	-@erase "$(INTDIR)\Hook.obj"
	-@erase "$(INTDIR)\Hook.res"
	-@erase "$(INTDIR)\Hook.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Hook.bsc"
	-@erase "$(OUTDIR)\Hook.exp"
	-@erase "$(OUTDIR)\Hook.lib"
	-@erase "..\..\Hook.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HOOK_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x412 /fo"$(INTDIR)\Hook.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Hook.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Hook.sbr"

"$(OUTDIR)\Hook.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Hook.pdb" /machine:I386 /def:".\Hook.def" /out:"..\..\Hook.dll" /implib:"$(OUTDIR)\Hook.lib" 
DEF_FILE= \
	".\Hook.def"
LINK32_OBJS= \
	"$(INTDIR)\Hook.obj" \
	"$(INTDIR)\Hook.res"

"..\..\Hook.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Hook - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Hook.dll"


CLEAN :
	-@erase "$(INTDIR)\Hook.obj"
	-@erase "$(INTDIR)\Hook.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Hook.dll"
	-@erase "$(OUTDIR)\Hook.exp"
	-@erase "$(OUTDIR)\Hook.ilk"
	-@erase "$(OUTDIR)\Hook.lib"
	-@erase "$(OUTDIR)\Hook.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HOOK_EXPORTS" /Fp"$(INTDIR)\Hook.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x412 /fo"$(INTDIR)\Hook.res" /i "WIN32" /i "NDEBUG" /i "_MBCS" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Hook.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Hook.pdb" /debug /machine:I386 /def:".\Hook.def" /out:"$(OUTDIR)\Hook.dll" /implib:"$(OUTDIR)\Hook.lib" /pdbtype:sept 
DEF_FILE= \
	".\Hook.def"
LINK32_OBJS= \
	"$(INTDIR)\Hook.obj" \
	"$(INTDIR)\Hook.res"

"$(OUTDIR)\Hook.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Hook.dep")
!INCLUDE "Hook.dep"
!ELSE 
!MESSAGE Warning: cannot find "Hook.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Hook - Win32 Release" || "$(CFG)" == "Hook - Win32 Debug"
SOURCE=.\Hook.cpp

!IF  "$(CFG)" == "Hook - Win32 Release"


"$(INTDIR)\Hook.obj"	"$(INTDIR)\Hook.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Hook - Win32 Debug"


"$(INTDIR)\Hook.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Hook.rc

"$(INTDIR)\Hook.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

