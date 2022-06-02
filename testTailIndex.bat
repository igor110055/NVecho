@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (10 20 30 40 50 60 70 80 90) do for %%B in (3 5 10 20 60 250) do C:\home\joshxli\research\strat\NVecho\sim\bin\msvc-14.0\release\NVecho-sim.exe -c etc\sim-SU.cfg -a TailIndexIntraLB --strat.riskFactor=0.007745 --strat.tpfMult 10 --strat.stopLossPeriod 3  --strat.ind1UBPct 100 --strat.ind2UBPct 100 --strat.tailIndexPeriod %%B --strat.ind3LBPct %%A --debug 0 %equity%

REM C:\home\joshxli\prod\strat\NVecho\sim\bin\msvc-14.0\debug\NVecho-sim_d.exe -c etc\sim-SU.cfg --strat.riskFactor=0.007745 --strat.tpfMult 1 %equity%