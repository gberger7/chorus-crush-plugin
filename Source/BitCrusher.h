#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BitCrusher
{
public:
    BitCrusher();
    ~BitCrusher();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);

private:
    double sampleRate = 44100.0;
    
    // Bit reduction parameters
    float bitDepth = 16.0f;
    float sampleRateReduction = 1.0f;
    
    // Sample rate reduction state
    float holdSample[2] = {0.0f, 0.0f};
    int sampleCounter = 0;
    int downsampleFactor = 1;
    
    // Anti-aliasing filter
    juce::dsp::IIR::Filter<float> antiAliasFilter[2];
    
    // Soft clipping for musicality
    float softClip(float input);
    float quantize(float input, float bits);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitCrusher)
};