@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\prod\strat\Vecho\sim\bin\msvc-14.0\release\Vecho-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a VechoProdSim ^
-p ..\univ\live\pf-mlp-Vecho-opt-prod.tsv ^
--px-precision 0.0000001 ^
--strat.riskFactor=0.005 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 90 ^
--strat.ind2LBPct 90 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 60 ^
--strat.tickMult 1.5  ^
--debug 1 ^
%equity%

