# Microsoft Developer Studio Project File - Name="Sequence" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Sequence - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sequence.mak" CFG="Sequence - Win32 Debug"
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
# ADD CPP /nologo /MT /W3 /GX /I "." /I "h" /I "k:\h" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "CIM_SIMULATION" /D "_WIN32_DCOM" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqcon.lib cimsqconIO.lib kutlstr.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"..\Sequence.exe" /libpath:"k:\lib" /libpath:"."

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
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /I "." /I ".." /I "h" /I "k:\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FD /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib cimsqcon.lib cimsqconIO.lib kutlstr.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\__Sequence.exe" /pdbtype:sept /libpath:"k:\lib"

!ENDIF 

# Begin Target

# Name "Sequence - Win32 Release"
# Name "Sequence - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\Adjust_Wnd.c
# End Source File
# Begin Source File

SOURCE=.\CVD_Rcp.c
# End Source File
# Begin Source File

SOURCE=.\GUI_FOR_BMP.c
# End Source File
# Begin Source File

SOURCE=.\interface.c
# End Source File
# Begin Source File

SOURCE=.\Ledapi.c
# End Source File
# Begin Source File

SOURCE=.\prg_code.c
# End Source File
# Begin Source File

SOURCE=.\Recipe_code.c
# End Source File
# Begin Source File

SOURCE=.\Recipe_File.c
# End Source File
# Begin Source File

SOURCE=.\Recipe_Ini.c
# End Source File
# Begin Source File

SOURCE=.\SEED_KISA.c
# End Source File
# Begin Source File

SOURCE=.\SubItem2_rcp.c
# End Source File
# Begin Source File

SOURCE=.\SubItem_rcp.c
# End Source File
# Begin Source File

SOURCE=.\TextParser_C.c
# End Source File
# Begin Source File

SOURCE=.\usr_code.c
# End Source File
# Begin Source File

SOURCE=.\Valv_Ctrl.c
# End Source File
# Begin Source File

SOURCE=.\Valv_Detail.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\CVD_Rcp.h
# End Source File
# Begin Source File

SOURCE=.\Data_Type.h
# End Source File
# Begin Source File

SOURCE=.\ExcelLib.h
# End Source File
# Begin Source File

SOURCE=.\File.h
# End Source File
# Begin Source File

SOURCE=.\GroupAuthority.h
# End Source File
# Begin Source File

SOURCE=.\IniFile_Param.h
# End Source File
# Begin Source File

SOURCE=.\interface.h
# End Source File
# Begin Source File

SOURCE=.\iodefine.h
# End Source File
# Begin Source File

SOURCE=.\Ledapi.h
# End Source File
# Begin Source File

SOURCE=.\LotType.h
# End Source File
# Begin Source File

SOURCE=.\Prg_Param.h
# End Source File
# Begin Source File

SOURCE=.\Read_Ini.h
# End Source File
# Begin Source File

SOURCE=.\Recipe_Code.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SEED_KISA.h
# End Source File
# Begin Source File

SOURCE=.\TextParser_C.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\BACKGROUND.bmp
# End Source File
# Begin Source File

SOURCE=.\Background_Gasline.bmp
# End Source File
# Begin Source File

SOURCE=.\Background_Heater.bmp
# End Source File
# Begin Source File

SOURCE=K:\H\BACKGROUND_Heater.bmp
# End Source File
# Begin Source File

SOURCE=K:\H\Background_Val_Det.bmp
# End Source File
# Begin Source File

SOURCE=K:\H\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=K:\H\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=K:\H\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\Cancel_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Cancel_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Cancel_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\cancel_file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\cancel_file_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\cancel_file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Cancel_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Copy_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Copy_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Copy_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Edit_file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Edit_file_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Edit_file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Enable_Arrow_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Enable_Arrow_Up.bmp
# End Source File
# Begin Source File

SOURCE=.\file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\file_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Group.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Group.bmp
# End Source File
# Begin Source File

SOURCE=.\Import_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Import_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Import_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\insert2_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\insert2_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\insert2_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\insert_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\insert_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\insert_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=K:\h\KEY_BUTTON.ico
# End Source File
# Begin Source File

SOURCE=K:\h\left_dis.bmp
# End Source File
# Begin Source File

SOURCE=.\Left_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\left_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\left_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\New_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\New_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\New_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\ok_file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\ok_file_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\ok_file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Pressed_Arrow_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Pressed_Arrow_Down.bmp
# End Source File
# Begin Source File

SOURCE=.\Pressed_Arrow_Up.bmp
# End Source File
# Begin Source File

SOURCE=K:\h\Program_GUI.ico
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=.\right_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\right_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\right_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Saveas_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Saveas_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Sub_BACKGROUND.bmp
# End Source File
# End Group
# Begin Group "Lib"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=.\Lib\kutltime.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutletc.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutlfile.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutlgui.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutlgui2.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutlstr.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\kutlanal.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\Resource\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Cancel_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Cancel_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Copy_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Copy_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Copy_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\FEdit_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\FEdit_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\FEdit_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\file_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert2_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert2_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert2_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\insert_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Left_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Left_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Left_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\New_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\New_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\ok_file_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\ok_file_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\remove_disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\remove_enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\remove_pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Right_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Right_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Right_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Save_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Save_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Save_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Saveas_Disable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Saveas_Enable.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Saveas_Pressed.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\Sub2_BACKGROUND.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\valve_background.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource\valvestep_background.bmp
# End Source File
# End Target
# End Project
