for %%A in (1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 100) do for %%B in (0.6 0.7 0.8 0.9 1 1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2 2.2 2.4 2.6 2.8 3 3.5 4 4.5 5 5.5 6 6.5 7 7.5 8 8.5 9 9.5 10 100) do C:\home\joshxli\prod\strat\NVecho\sim\bin\msvc-14.0\release\NVecho-sim.exe -c etc\sim-SU.cfg -a NVechoTest_StpBeforeLmt --strat.ind2UBPct 30 --strat.riskFactor=0.007745 --strat.tpfMult %%B --strat.stopLossPeriod %%A --debug 0 

