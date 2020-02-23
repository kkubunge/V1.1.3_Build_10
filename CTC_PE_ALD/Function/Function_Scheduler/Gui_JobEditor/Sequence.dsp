# Microsoft Developer Studio Project File - Name="Sequence" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Sequence - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak" CFG="Sequence - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sequence - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release2" (based on "Win32 (x86) Console Application")
!MESSAGE "Sequence - Win32 Release3" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Sequence - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "P:\CONTROL.PRO\include" /I "." /I ".\H" /I "k:\H" /I "P:\CONTROL.PRO\95" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D ENV_STD=ENV_STD_NT386 /D ENV_RPKERNEL_AVAILABLE=FALSE /D "_X86_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\Gui_JobEditor.exe" /libpath:"k:\lib" /libpath:"p:\Control.PRO\LIB\NT386\nodebug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Sequence___Win32_Release2"
# PROP BASE Intermediate_Dir "Sequence___Win32_Release2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release2"
# PROP Intermediate_Dir "Release2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /I "P:\CONTROL.PRO\include" /I "." /I ".\H" /I "k:\H" /I "P:\CONTROL.PRO\95" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D ENV_STD=ENV_STD_NT386 /D ENV_RPKERNEL_AVAILABLE=FALSE /D "_X86_" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /I "P:\CONTROL.PRO\include" /I "." /I ".\H" /I "k:\H" /I "P:\CONTROL.PRO\95" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D ENV_STD=ENV_STD_NT386 /D ENV_RPKERNEL_AVAILABLE=FALSE /D "_X86_" /D "X_160_SIZE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\Gui_JobEditor.exe" /libpath:"k:\lib" /libpath:"p:\Control.PRO\LIB\NT386\nodebug"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\Gui_JobEditor2.exe" /libpath:"k:\lib" /libpath:"p:\Control.PRO\LIB\NT386\nodebug"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Sequence___Win32_Release3"
# PROP BASE Intermediate_Dir "Sequence___Win32_Release3"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release3"
# PROP Intermediate_Dir "Release3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /I "P:\CONTROL.PRO\include" /I "." /I ".\H" /I "k:\H" /I "P:\CONTROL.PRO\95" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D ENV_STD=ENV_STD_NT386 /D ENV_RPKERNEL_AVAILABLE=FALSE /D "_X86_" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /GX /I "P:\CONTROL.PRO\include" /I "." /I ".\H" /I "k:\H" /I "P:\CONTROL.PRO\95" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D ENV_STD=ENV_STD_NT386 /D ENV_RPKERNEL_AVAILABLE=FALSE /D "_X86_" /D "X_200_SIZE" /D "DATA_EXPAND1" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\Gui_JobEditor.exe" /libpath:"k:\lib" /libpath:"p:\Control.PRO\LIB\NT386\nodebug"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"k:\exec\Gui_JobEditor3.exe" /libpath:"k:\lib" /libpath:"p:\Control.PRO\LIB\NT386\nodebug"

!ENDIF 

# Begin Target

# Name "Sequence - Win32 Release"
# Name "Sequence - Win32 Release2"
# Name "Sequence - Win32 Release3"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Source\AppCTC.c
# End Source File
# Begin Source File

SOURCE=.\Source\AppInit.c
# End Source File
# Begin Source File

SOURCE=.\Source\AppPlotting.c
# End Source File
# Begin Source File

SOURCE=.\Source\Appprm.c
# End Source File
# Begin Source File

SOURCE=.\Source\Extern_view.c
# End Source File
# Begin Source File

SOURCE=.\Source\Job_code.c
# End Source File
# Begin Source File

SOURCE=.\Source\Ledapi.c
# End Source File
# Begin Source File

SOURCE=.\Source\logview.c
# End Source File
# Begin Source File

SOURCE=.\Source\logviewT.c
# End Source File
# Begin Source File

SOURCE=.\Source\prg_code.c
# PROP Exclude_From_Scan -1

!IF  "$(CFG)" == "Sequence - Win32 Release"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Source\StepView.c
# End Source File
# Begin Source File

SOURCE=.\Source\usr_code.c
# PROP Exclude_From_Scan -1

!IF  "$(CFG)" == "Sequence - Win32 Release"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\H\AppCTC.h
# End Source File
# Begin Source File

SOURCE=.\H\Appinit.h
# End Source File
# Begin Source File

SOURCE=.\H\AppPlotting.h
# End Source File
# Begin Source File

SOURCE=.\H\Appprm.h
# End Source File
# Begin Source File

SOURCE=.\H\iodefine.h
# PROP Exclude_From_Scan -1

!IF  "$(CFG)" == "Sequence - Win32 Release"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release2"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Release3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\H\Jobdata.h
# End Source File
# Begin Source File

SOURCE=.\H\Ledapi.h
# End Source File
# Begin Source File

SOURCE=.\H\logview.h
# End Source File
# Begin Source File

SOURCE=.\H\LotType.h
# End Source File
# Begin Source File

SOURCE=.\H\StepView.h
# End Source File
# End Group
# Begin Group "Kornic Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=K:\lib\kutlAnal.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlEtc.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlFile.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlgui.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlgui2.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlStr.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlTime.lib
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\background.bmp
# End Source File
# Begin Source File

SOURCE=.\icon_bitmap.bmp
# End Source File
# Begin Source File

SOURCE=.\icon_Y16.bmp
# End Source File
# Begin Source File

SOURCE=K:\h\KEY_BUTTON.ico
# End Source File
# Begin Source File

SOURCE=.\New_Disable.bmp
# End Source File
# Begin Source File

SOURCE=K:\h\Program_GUI.ico
# End Source File
# End Target
# End Project
