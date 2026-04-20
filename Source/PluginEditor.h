#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class CrushChorusAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CrushChorusAudioProcessorEditor(CrushChorusAudioProcessor&);
    ~CrushChorusAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CrushChorusAudioProcessor& audioProcessor;
    
    // UI Components
    juce::Slider crushMixSlider;
    juce::Label crushLabel;
    
    juce::Slider chorusMixSlider;
    juce::Label chorusLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrushChorusAudioProcessorEditor)
};