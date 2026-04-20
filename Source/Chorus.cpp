#include "Chorus.h"

Chorus::Chorus()
{
}

Chorus::~Chorus()
{
}

void Chorus::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    
    // Setup delay buffer
    delayBufferSize = static_cast<int>(sampleRate * 0.1); // 100ms max delay
    delayBuffer.setSize(2, delayBufferSize);
    delayBuffer.clear();
    writePosition = 0;
    
    // Setup filters for analog warmth (Brigade-style)
    auto lowPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 8000.0);
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0);
    
    for (int ch = 0; ch < 2; ++ch)
    {
        lowPassFilter[ch].coefficients = lowPassCoeffs;
        highPassFilter[ch].coefficients = highPassCoeffs;
        lowPassFilter[ch].reset();
        highPassFilter[ch].reset();
        
        // Initialize LFO with slight phase offset for stereo
        lfoPhase[ch] = ch * 0.25f; // 90 degree phase difference
    }
}

void Chorus::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels && channel < 2; ++channel)
        {
            float input = buffer.getSample(channel, sample);
            
            // Generate LFO modulation
            float lfo = generateLFO(channel);
            
            // Calculate delay time (Brigade-style: 5-25ms base delay + modulation)
            float baseDelay = 0.015f; // 15ms base delay
            float modulatedDelay = baseDelay + (lfo * lfoDepth);
            float delayInSamples = modulatedDelay * static_cast<float>(sampleRate);
            
            // Get delayed sample with interpolation
            float delayedSample = getDelayedSample(channel, delayInSamples);
            
            // Apply feedback
            float feedbackSample = delayedSample * feedback;
            
            // Write to delay buffer (input + feedback)
            writeToDelayBuffer(channel, input + feedbackSample);
            
            // Apply analog-style filtering to delayed signal
            float filteredDelay = lowPassFilter[channel].processSample(delayedSample);
            filteredDelay = highPassFilter[channel].processSample(filteredDelay);
            
            // Add subtle saturation for warmth
            filteredDelay = std::tanh(filteredDelay * (1.0f + warmth)) / (1.0f + warmth);
            
            // Stereo widening - opposite channels get different modulation
            float stereoMod = (channel == 0) ? 1.0f : -1.0f;
            float widening = stereoWidth * stereoMod * lfo * 0.1f;
            
            // Mix dry and wet signals
            float output = input + filteredDelay + widening;
            
            buffer.setSample(channel, sample, output * 0.7f); // Slight level reduction
        }
        
        // Advance write position
        writePosition = (writePosition + 1) % delayBufferSize;
    }
}

float Chorus::getDelayedSample(int channel, float delayInSamples)
{
    float readPosition = writePosition - delayInSamples;
    
    if (readPosition < 0.0f)
        readPosition += delayBufferSize;
    
    // Linear interpolation
    int readPos1 = static_cast<int>(readPosition) % delayBufferSize;
    int readPos2 = (readPos1 + 1) % delayBufferSize;
    float fraction = readPosition - static_cast<float>(readPos1);
    
    float sample1 = delayBuffer.getSample(channel, readPos1);
    float sample2 = delayBuffer.getSample(channel, readPos2);
    
    return sample1 + fraction * (sample2 - sample1);
}

void Chorus::writeToDelayBuffer(int channel, float sample)
{
    delayBuffer.setSample(channel, writePosition, sample);
}

float Chorus::generateLFO(int channel)
{
    // Sine wave LFO with slight phase offset per channel
    float lfo = std::sin(lfoPhase[channel] * 2.0f * juce::MathConstants<float>::pi);
    
    // Advance phase
    lfoPhase[channel] += lfoRate / static_cast<float>(sampleRate);
    
    if (lfoPhase[channel] >= 1.0f)
        lfoPhase[channel] -= 1.0f;
    
    return lfo;
}