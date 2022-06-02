@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

REM vola target: 10% ==> riskFactor: 0.005759

%~dp0\sim\bin\msvc-14.0\release\NVecho-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a NVecho ^
-t 20000101, ^
-p etc\pf-mlp-NVecho-prod.tsv ^
--px-precision 0.0000001 ^
--strat.farPeriod 40 ^
--strat.riskFactor 0.0025 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 95 ^
--strat.ind2LBPct 95 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 46 ^
--strat.tickMult 1.5  ^
--strat.alpha 1 ^
--strat.beta 1 ^
--strat.refAtrPeriod 250 ^
--debug 0 ^
%equity%

