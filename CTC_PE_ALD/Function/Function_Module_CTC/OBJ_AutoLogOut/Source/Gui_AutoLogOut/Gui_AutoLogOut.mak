# Microsoft Developer Studio Generated NMAKE File, Based on Gui_AutoLogOut.dsp
!IF "$(CFG)" == ""
CFG=Gui_AutoLogOut - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Gui_AutoLogOut - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Gui_AutoLogOut - Win32 Release" && "$(CFG)" != "Gui_AutoLogOut - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gui_AutoLogOut.mak" CFG="Gui_AutoLogOut - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gui_AutoLogOut - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Gui_AutoLogOut - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Gui_AutoLogOut - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\Gui_AutoLogOut.exe" "$(OUTDIR)\Gui_AutoLogOut.bsc"


CLEAN :
	-@erase "$(INTDIR)\Gui_AutoLogOut.res"
	-@erase "$(INTDIR)\prg_code.obj"
	-@erase "$(INTDIR)\prg_code.sbr"
	-@erase "$(INTDIR)\usr_code.obj"
	-@erase "$(INTDIR)\usr_code.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Gui_AutoLogOut.bsc"
	-@erase "..\..\Gui_AutoLogOut.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "." /I "h" /I "K:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
RSC_PROJ=/l 0x412 /fo"$(INTDIR)\Gui_AutoLogOut.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gui_AutoLogOut.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\prg_code.sbr" \
	"$(INTDIR)\usr_code.sbr"

"$(OUTDIR)\Gui_AutoLogOut.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cimsqcon.lib cimsqconio.lib kutlstr.lib kutlanal.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Gui_AutoLogOut.pdb" /machine:I386 /out:"..\..\Gui_AutoLogOut.exe" /libpath:"k:\lib" 
LINK32_OBJS= \
	"$(INTDIR)\prg_code.obj" \
	"$(INTDIR)\usr_code.obj" \
	"$(INTDIR)\Gui_AutoLogOut.res" \
	".\Source\Hook.lib" \
	"K:\lib\kutltime.lib" \
	"K:\lib\kutlanal.lib" \
	"K:\lib\kutletc.lib" \
	"K:\lib\kutlfile.lib" \
	"K:\lib\kutlgui2.lib" \
	"K:\lib\kutlgui2_2.lib" \
	"K:\lib\kutlgui2_3.lib" \
	"K:\lib\kutlgui.lib" \
	"K:\lib\kutlstr.lib"

"..\..\Gui_AutoLogOut.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Gui_AutoLogOut - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Gui_AutoLogOut.exe"


CLEAN :
	-@erase "$(INTDIR)\Gui_AutoLogOut.res"
	-@erase "$(INTDIR)\prg_code.obj"
	-@erase "$(INTDIR)\usr_code.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Gui_AutoLogOut.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\Gui_AutoLogOut.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
RSC_PROJ=/l 0x412 /fo"$(INTDIR)\Gui_AutoLogOut.res" /i "WIN32" /i "NDEBUG" /i "_MBCS" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Gui_AutoLogOut.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Gui_AutoLogOut.pdb" /machine:I386 /out:"$(OUTDIR)\Gui_AutoLogOut.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\prg_code.obj" \
	"$(INTDIR)\usr_code.obj" \
	"$(INTDIR)\Gui_AutoLogOut.res" \
	".\Source\Hook.lib" \
	"K:\lib\kutltime.lib" \
	"K:\lib\kutlanal.lib" \
	"K:\lib\kutletc.lib" \
	"K:\lib\kutlfile.lib" \
	"K:\lib\kutlgui2.lib" \
	"K:\lib\kutlgui2_2.lib" \
	"K:\lib\kutlgui2_3.lib" \
	"K:\lib\kutlgui.lib" \
	"K:\lib\kutlstr.lib"

"$(OUTDIR)\Gui_AutoLogOut.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Gui_AutoLogOut.dep")
!INCLUDE "Gui_AutoLogOut.dep"
!ELSE 
!MESSAGE Warning: cannot find "Gui_AutoLogOut.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Gui_AutoLogOut - Win32 Release" || "$(CFG)" == "Gui_AutoLogOut - Win32 Debug"
SOURCE=.\Source\Gui_AutoLogOut.rc

!IF  "$(CFG)" == "Gui_AutoLogOut - Win32 Release"


"$(INTDIR)\Gui_AutoLogOut.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x412 /fo"$(INTDIR)\Gui_AutoLogOut.res" /i "Source" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "Gui_AutoLogOut - Win32 Debug"


"$(INTDIR)\Gui_AutoLogOut.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x412 /fo"$(INTDIR)\Gui_AutoLogOut.res" /i "WIN32" /i "NDEBUG" /i "_MBCS" /i "Source" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\Source\prg_code.c

!IF  "$(CFG)" == "Gui_AutoLogOut - Win32 Release"


"$(INTDIR)\prg_code.obj"	"$(INTDIR)\prg_code.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Gui_AutoLogOut - Win32 Debug"


"$(INTDIR)\prg_code.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Source\usr_code.c

!IF  "$(CFG)" == "Gui_AutoLogOut - Win32 Release"


"$(INTDIR)\usr_code.obj"	"$(INTDIR)\usr_code.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Gui_AutoLogOut - Win32 Debug"


"$(INTDIR)\usr_code.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

