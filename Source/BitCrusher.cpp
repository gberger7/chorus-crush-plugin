#include "BitCrusher.h"

BitCrusher::BitCrusher()
{
}

BitCrusher::~BitCrusher()
{
}

void BitCrusher::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    
    // Setup anti-aliasing filters (low-pass at Nyquist/4 for smooth aliasing)
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, sampleRate * 0.125);
    
    for (int ch = 0; ch < 2; ++ch)
    {
        antiAliasFilter[ch].coefficients = coefficients;
        antiAliasFilter[ch].reset();
    }
    
    // Reset state
    sampleCounter = 0;
    holdSample[0] = holdSample[1] = 0.0f;
}

void BitCrusher::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Logic Pro X Downsample style - aggressive sample rate reduction + bit crushing
    downsampleFactor = 6; // Moderate downsampling like Logic's Downsample
    
    for (int channel = 0; channel < numChannels && channel < 2; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];
            
            // Sample rate reduction with hold (Logic Pro X Downsample style)
            if (sampleCounter % downsampleFactor == 0)
            {
                // Apply aggressive bit crushing like Logic's Bitcrusher
                float boosted = input * 2.0f; // Boost for more aggressive crushing
                float crushed = quantize(boosted, 4.0f); // 4-bit for harsh digital sound
                holdSample[channel] = juce::jlimit(-1.0f, 1.0f, crushed);
            }
            
            // Output the held sample (creates the downsampled + bit crushed effect)
            channelData[sample] = holdSample[channel];
            sampleCounter++; // Increment per sample for proper downsampling
        }
    }
}

float BitCrusher::softClip(float input)
{
    // Soft saturation for analog warmth
    const float threshold = 0.7f;
    
    if (std::abs(input) < threshold)
        return input;
    
    float sign = (input > 0.0f) ? 1.0f : -1.0f;
    float excess = std::abs(input) - threshold;
    float compressed = threshold + excess / (1.0f + excess * 2.0f);
    
    return sign * compressed;
}

float BitCrusher::quantize(float input, float bits)
{
    // Quantize to specified bit depth
    float levels = std::pow(2.0f, bits);
    float step = 2.0f / levels;
    
    // Add subtle dithering for smoother sound
    float dither = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * step * 0.1f;
    
    float quantized = std::round((input + dither) / step) * step;
    
    // Clamp to valid range
    return juce::jlimit(-1.0f, 1.0f, quantized);
}