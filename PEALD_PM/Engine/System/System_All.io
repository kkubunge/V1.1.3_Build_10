#

$$$ Enum
AlarmSts      Absent Present
S_Msglog      No     Yes
S_IODrvLoad   Unload  Load
S_IntlksCtrl  All_Enable  SEna_VDis  SDis_VEna  All_Disable
S_DisconCon   DisConnect  Connect  ConUse  ConDis  ConDisHW
S_FunctnSts   Available	Running  Aborted  Success  Error  Killed  Unknown
S_AlmRunMode  Clear Retry Wait Ignore Disable Enable Stop Abort Paused Clear_nP Retry_nP Wait_nP Ignore_nP Disable_nP Enable_nP Stop_nP Abort_nP Unknown
S_AlmRunCtl2  Enable Disable DisablePost
#
$$$ Digital
#                    IN                                  | OUT
#------------------- --------- --------  -- -- -- -- --  --------- -- -- -- --
AlarmStatus          AlarmSts     SYSTEM     0  -  -  -  1
#Mdl_AlarmStatus     AlarmSts     SYSTEM     0  0  100 200  1    # From 100 to 200
#Mdl_AlarmStatus     AlarmSts     SYSTEM   100  0  100 200  1    # From 100 to 200(Just Level 0)
#Mdl_AlarmStatus     AlarmSts     SYSTEM   101  0  100 200  1    # From 100 to 200(Just Level 1)
#Mdl_AlarmStatus     AlarmSts     SYSTEM   109  0  100 200  1    # From 100 to 200(Just Level 9)
#Mdl_AlarmStatus     AlarmSts     SYSTEM     0  1  100 200  1    # Except (From 100 to 200)
#Mdl_AlarmStatus     AlarmSts     SYSTEM   100  1  100 200  1    # Except (From 100 to 200)(Just Level 0)
#Mdl_AlarmStatus     AlarmSts     SYSTEM   101  1  100 200  1    # Except (From 100 to 200)(Just Level 1)
#Mdl_AlarmStatus     AlarmSts     SYSTEM   109  1  100 200  1    # Except (From 100 to 200)(Just Level 9)
#AlarmHappenSts      AlarmSts     SYSTEM     0  2    -   -  1  SYSTEM     0  2  -  -

Sys.Year             -            SYSTEM     1  1  -  -  1
Sys.Month            -            SYSTEM     1  2  -  -  1
Sys.Day              -            SYSTEM     1  3  -  -  1
Sys.Hour             -            SYSTEM     1  4  -  -  1
Sys.Min              -            SYSTEM     1  5  -  -  1
Sys.Sec              -            SYSTEM     1  6  -  -  1

Message.log          S_Msglog     SYSTEM     2  0  -  -  1  SYSTEM    2  0  -  -
Event.log            S_Msglog     SYSTEM     2  1  -  -  1  SYSTEM    2  1  -  -
IO_Driver_Loding     S_IODrvLoad  SYSTEM     2  2  -  -  1

CurUser_Level        -            SYSTEM     3  0  -  -  1
Gui_BottomBar        -            SYSTEM     3  1  -  -  1
Gui_RightBar         -            SYSTEM     3  2  -  -  1
Gui_NavBar           -            SYSTEM     3  3  -  -  1

IO_Driver_Use_Count  -            SYSTEM     4  0  0  -  1
IO_Digital_Use_Count -            SYSTEM     4  0  1  -  1
IO_Analog_Use_Count  -            SYSTEM     4  0  2  -  1
IO_String_Use_Count  -            SYSTEM     4  0  3  -  1
IO_Enum_Use_Count    -            SYSTEM     4  0  4  -  1
IO_LookUp_Use_Count  -            SYSTEM     4  0  5  -  1

GRD_Driver_Use_Count -            SYSTEM     4  1  0  -  1
GRD_IO_Use_Count     -            SYSTEM     4  1  1  -  1

FUNCTION_Use_Count   -            SYSTEM     4  2  0  -  1
FUNCTION_List_Use    -            SYSTEM     4  2  1  -  1

INTLKS_Value_Use     -            SYSTEM     4  3  0  -  1
INTLKS_Command_Use   -            SYSTEM     4  3  1  -  1
INTLKS_Condition_Use -            SYSTEM     4  3  2  -  1

INTLKS_CONTROL       S_IntlksCtrl SYSTEM     5  -  -  -  1  SYSTEM    5  -  -  -

ControlPRO_Connect   S_DisconCon  SYSTEM     6  0  -  -  1
ControlPRO_ORDB_Con  S_DisconCon  SYSTEM     6  1  -  -  1
ControlPRO_IOBUS_Con S_DisconCon  SYSTEM     6  2  -  -  1
ControlPRO_EBUS_Con  S_DisconCon  SYSTEM     6  3  -  -  1
ControlPRO_ALARM_Con S_DisconCon  SYSTEM     6  4  -  -  1

EasyCluster_Connect  S_DisconCon  SYSTEM     8  0  -  -  1
EasyCluster_IOMon    -            SYSTEM     8  3  -  -  1

Alarm_100_Style      S_AlmRunMode SYSTEM     7  0  100 -  1  SYSTEM  7  0  100  -
Alarm_100_Status     S_AlmRunMode SYSTEM     7  1  100 -  1
Alarm_100_Style2     S_AlmRunCtl2 -          -  -  -   -  1  SYSTEM  7  2  100 100

#Function_Status     S_FunctnSts  SYSTEM     -  -  -  -  1
#s:Function_Status   S_FunctnSts  SYSTEM     -  -  -  -  1
#e:Fnc_Event_Status  S_FunctnSts  SYSTEM     -  -  -  -  1
#v:Fnc_Event_Status  S_FunctnSts  SYSTEM     -  -  -  -  1

#$$$ String
#Function_Run         -   -   1   SYSTEM   -
#e:Function_Event_Run -   -   1   SYSTEM   -

$$$ Analog

#Disk_Free_Space_Run    %	SYSTEM  0 -  -  1  0    99999   0
#Disk_Free_SpaceC       %	SYSTEM  0 2  -  1  0    99999   0
#Disk_Free_SpaceD       %	SYSTEM  0 3  -  1  0    99999   0
#Disk_Free_SpaceE       %	SYSTEM  0 4  -  1  0    99999   0
