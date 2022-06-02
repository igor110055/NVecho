@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

REM vola target: 10% ==> riskFactor: 0.00349

C:\home\joshxli\prod\strat\NVecho\sim\bin\msvc-14.0\release\NVecho-sim.exe ^
-c etc\prod-BBAT-lastAsClose.cfg ^
-a NVecho ^
-p ..\univ\live\pf-mlp-NVecho-opt-prod.tsv ^
--px-precision 0.0000001 ^
--strat.riskFactor=0.00349 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 90 ^
--strat.ind2LBPct 90 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 60 ^
--strat.tickMult 1.5  ^
--strat.alpha 0 ^
--strat.beta 0 ^
--strat.refAtrPeriod 250 ^
--debug 1 ^
%equity%
