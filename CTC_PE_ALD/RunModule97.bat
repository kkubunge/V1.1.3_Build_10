@echo off
WaitKDrive.exe
Engine\Exec\drvkill
subst F: D:\DataLog
md F:\$$IOMonitorLog
md F:\$$IOMonitorLog\PM1
md F:\$$IOMonitorLog\PM2
md F:\$$IOMonitorLog\PM3
md F:\Plotter
md F:\Plotter\PM1
md F:\Plotter\PM2
md F:\Plotter\PM3
md F:\Plotter\TM
md F:\DataLog
md F:\DataLog\FFU
md F:\DataLog\VAC1
md F:\Datalog\_ManualLog\PM1
md F:\Datalog\_ManualLog\PM2
md F:\Datalog\_ManualLog\PM3
md F:\Datalog\_Log
md F:\Datalog\_Log\AWCRANGE
md F:\Datalog\_Log\AWCSCAN
md F:\Datalog\_Log\EventLog
md F:\Datalog\_Log\IOMonitor
md F:\Datalog\_Log\MCC
md F:\Datalog\_Log\PIOViewer
md F:\MARS\Logs\EventLog
md F:\MARS\PartsLog\PM1
md F:\MARS\PartsLog\PM2
md F:\MARS\PartsLog\PM3
md History
md Datalog
set path=%path%;Engine\Cimsdll
Engine\Exec\Cims97_h2 startup.cfg	/hd	
pause