#include "NVecho-strat.hpp"

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <typeinfo>
#include <pirate/tr/Trader.hpp>
#include <cassert>
#include <pirate/tr/Trader.hpp>
#include <pirate/tr/cfg/MktDef.hpp>

using namespace std;
using pirate::ind::naValue;
using pirate::ind::TmPx;
using boost::gregorian::date;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using pirate::tr::data::OHLC;
using pirate::ind::naValue;
using pirate::ind::na;
using pirate::ind::EMACalc;
using pirate::tr::cfg::MktDef;

Strat::Strat(
    const pirate::ind::period_type aNearPeriod,
    const pirate::ind::period_type aFarPeriod,
    const pirate::ind::period_type aLongAaaPeriod, 
    const pirate::ind::period_type aShortAaaPeriod, 
    const pirate::ind::period_type aRicciPeriod,
    const pirate::ind::period_type aAllocAtrPeriod,
    //const pirate::ind::period_type aTrigAtrPeriod,
    const pirate::ind::period_type aRefAtrPeriod,
    const pirate::ind::period_type aRSIPeriod,
    const pirate::ind::period_type aPctPeriod, 

    const double aInd1LBPct,
    const double aInd1UBPct,
    const double aInd2LBPct,
    const double aInd2UBPct,
    const double aInd3LBPct,
    const double aInd3UBPct,
    const float aRiskFactor,
    const float aTickMult, 
    const float aTPFMult,
    const float aAlpha,
    const float aBeta
    )
    : pirate::tr::bar::BarStrat(),
      floatNA(naValue<float>()),
      doubleNA(naValue<double>()),

      hiStopLossCalc(aNearPeriod),
      hiFarCalc(aFarPeriod),
      loStopLossCalc(aNearPeriod),
      loFarCalc(aFarPeriod),

      longAaaUpCalc(EMACalc::Period(static_cast<unsigned int>(aLongAaaPeriod)), 0),
      longAaaDownCalc(EMACalc::Period(static_cast<unsigned int>(aLongAaaPeriod)), 0), 
      longAaaDiffCalc(EMACalc::Period(static_cast<unsigned int>(aLongAaaPeriod)), 0),
      longAaaAtrCalc(aLongAaaPeriod),

      shortAaaUpCalc(EMACalc::Period(static_cast<unsigned int>(aShortAaaPeriod)), 0),
      shortAaaDownCalc(EMACalc::Period(static_cast<unsigned int>(aShortAaaPeriod)), 0), 
      shortAaaDiffCalc(EMACalc::Period(static_cast<unsigned int>(aShortAaaPeriod)), 0),
      shortAaaAtrCalc(aShortAaaPeriod),
      
      ricciHiCalc(aRicciPeriod),
      ricciLoCalc(aRicciPeriod),
      ricciAtrCalc(aRicciPeriod), 

    volumeMACalc(4),
      rsiCalc(EMACalc::Period(static_cast<unsigned int>(aRSIPeriod)), 0),

      allocAtrCalc(aAllocAtrPeriod), 
      //trigAtrCalc(aTrigAtrPeriod), 
      refAtrCalc(aRefAtrPeriod), 

      ind1LBCalc(aPctPeriod, aInd1LBPct),
      ind1UBCalc(aPctPeriod, aInd1UBPct),
      ind2LBCalc(aPctPeriod, aInd2LBPct),
      ind2UBCalc(aPctPeriod, aInd2UBPct),
      ind3LBCalc(aPctPeriod, aInd3LBPct),
      ind3UBCalc(aPctPeriod, aInd3UBPct),

      hiStopLoss(),
      loStopLoss(),
      hiFar(),
      loFar(),

    tradeDirection(0),
    longMult(doubleNA),
    shortMult(doubleNA),
      longAaa(doubleNA),
      longAaaAtr(doubleNA),
      shortAaa(doubleNA),
      shortAaaAtr(doubleNA),
      ricch(),
      riccl(),
      ricca(doubleNA),

      rsi(doubleNA),
      allocAtr(doubleNA),
      //trigAtr(doubleNA),

      ind1LBPct(aInd1LBPct),
      ind1UBPct(aInd1UBPct),
      ind2LBPct(aInd2LBPct),
      ind2UBPct(aInd2UBPct),
      ind3LBPct(aInd3LBPct),
      ind3UBPct(aInd3UBPct),

      ind1Value(doubleNA),
      ind2Value(doubleNA),
      ind1LBValue(doubleNA),
      ind1UBValue(doubleNA),
      ind2LBValue(doubleNA),
      ind2UBValue(doubleNA),
      ind3LBValue(doubleNA),
      ind3UBValue(doubleNA),

      minTickSz(0),
      //slpgMult(aSlpgMult),
      riskFactor(aRiskFactor),
      tickMult(aTickMult),
      tpfMult(aTPFMult),
      alpha(aAlpha),
      beta(aBeta),
      DELTA(0),

      m_pxs(300),
      m_dtms(300),
      m_vols(300),
      m_prevClose(naValue<float>()),
      m_adjCtID(0),

      m_prevDtm(),
      m_adjDtm(),
   
      m_prevDt(0),
      m_adjDt(0),
      m_prevTm(0),
      m_adjTm(0)
{
    pxAdjHist.swap(*(new BarBuff(2)));
    pxRawHist.swap(*(new BarBuff(300)));
    hhHist.swap(*(new DoubleBuff(3)));
    llHist.swap(*(new DoubleBuff(3)));
    allocAtrHist.swap(*(new DoubleBuff(3)));
    rgHist.swap(*(new DoubleBuff(3)));
    refAtrHist.swap(*(new DoubleBuff(3)));
}

/* virtual */ void Strat::begin() {

    char *endptr;
    
    minTickSz = minTickSize();
    const MktDef& mktDef = m_traderPtr->mktDef();
    // settlement: first column after CtCnt
    MktDef::const_iterator mktDefIter = mktDef.beginArgs();
    const std::string& settleString = *mktDefIter;
    std::string::size_type delimPos = settleString.find(":");
    int hh = static_cast<int>(strtol(settleString.substr(0, delimPos).c_str(),
                                     &endptr, 10));
    char *endptr0 = endptr;
    int errno0 = errno;
    int mm = static_cast<int>(strtol(settleString.substr(delimPos+1,
                                                         std::string::npos).c_str(),
                                     &endptr, 10));
    if (*endptr == '\0' && *endptr0 == '\0' && errno == 0 && errno0 == 0) {
        settleTm = time_duration(hh, mm, 0);
        settleTmInt = hh*100 + mm;
    }

    
    mktDefIter += 12;
    // TradeDir: 12 columns after settle (13th after ctcnt)
    // we need to reserve and retain the order of colums before TradeDir column
    // for pirate.data.ibpofdata.py 
    /*/
    const std::string& tradeDirString = *mktDefIter;
    if (!std::all_of(tradeDirString.begin(), tradeDirString.end(), isspace)) {
        long td = strtol(tradeDirString.c_str(), &endptr, 10);
        if (*endptr == '\0' && errno == 0) tradeDirection = static_cast<int>(td);
    } 
    mktDefIter++    
    //*/

    // longMult
    /*/
    const std::string& longMultString = *mktDefIter;
    if (!std::all_of(longMultString.begin(), longMultString.end(), isspace)) {
        double lm = strtod(longMultString.c_str(), &endptr);
        if (*endptr == '\0' && errno == 0) longMult = lm;
    }
    mktDefIter++;
    //*/
    
    // shortMult
    /*/
    const std::string& shortMultString = *mktDefIter;
    if (!std::all_of(shortMultString.begin(), shortMultString.end(), isspace)) {
        double sm = strtod(shortMultString.c_str(), &endptr);
        if (*endptr == '\0' && errno == 0) shortMult = sm;
    }    
    //*/

    if (debugLevel > 0)
    {
        logHeader() 
            << sep << "TsDt"
            << sep << "spread"
            << sep << "adjOp"
            << sep << "adjHi"
            << sep << "adjLo"
            << sep << "adjCl"

            << sep << "longMult"
            << sep << "shortMult"
            << sep << "hiFar"
            << sep << "loFar"

            << sep << "longAaa"
            << sep << "longAaaAtr"
            << sep << "shortAaa"
            << sep << "shortAaaAtr"
            << sep << "ricch"
            << sep << "riccl"
            << sep << "ricca"

            << sep << "rsi"

            << sep << "allocAtr"
            << sep << "rg"
            << sep << "refAtr"
            
            << sep << "weight"
            << sep << "equity"
            << sep << "exchRt"
            << sep << "ptValue"
            << sep << "nc"
            << sep << "volCap"
            
            << sep << "ind1Value"
            << sep << "ind1LB"
            << sep << "ind1UB"
            << sep << "ind2LB"
            << sep << "ind2UB"
            << sep << "ind3LB"
            << sep << "ind3UB"

            << sep << "\n";
    }
}

/* virtual */ void Strat::calc() {

    //std::cout << "dtm = " << dtm << std::endl;

    // MktSim is still spooling in new ct data before the last time 
    // of the previous contract (rollover time).  We will not use 
    // these data to do anything and hence we just return. 
    if (dtm < m_adjDtm && !m_adjDtm.is_special())
    {
        hiStopLoss = hiStopLossCalc.calc(dtm, high);
        loStopLoss = loStopLossCalc.calc(dtm, low);

        //* calc on raw prices 
        hiFar = hiFarCalc.calc(dtm, high);
        loFar = loFarCalc.calc(dtm, low);
        hhHist.push_front(hiFar.px);
        llHist.push_front(loFar.px);
        //*/
        
        pxRawHist.push_front(OHLC(open, high, low, close));
        hhHist.push_front(hiFar.px);
        llHist.push_front(loFar.px);

        prevVolumeMA = volumeMA;
        if (volume > 0) {
            volumeMA = volumeMACalc.calc(volume);
        }
        return; 
    }
    // now, we have passed the rollover point and we will adjust the saved 
    // old contract data (rather than using current contract data before 
    // rollover time!) and use the adjusted data for indicator calculation. 
    // So, we need to recalculate all indicators. 
    
    //std::cout << "ctID = " << ctID << ", m_adjCtID = " << m_adjCtID 
    //          << ", m_adjDtm = " << to_iso_string(m_adjDtm)  
    //          << ", dtm = " << to_iso_string(dtm) << std::endl;

    if (ctID != m_adjCtID && !m_pxs.empty()) { 
        m_pxs.pop_back(); 
        m_dtms.pop_back();
        m_vols.pop_back();
        if (!m_pxs.empty())
        {
            const float clSpread = close - m_prevClose;
            BarBuff::iterator i = m_pxs.begin();
            BarBuff::iterator e = m_pxs.end();
            DtmsBuff::iterator j = m_dtms.begin();
            IntBuff::iterator k = m_vols.begin();
            for (; i != e; ++i, ++k, ++j) 
            {
                i->op += clSpread;
                i->cl += clSpread;
                i->hi += clSpread;
                i->lo += clSpread;
                /*
                std::cout << "... ctID = " << ctID << ", m_adjCtID = " << m_adjCtID 
                          << ", dtm = " << to_iso_string(*j) << std::endl;
                */
                doCalc(*j, i->op, i->hi, i->lo, i->cl, 
                       *k, clSpread, true);
            }
        }
        m_adjCtID = ctID;
    }
            
    hiStopLoss = hiStopLossCalc.calc(dtm, high);
    loStopLoss = loStopLossCalc.calc(dtm, low);

    // calc on adj OR raw
    // for raw, turn on calc in the beginning of this function
    // for adj, turn on calc in doCalc()
    hiFar = hiFarCalc.calc(dtm, high);
    loFar = loFarCalc.calc(dtm, low);
    hhHist.push_front(hiFar.px);
    llHist.push_front(loFar.px);
    
    pxRawHist.push_front(OHLC(open, high, low, close));

    prevVolumeMA = volumeMA;
    if (volume > 0) {
        volumeMA = volumeMACalc.calc(volume);
    }
    doCalc(dtm, open, high, low, close, volume, 0, false);

    m_pxs.push_back(OHLC(open, high, low, close));
    m_dtms.push_back(dtm);
    m_vols.push_back(volume);

    m_prevClose = close;
    m_prevDtm = dtm;
    m_prevDt = dt;
    m_prevTm = tm;
}

void 
Strat::doCalc(const boost::posix_time::ptime dt_tm,
              const float openPx,
              const float highPx,
              const float lowPx,
              const float closePx,
              const int vol, 
              const float spread,
              const bool inAdj)
{
    DELTA = minTickSz * tickMult;

    /*
    hiStopLoss = hiStopLossCalc.calc(dt_tm, highPx);
    loStopLoss = loStopLossCalc.calc(dt_tm, lowPx);
    //*/
    
    rsi = rsiCalc.calcSkipNA(closePx);
    allocAtr = allocAtrCalc.calc(openPx, highPx, lowPx, closePx);
    //trigAtr = trigAtrCalc.calc(openPx, highPx, lowPx, closePx);
    refAtr = refAtrCalc.calc(openPx, highPx, lowPx, closePx);
    allocAtrHist.push_front(allocAtr);
    rgHist.push_front(highPx - lowPx);
    refAtrHist.push_front(refAtr);
    longAaaAtr = longAaaAtrCalc.calc(openPx, highPx, lowPx, closePx);
    if (!na(longAaaAtr) && longAaaAtr != 0)
    {
        double upMove = highPx - pxAdjHist[0].hi;
        double downMove = pxAdjHist[0].lo - lowPx;
        double pDM = (upMove > downMove && upMove > 0) ? upMove : 0;
        double nDM = (downMove > upMove && downMove > 0) ? downMove : 0; 
        double pDI = longAaaUpCalc.calc(pDM / longAaaAtr);
        double nDI = longAaaDownCalc.calc(nDM / longAaaAtr);
        if (pDI + nDI != 0)
        {
            longAaa = longAaaDiffCalc.calc(std::fabs((pDI - nDI) / (pDI + nDI)));
        } else 
        {
            longAaa = doubleNA;
        }
    } else 
    {
        longAaa = doubleNA;
    }

    shortAaaAtr = shortAaaAtrCalc.calc(openPx, highPx, lowPx, closePx);
    if (!na(shortAaaAtr) && shortAaaAtr != 0)
    {
        double upMove = highPx - pxAdjHist[0].hi;
        double downMove = pxAdjHist[0].lo - lowPx;
        double pDM = (upMove > downMove && upMove > 0) ? upMove : 0;
        double nDM = (downMove > upMove && downMove > 0) ? downMove : 0; 
        double pDI = shortAaaUpCalc.calc(pDM / shortAaaAtr);
        double nDI = shortAaaDownCalc.calc(nDM / shortAaaAtr);
        if (pDI + nDI != 0)
        {
            shortAaa = shortAaaDiffCalc.calc(std::fabs((pDI - nDI) / (pDI + nDI)));
        } else 
        {
            shortAaa = doubleNA;
        }
    } else 
    {
        shortAaa = doubleNA;
    }

    ricch = ricciHiCalc.calc(dt_tm, highPx);
    riccl = ricciLoCalc.calc(dt_tm, lowPx);
    ricca = ricciAtrCalc.calc(openPx, highPx, lowPx, closePx); 

    if (!inAdj) {
        ind1Value = pk0(highPx, lowPx);
        ind2Value = gk0(openPx, highPx, lowPx, closePx);
    }
    
    if (!na(ind1Value) && !inAdj)
    {
        ind1LBValue = ind1LBCalc.calc(ind1Value);
    }
    if (!na(ind2Value) && !inAdj) {
        ind2LBValue = ind2LBCalc.calc(ind2Value);
    }

    pxAdjHist.push_front(OHLC(openPx, highPx, lowPx, closePx));

    /* calc on adjusted prices 
    hiFar = hiFarCalc.calc(dtm, highPx);  
    loFar = loFarCalc.calc(dtm, lowPx);
    hhHist.push_front(hiFar.px);
    llHist.push_front(loFar.px);
    //*/

    if (debugLevel > 0 && !inAdj)
    {
        logValues() 
            << sep << to_iso_string(dt_tm)
            << sep << spread
            << sep << openPx
            << sep << highPx
            << sep << lowPx
            << sep << closePx

            << sep << longMult
            << sep << shortMult
            << sep << hiFar.px
            << sep << loFar.px

            << sep << longAaa
            << sep << longAaaAtr

            << sep << shortAaa
            << sep << shortAaaAtr

            << sep << ricch.px
            << sep << riccl.px
            << sep << ricca

            << sep << rsi
            << sep << allocAtr
            << sep << highPx - lowPx
            << sep << refAtr
            << sep << weight()
            << sep << equity()
            << sep << exchRate
            << sep << pointValue()
            << sep << static_cast<unsigned int>(equity()*riskFactor*weight() / (allocAtrHist[0]*exchRate*pointValue()))
            << sep << static_cast<unsigned int>(floor(0.01*volumeMA)) 
            
            << sep << ind1Value
            << sep << ind1LBValue
            << sep << ind1UBValue
            << sep << ind2LBValue
            << sep << ind2UBValue
            << sep << ind3LBValue
            << sep << ind3UBValue

            << sep << "\n"; 
    }                     
}

/* virtual */ 
void
Strat::trade() 
{
    unsigned int nc = 
        static_cast<unsigned int>(equity()*riskFactor*weight() / (allocAtrHist[0]*exchRate*pointValue()));

    //std::cout << "dtm = " << dtm << std::endl;

    unsigned int volumeCap = static_cast<unsigned int>(floor(0.01 * (volumeMA)));

    //*
    if (volumeCap > 0 && nc > volumeCap) {
        nc = volumeCap;
    } else if (volumeCap == 0) {
        nc = 1;
    }
    
    //*/    

    if (nc <= 0)
    {
        nc = 1;
    }
    
    bool longCond = testEnterLongFilter();    
    bool shortCond = testEnterShortFilter();

    if (isLong())
    {
        unsigned long longSize = static_cast<unsigned long>(longPosSize());
        assert(longSize != 0);
        if (longSize != 0) 
        {
            exitLong("SX_MOC", static_cast<unsigned int>(longSize),
                     otMOC(), tifCurrBar());
        }
    } 

    if (shortCond) {
        sell("LE_VB", nc, otLMT(roundToTic(static_cast<float>(close + beta*rgHist[1]))));
    }

    if (isShort())
    {
        unsigned long shortSize = static_cast<unsigned long>(shortPosSize());
        assert(shortSize != 0);
        if (shortSize != 0)
        {
            exitShort("LX_MOC", static_cast<unsigned int>(shortSize),
                      otMOC(), tifCurrBar());
        }
    } 

    if (longCond) { 
        buy("LE_VB", nc, otLMT(roundToTic(static_cast<float>(close - alpha*rgHist[1]))));
    }
}


bool Strat::testEnterLongFilter()
{
    float buyLevel = roundToTic(static_cast<float>(close - alpha*rgHist[1]));
    bool ind1Cond =  
         (ind1LBPct <= 0 || ind1Value >= ind1LBValue);
    bool tradeDirCond = longMult > 0;
    bool aboveBottom = buyLevel >= loFar.px;

    /*
    std::cout << "testEnterLongFilter: "  
              << to_iso_string(dtm) << ": ind1Cond = " << ind1Cond
              << ", remoteBuy = " << remoteBuy 
              << ": longMult = " << longMult << ", rgHist[1] = " << rgHist[1] 
              << ", alpha = " << alpha << ", refAtrHist[1] = " << refAtrHist[1] 
              << std::endl; 
    */
    
    bool remoteBuy = alpha*rgHist[1] >= alpha*refAtrHist[1];
    bool retValue = ind1Cond; //tradeDirCond && ind1Cond && remoteBuy && aboveBottom;
    return retValue;    
}

bool Strat::testEnterShortFilter()
{
    float sellLevel = roundToTic(static_cast<float>(close + beta*rgHist[1]));
    bool ind2Cond =  
         (ind2LBPct <= 0 || ind2Value >= ind2LBValue);
    bool tradeDirCond = shortMult > 0;
    bool remoteSell = beta*rgHist[1] >= beta*refAtrHist[1];
    bool belowTop = sellLevel <= hiFar.px;
    
    bool retValue = ind2Cond; //tradeDirCond && ind2Cond && remoteSell && belowTop;
    return retValue;
}

/* virtual */ 
void Strat::rollCleanUp() {
    m_adjDtm = m_prevDtm;
    m_adjDt = m_prevDt;
    m_adjTm = m_prevTm;

    hiStopLoss = 
        loStopLoss = 
        hiFar = 
        loFar = 
        ricch = 
        riccl = TmPx();
    ricca = 
        longAaa = 
        longAaaAtr = 
        shortAaa = 
        shortAaaAtr =
        ind1Value = 
        ind2Value =
        ind3Value =
        ind1LBValue = 
        ind1UBValue = 
        ind2LBValue = 
        ind2UBValue = 
        ind3LBValue = 
        ind3UBValue = doubleNA;
    
    hiStopLossCalc.cleanup();
    loStopLossCalc.cleanup();
    hiFarCalc.cleanup();
    loFarCalc.cleanup();

    longAaaUpCalc.cleanup();
    longAaaDownCalc.cleanup();
    longAaaDiffCalc.cleanup();
    longAaaAtrCalc.cleanup();

    shortAaaUpCalc.cleanup();
    shortAaaDownCalc.cleanup();
    shortAaaDiffCalc.cleanup();
    shortAaaAtrCalc.cleanup();

    ricciHiCalc.cleanup();
    ricciLoCalc.cleanup();
    ricciAtrCalc.cleanup();
    
    rsiCalc.cleanup();
    allocAtrCalc.cleanup();
    //trigAtrCalc.cleanup();

    //ind1LBCalc.cleanup();
    //ind1UBCalc.cleanup();
    //ind2LBCalc.cleanup();
    //ind2UBCalc.cleanup();
    //ind3LBCalc.cleanup();
    //ind3UBCalc.cleanup();

    pxAdjHist.clear();
    pxRawHist.clear();
    hhHist.clear();
    llHist.clear();
    allocAtrHist.clear();
    //trigAtrHist.clear();
    refAtrHist.clear();
}

