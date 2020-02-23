# Microsoft Developer Studio Generated NMAKE File, Based on EIP_RF.dsp
!IF "$(CFG)" == ""
CFG=EIP_RF - Win32 Debug
!MESSAGE No configuration specified. Defaulting to EIP_RF - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "EIP_RF - Win32 Release" && "$(CFG)" != "EIP_RF - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EIP_RF.mak" CFG="EIP_RF - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EIP_RF - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "EIP_RF - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "EIP_RF - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "k:\Driver\EIP_RF.dll"

!ELSE 

ALL : "k:\Driver\EIP_RF.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\driver.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\EIP_RF.exp"
	-@erase "$(OUTDIR)\EIP_RF.lib"
	-@erase "k:\Driver\EIP_RF.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\EIP_RF.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib cimrs232.lib cimserhd.lib /nologo /subsystem:windows /dll\
 /pdb:none /machine:I386 /nodefaultlib:"libcmt.lib" /out:"k:\Driver\EIP_RF.dll"\
 /implib:"$(OUTDIR)\EIP_RF.lib" /libpath:"k:\lib" 
LINK32_OBJS= \
	"$(INTDIR)\driver.obj"

"k:\Driver\EIP_RF.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "EIP_RF - Win32 Debug"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "k:\Driver\EIP_RF.dll"

!ELSE 

ALL : "k:\Driver\EIP_RF.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\driver.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\EIP_RF.exp"
	-@erase "$(OUTDIR)\EIP_RF.lib"
	-@erase "k:\Driver\EIP_RF.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /I "..\h" /I "k:\h" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\EIP_RF.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cimrs232.lib cimserhd.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:none\
 /machine:I386 /nodefaultlib:"libcmt.lib" /out:"k:\Driver\EIP_RF.dll"\
 /implib:"$(OUTDIR)\EIP_RF.lib" /libpath:"k:\lib" 
LINK32_OBJS= \
	"$(INTDIR)\driver.obj"

"k:\Driver\EIP_RF.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "EIP_RF - Win32 Release" || "$(CFG)" == "EIP_RF - Win32 Debug"
SOURCE=..\SOURCE\driver.c
DEP_CPP_DRIVE=\
	"..\H\driver.h"\
	"k:\h\cimrs232.h"\
	

"$(INTDIR)\driver.obj" : $(SOURCE) $(DEP_CPP_DRIVE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

