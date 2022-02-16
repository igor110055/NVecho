@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

REM vola target: 10% ==> riskFactor: 0.00349

for %%A in (0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.1 1.2) do (
for %%B in (0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.1 1.2) do (
C:\home\joshxli\prod\strat\Vecho\sim\bin\msvc-14.0\release\Vecho-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a Vecho_loop_alpha_beta ^
-p ..\univ\live\pf-mlp-Vecho-opt-prod.tsv ^
--px-precision 0.0000001 ^
--strat.riskFactor=0.00349 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 90 ^
--strat.ind2LBPct 90 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 60 ^
--strat.tickMult 1.5  ^
--strat.alpha %%A ^
--strat.beta %%B ^
--strat.refAtrPeriod 250 ^
--debug 0 ^
%equity%
))
