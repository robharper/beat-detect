

#ifndef __BDUTILS_H__
#define __BDUTILS_H__

// BeatDetect Parameters
typedef struct
{
    // Onset Detection:
    //   Onset detection minimum onset distance
    //   Onset detection threshold top and bottom levels for hysteresis
    FLOAT   flOnsetDetectResetTime;
    FLOAT   flOnsetDetectThreshLow, flOnsetDetectThreshHigh;

    // Onset Detection and Assembly:
    //   Min onset distance
    //   Minimum threshold for onset output
    FLOAT   flOnsetCombineMinDist;
    FLOAT   flOnsetCombineMinOnset;

    // Onset Stream Output Sampling Rate
    INT32   nOnsetSamplingRate;

    // Maximum difference in node periods that consitutes an identical node
    FLOAT   flNodeMaxDiff;
    
    // Variable Sampler
    //   Starting Sample Period
    //   Ratio of previous error vs current error for input to PD Controller
    //   Proportional and Differential Gains for the PD Controller
    BOOL    fEnableVarSampler;
    FLOAT   flVarSamplerStartPeriod;
    FLOAT   flVarSamplerMaxErrorTime;
    FLOAT   flExpectationStdDevSamples;
    FLOAT   flExpectationDeviancePercent;
    FLOAT   flVarSamplerGainProp, flVarSamplerGainDiff;
    //   Fuzzy onset triangle distribution width
    FLOAT   flFuzzyOnsetWidth;

    // Timing Nets:
    //   Loop initial/min value
    //   Loop max value
    FLOAT   flLoopInitValue;
    FLOAT   flLoopMaxValue;

    // CSN:
    //   Alpha and Beta constants for the CSN linkage parameters
    //   CSN decay rate
    //   Minimum and maximum activation allowed
    //   Threshold multiplier for determining the top output from the CSN - Option 1
    //   Threshold in seconds for how long a loop must have max CSN output to be selected - Option 2
    FLOAT   flCSNAlpha;
    FLOAT   flCSNMinLink, flCSNMaxLink;
    FLOAT   flCSNInputLink;
    FLOAT   flCSNDecay;
    FLOAT   flCSNMinAct, flCSNMaxAct;
    FLOAT   flCSNOutputTimeThresh;  // Option 2

    // Beat Detection Logic
    //   Threshold percentage between mean and max loop value to output beats (Option 1)
    FLOAT   flBeatOutputMinThresh;

    // IOI Statistics Collector: 
    //   max onset tracking time, histogram halflife
    //   dominant IOI list threshold low and high
    FLOAT   flIOIMaxOnsetTime, flIOIHistHalflife;
    FLOAT   flIOIDomThreshRatio;

    // Performance Measures
    BOOL    fTrackPerformance;
    FLOAT   flTrackBeginOffset;
    INT32   nTrackChangeNode;

} BDParamsType;

extern BDParamsType  g_BDParams;

HRESULT InitializeSettings();


// Helper Macros
#define BOUND(x,l,u)    min(max(x,l),u)




#endif