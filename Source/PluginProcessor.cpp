#include "PluginProcessor.h"
#include "PluginEditor.h"

CrushChorusAudioProcessor::CrushChorusAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      valueTreeState(*this, nullptr, "Parameters",
                     juce::AudioProcessorValueTreeState::ParameterLayout{
                         std::make_unique<juce::AudioParameterFloat>("crushMix", "Crush Mix", 0.0f, 1.0f, 0.0f),
                         std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.0f)
                     })
{
    crushMixParam = valueTreeState.getRawParameterValue("crushMix");
    chorusMixParam = valueTreeState.getRawParameterValue("chorusMix");
}

CrushChorusAudioProcessor::~CrushChorusAudioProcessor()
{
}

const juce::String CrushChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CrushChorusAudioProcessor::acceptsMidi() const
{
    return false;
}

bool CrushChorusAudioProcessor::producesMidi() const
{
    return false;
}

bool CrushChorusAudioProcessor::isMidiEffect() const
{
    return false;
}

double CrushChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CrushChorusAudioProcessor::getNumPrograms()
{
    return 1;
}

int CrushChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CrushChorusAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CrushChorusAudioProcessor::getProgramName(int index)
{
    return {};
}

void CrushChorusAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void CrushChorusAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    bitCrusher.prepare(sampleRate, samplesPerBlock);
    chorus.prepare(sampleRate, samplesPerBlock);
}

void CrushChorusAudioProcessor::releaseResources()
{
}

bool CrushChorusAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void CrushChorusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Store dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Apply bitcrusher
    juce::AudioBuffer<float> crushBuffer;
    crushBuffer.makeCopyOf(dryBuffer);
    bitCrusher.process(crushBuffer);
    
    // Apply chorus
    juce::AudioBuffer<float> chorusBuffer;
    chorusBuffer.makeCopyOf(dryBuffer);
    chorus.process(chorusBuffer);
    
    // True parallel blending - both effects blend with dry independently
    float crushMix = crushMixParam->load();
    float chorusMix = chorusMixParam->load();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);
        auto* crushData = crushBuffer.getReadPointer(channel);
        auto* chorusData = chorusBuffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float dry = dryData[sample];
            float crushed = crushData[sample];
            float chorused = chorusData[sample];
            
            // Step 1: Apply crush effect
            float crushResult = dry * (1.0f - crushMix) + crushed * crushMix;
            
            // Step 2: Apply chorus effect - but preserve the crush character
            float result;
            if (crushMix > 0.0f && chorusMix > 0.0f)
            {
                // Both active - blend crush with chorus, keeping crush prominent
                float chorusBlend = crushResult * (1.0f - chorusMix) + chorused * chorusMix;
                result = (crushResult * 0.7f) + (chorusBlend * 0.5f); // Keep crush prominent
            }
            else if (crushMix > 0.0f)
            {
                // Only crush active
                result = crushResult;
            }
            else if (chorusMix > 0.0f)
            {
                // Only chorus active
                result = dry * (1.0f - chorusMix) + chorused * chorusMix;
            }
            else
            {
                // No effects active
                result = dry;
            }
            
            channelData[sample] = result * 0.9f; // Slight level reduction for headroom
        }
    }
}

bool CrushChorusAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CrushChorusAudioProcessor::createEditor()
{
    return new CrushChorusAudioProcessorEditor(*this);
}

void CrushChorusAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CrushChorusAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrushChorusAudioProcessor();
}