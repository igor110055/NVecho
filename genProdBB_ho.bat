@echo off

REM set equity=
REM if "%1"=="" (
REM    set equity="" 
REM ) else (  
REM   set equity= -e c%1
REM )

REM vola target: 10% ==> riskFactor: 0.005759

%~dp0\sim\bin\msvc-14.0\release\NVecho-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a NVecho ^
-p ..\univ\live\pf-mlp-NVecho-opt-prod.tsv ^
--trade-range=20150101,%1 ^
--px-precision 0.0000001 ^
--strat.farPeriod 40 ^
--strat.riskFactor 0.005759 ^
--strat.pctPeriod 250 ^
--strat.ind1LBPct 90 ^
--strat.ind2LBPct 90 ^
--strat.allocAtrPeriod 250 ^
--strat.trigAtrPeriod 46 ^
--strat.tickMult 1.5  ^
--strat.alpha 0 ^
--strat.beta 0 ^
--strat.refAtrPeriod 250 ^
--debug 0 ^
%equity%

