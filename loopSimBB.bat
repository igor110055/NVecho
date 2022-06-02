@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (0.4 0.45 0.5 0.55 0.6 0.65 0.7 0.75 0.8 0.85 0.9 0.95 1 1.1 1.15 1.2 1.25 1.3 1.35 1.4 1.45 1.5 1.55 1.6 1.65 1.7 1.75 1.8 1.85 1.9 1.95 2 2.5 3) do C:\home\joshxli\prod\strat\NVecho\sim\bin\msvc-14.0\release\NVecho-sim.exe -c etc\sim-BBAT.cfg -a NVechoSimLoopOnTickMultSince2012LongOnly -m %%A -p ..\univ\pf-mlp-super-sim.tsv --strat.riskFactor=0.005 --strat.pctPeriod 250 --strat.ind1LBPct 90 --strat.ind2LBPct 90 --strat.allocAtrPeriod 250 --strat.trigAtrPeriod 60 --strat.tickMult %%A  --debug 0 %equity%

