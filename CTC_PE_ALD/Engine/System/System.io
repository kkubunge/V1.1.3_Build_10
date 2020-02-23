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
AlarmStatus          AlarmSts    SYSTEM     0  -  -  -  1
AlarmHappenSts       AlarmSts    SYSTEM     0  2  -  -  1  SYSTEM    0  2  -  -

Sys.Year             -           SYSTEM     1  1  -  -  1
Sys.Month            -           SYSTEM     1  2  -  -  1
Sys.Day              -           SYSTEM     1  3  -  -  1
Sys.Hour             -           SYSTEM     1  4  -  -  1
Sys.Min              -           SYSTEM     1  5  -  -  1
Sys.Sec              -           SYSTEM     1  6  -  -  1

Message.log          S_Msglog    SYSTEM     2  0  -  -  1  SYSTEM    2  0  -  -
Event.log            S_Msglog    SYSTEM     2  1  -  -  1  SYSTEM    2  1  -  -
IO_Driver_Loding     S_IODrvLoad SYSTEM     2  2  -  -  1

INTLKS_CONTROL       S_IntlksCtrl SYSTEM    5  -  -  -  1  SYSTEM    5  -  -  -

ControlPRO_Connect   S_DisconCon SYSTEM     6  0  -  -  1

EasyCluster_Connect  S_DisconCon SYSTEM     8  0  -  -  1
EasyCluster_IOMon    -           SYSTEM     8  3  -  -  1

#Function_Status     S_FunctnSts SYSTEM     -  -  -  -  1

