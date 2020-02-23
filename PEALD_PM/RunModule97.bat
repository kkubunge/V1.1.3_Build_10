@echo off
Engine\Exec\drvkill
md Datalog
md Datalog\SIDE1
md Datalog\SIDE2
md _Log
md _Log\EnetIP
md _Log\EventLog
md _Log\IOMonitor
set path=%path%;Engine\Cimsdll
Engine\Exec\Cims97_h Startup.cfg	/hd
Net time \\192.168.10.200 /set /y
pause
