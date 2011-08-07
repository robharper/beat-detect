
#include "stdafx.h"
#include "BDUtils.h"
#include <math.h>



// Parameters
BDParamsType  g_BDParams;


HRESULT InitializeSettings()
{
    // Onset Detection:
    //   Onset detection minimum onset distance
    //   Onset detection threshold top and bottom levels for hysteresis
    g_BDParams.flOnsetDetectResetTime = 0.1f;        // 100 ms
    g_BDParams.flOnsetDetectThreshHigh = 0.06f;      // 0.06 is From Sepannen
    g_BDParams.flOnsetDetectThreshLow = -0.035f;     // From Sepannen

    g_BDParams.nOnsetSamplingRate = 441;

    // Onset Detection and Assembly:
    //   Min onset distance
    g_BDParams.flOnsetCombineMinDist = 0.05f;         // 50 ms
    g_BDParams.flOnsetCombineMinOnset = 0.1f;

    // Variable Sampler
    //   Starting Sample Period
    g_BDParams.fEnableVarSampler = TRUE;
    g_BDParams.flVarSamplerStartPeriod = 0.02f;      // 50 Hz
    g_BDParams.flVarSamplerMaxErrorTime = 0.06f;     // DETERMINE EXPERIMENTALLY: Max jitter offset = ~3 samples
    g_BDParams.flExpectationStdDevSamples = 2;       // From Cemgil et al
    g_BDParams.flExpectationDeviancePercent = 0.08f; // From Krumhansl, 2000
    g_BDParams.flVarSamplerGainProp = 1.0f;          // Proportional Gain
    g_BDParams.flVarSamplerGainDiff = 1.0f;          // Differential Gain
    //  Fuzzy onset triangle width
    g_BDParams.flFuzzyOnsetWidth = g_BDParams.flVarSamplerStartPeriod;

    // Maximum difference in node periods that consitutes an identical node
    g_BDParams.flNodeMaxDiff = 0.01f;
    
    // Timing Nets:
    //   Loop alpha, beta, time constant
    //   Loop integrator alpha, beta
    g_BDParams.flLoopInitValue = (FLOAT)pow(2, -5);
    g_BDParams.flLoopMaxValue = 1 - g_BDParams.flLoopInitValue;

    // CSN within Timing Nets
    g_BDParams.flCSNAlpha   = 5.0f;
    g_BDParams.flCSNMinLink = -0.03f;
    g_BDParams.flCSNMaxLink = 0.04f;
    g_BDParams.flCSNInputLink = 0.2f;
    g_BDParams.flCSNDecay   = 0.8f;
    g_BDParams.flCSNMinAct  = -1;
    g_BDParams.flCSNMaxAct  = 1;
    g_BDParams.flCSNOutputTimeThresh = 1.0f;        // One second as best required to be selected

    // Beat Detection Logic for within a loop
    // Threshold weight between max and loop mean
    // Minimum threshold allowed
    g_BDParams.flBeatOutputMinThresh  = 0.0f;       //

    // IOI Statistics Collector: 
    //   max onset tracking time, histogram halflife
    //   dominant IOI list threshold low and high
    g_BDParams.flIOIMaxOnsetTime = 1.2f;
    g_BDParams.flIOIHistHalflife = 2.0f;
    g_BDParams.flIOIDomThreshRatio = 0.25f;

    // Performance Measures
    //
    g_BDParams.fTrackPerformance = FALSE;
    g_BDParams.flTrackBeginOffset = 8.0f;
    g_BDParams.nTrackChangeNode = 0;

    return S_OK;
}

