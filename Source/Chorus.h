#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Chorus
{
public:
    Chorus();
    ~Chorus();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);

private:
    double sampleRate = 44100.0;
    
    // Delay line for chorus effect
    static constexpr int maxDelayInSamples = 4410; // 100ms at 44.1kHz
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize = 0;
    int writePosition = 0;
    
    // LFO for modulation
    float lfoPhase[2] = {0.0f, 0.0f};
    float lfoRate = 0.5f; // Hz
    float lfoDepth = 0.003f; // seconds
    
    // Brigade-style parameters
    float feedback = 0.15f;
    float stereoWidth = 0.8f;
    float warmth = 0.3f;
    
    // Filtering for analog warmth
    juce::dsp::IIR::Filter<float> lowPassFilter[2];
    juce::dsp::IIR::Filter<float> highPassFilter[2];
    
    // Utility functions
    float getDelayedSample(int channel, float delayInSamples);
    void writeToDelayBuffer(int channel, float sample);
    float generateLFO(int channel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
};