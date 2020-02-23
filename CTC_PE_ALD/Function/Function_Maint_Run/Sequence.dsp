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
!MESSAGE "Sequence - Win32 Debug" (based on "Win32 (x86) Console Application")
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
# ADD CPP /nologo /MT /W3 /GX /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cimsqcon.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"sequence.exe" /libpath:"k:\lib"

!ELSEIF  "$(CFG)" == "Sequence - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cimsqcon.lib /nologo /subsystem:console /debug /machine:I386 /out:"dbg_sequence.exe" /pdbtype:sept /libpath:"k:\lib"

!ENDIF 

# Begin Target

# Name "Sequence - Win32 Release"
# Name "Sequence - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Source\BGChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Area.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FingerAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PRG_CODE.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\TextParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TransferArea.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TransferManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TransferOrder.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Source\Area.h
# End Source File
# Begin Source File

SOURCE=.\Source\FingerAction.h
# End Source File
# Begin Source File

SOURCE=.\H\iodefine.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Source\BGChecker.h
# End Source File
# Begin Source File

SOURCE=.\Source\MsgBoxDll.h
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Source\TextParser.h
# End Source File
# Begin Source File

SOURCE=.\Source\TransferArea.h
# End Source File
# Begin Source File

SOURCE=.\Source\TransferManager.h
# End Source File
# Begin Source File

SOURCE=.\Source\TransferOrder.h
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

SOURCE=K:\lib\kutlStr.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlTime.lib
# End Source File
# Begin Source File

SOURCE=K:\Lib\CimSqConIO.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=.\Source\StdAfx.cpp
# End Source File
# End Target
# End Project
