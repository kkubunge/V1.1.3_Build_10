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
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
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
# ADD CPP /nologo /MT /W3 /GX /I "." /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" /D "_X86_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqconDll.lib cimsqconDllMain.lib CimSqConKNFS.lib ws2_32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"OBJ_Gui_Recipe_History.exe" /libpath:"k:\lib"
# Begin Target

# Name "Sequence - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Source\interface.c
# End Source File
# Begin Source File

SOURCE=.\Source\prg_code.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\usr_code.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\H\interface.h
# End Source File
# Begin Source File

SOURCE=.\H\iodefine.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
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

SOURCE=.\OperateLog.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\background.bmp
# End Source File
# Begin Source File

SOURCE=K:\h\KEY_BUTTON.ico
# End Source File
# Begin Source File

SOURCE=K:\h\Program_GUI.ico
# End Source File
# End Target
# End Project
