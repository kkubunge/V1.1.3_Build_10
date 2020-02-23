#

$$$ Enum
AlarmSts      Absent Present
S_Msglog      No     Yes
S_IODrvLoad   Unload  Load
S_IntlksCtrl  All_Enable  SEna_VDis  SDis_VEna  All_Disable
#
$$$ Digital
#                    IN                                  | OUT
#------------------- --------- --------  -- -- -- -- --  --------- -- -- -- --
AlarmStatus          AlarmSts     SYSTEM     0  -  -  -  1

Sys.Year             -            SYSTEM     1  1  -  -  1
Sys.Month            -            SYSTEM     1  2  -  -  1
Sys.Day              -            SYSTEM     1  3  -  -  1
Sys.Hour             -            SYSTEM     1  4  -  -  1
Sys.Min              -            SYSTEM     1  5  -  -  1
Sys.Sec              -            SYSTEM     1  6  -  -  1

Message.log          S_Msglog     SYSTEM     2  0  -  -  1  SYSTEM    2  0  -  -
Event.log            S_Msglog     SYSTEM     2  1  -  -  1  SYSTEM    2  1  -  -
IO_Driver_Loding     S_IODrvLoad  SYSTEM     2  2  -  -  1

INTLKS_CONTROL       S_IntlksCtrl SYSTEM     5  -  -  -  1  SYSTEM    5  -  -  -

