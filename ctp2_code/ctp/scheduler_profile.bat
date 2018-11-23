PREP /OM /FT /SF ?GEVHookCallback@CtpAi_BeginSchedulerEvent@@EAE?AW4GAME_EVENT_HOOK_DISPOSITION@@W4GAME_EVENT@@PAVGameEventArgList@@@Z /EXCALL /INC Scheduler.obj /INC CtpAi.obj /INC CtpGoal.obj /INC CtpAgent.obj /INC Plan.obj /INC RobotAstar2.obj %1
if errorlevel == 1 goto done 
PROFILE %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel == 1 goto done 
PREP /M %1
if errorlevel == 1 goto done 
PLIST /FLAT /STC %1 >%1.lst
:done
