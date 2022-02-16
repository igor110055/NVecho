@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\prod\strat\Vecho\sim\bin\msvc-14.0\release\Vecho-sim.exe -c etc\sim-BBAT.cfg -a VechoSim -p ..\univ\pf-mlp-super-sim.tsv  --strat.riskFactor=0.005 --strat.pctPeriod 250 --strat.ind1LBPct 90 --strat.ind2LBPct 90 --strat.allocAtrPeriod 250 --strat.trigAtrPeriod 60 --strat.tickMult 1.5  --debug 0 %equity%

