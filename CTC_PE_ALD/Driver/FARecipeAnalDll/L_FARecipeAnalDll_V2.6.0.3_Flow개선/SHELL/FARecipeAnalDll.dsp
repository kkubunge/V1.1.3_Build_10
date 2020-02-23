# Microsoft Developer Studio Project File - Name="FARecipeAnalDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FARecipeAnalDll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FARecipeAnalDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FARecipeAnalDll.mak" CFG="FARecipeAnalDll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FARecipeAnalDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /I "..\h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_X86_" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"..\..\FaRecipeAnal.dll" /libpath:"k:\lib"
# Begin Target

# Name "FARecipeAnalDll - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\SOURCE\CimsInterface.c
# End Source File
# Begin Source File

SOURCE=..\SOURCE\FaRecipeAnal.c
# End Source File
# Begin Source File

SOURCE=..\SOURCE\main.c
# End Source File
# Begin Source File

SOURCE=..\SOURCE\SharedMemory.c
# End Source File
# Begin Source File

SOURCE=..\SOURCE\TextParser_C.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\SOURCE\CimsInterface.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\FaRecipeAnal.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\iodefine.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\MsgBoxDll.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\SharedMemory.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\TextLogDll_M.h
# End Source File
# Begin Source File

SOURCE=..\SOURCE\TextParser_C.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\ResourceScript.rc
# End Source File
# End Group
# Begin Group "Kornic Library"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=K:\lib\kutlfile.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutletc.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlanal.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutlstr.lib
# End Source File
# Begin Source File

SOURCE=K:\lib\kutltime.lib
# End Source File
# End Group
# End Target
# End Project
