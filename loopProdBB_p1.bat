@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)
x
REM vola target: 10% ==> riskFactor: 0.00349

for %%A in (0.8 0.9 1 1.1 1.2 1.3) do (
for %%B in (0.8 0.9 1 1.1 1.2 1.3) do (
for %%C in (91 93 95 97 99) do (
for %%D in (91 93 95 97 99) do (
C:\home\joshxli\prod\strat\NVecho\sim\bin\msvc-14.0\release\NVecho-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a NVecho_super_loop_alpha_beta ^
-p etc\pf-mlp-super-sim.tsv ^
--px-precision 0.0000001 ^
--strat.riskFactor=0.0025 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct %%C ^
--strat.ind2LBPct %%D ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 60 ^
--strat.tickMult 1.5  ^
--strat.alpha %%A ^
--strat.beta %%B ^
--strat.refAtrPeriod 250 ^
--debug 0 ^
%equity%
))))
