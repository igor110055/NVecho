@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

REM vola target: 10% ==> riskFactor: 0.005759

for %%A in (46) do (
%~dp0\sim\bin\msvc-14.0\release\Vecho-sim.exe ^
-c etc\sim-BBAT.cfg ^
-a Vecho_loopOnTrigAtrPeriod_belowTopAndAboveBottom ^
-p ..\univ\live\pf-mlp-Vecho-opt-prod.tsv ^
--px-precision 0.0000001 ^
--strat.farPeriod 40 ^
--strat.riskFactor 0.005759 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 90 ^
--strat.ind2LBPct 90 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod %%A ^
--strat.tickMult 1.5  ^
--strat.alpha 0 ^
--strat.beta 0 ^
--strat.refAtrPeriod 250 ^
--debug 0 ^
%equity%
)
