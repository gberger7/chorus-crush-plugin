#include "PluginProcessor.h"
#include "PluginEditor.h"

CrushChorusAudioProcessorEditor::CrushChorusAudioProcessorEditor(CrushChorusAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
    
    // Setup Crush Mix Slider
    crushMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    crushMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    crushMixSlider.setRange(0.0, 1.0, 0.01);
    crushMixSlider.setValue(0.0);
    crushMixSlider.setTextValueSuffix("%");
    crushMixSlider.setNumDecimalPlacesToDisplay(0);
    crushMixSlider.textFromValueFunction = [](double value) { return juce::String(static_cast<int>(value * 100)) + "%"; };
    crushMixSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(crushMixSlider);
    
    crushLabel.setText("CRUSH", juce::dontSendNotification);
    crushLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    crushLabel.setJustificationType(juce::Justification::centred);
    crushLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(crushLabel);
    
    // Setup Chorus Mix Slider
    chorusMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chorusMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    chorusMixSlider.setRange(0.0, 1.0, 0.01);
    chorusMixSlider.setValue(0.0);
    chorusMixSlider.setTextValueSuffix("%");
    chorusMixSlider.setNumDecimalPlacesToDisplay(0);
    chorusMixSlider.textFromValueFunction = [](double value) { return juce::String(static_cast<int>(value * 100)) + "%"; };
    chorusMixSlider.setTextBoxIsEditable(true);
    addAndMakeVisible(chorusMixSlider);
    
    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    chorusLabel.setJustificationType(juce::Justification::centred);
    chorusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(chorusLabel);
    
    // Create parameter attachments
    crushMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "crushMix", crushMixSlider);
    
    chorusMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "chorusMix", chorusMixSlider);
    
    // Custom slider appearance
    auto setupSliderLookAndFeel = [](juce::Slider& slider) {
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a90e2));
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2c3e50));
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff34495e));
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    };
    
    setupSliderLookAndFeel(crushMixSlider);
    setupSliderLookAndFeel(chorusMixSlider);
}

CrushChorusAudioProcessorEditor::~CrushChorusAudioProcessorEditor()
{
}

void CrushChorusAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Dark gradient background
    juce::ColourGradient gradient(juce::Colour(0xff2c3e50), 0, 0,
                                  juce::Colour(0xff34495e), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Plugin title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("CHORUS CRUSH", getLocalBounds().removeFromTop(60), 
               juce::Justification::centred, true);
    
    // Subtle border
    g.setColour(juce::Colour(0xff4a90e2));
    g.drawRect(getLocalBounds(), 2);
}

void CrushChorusAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    bounds.reduce(20, 20);
    
    // Split into two columns
    auto leftColumn = bounds.removeFromLeft(bounds.getWidth() / 2);
    auto rightColumn = bounds;
    
    // Chorus section (left side)
    chorusLabel.setBounds(leftColumn.removeFromTop(30));
    leftColumn.removeFromTop(10); // Spacing
    chorusMixSlider.setBounds(leftColumn.removeFromTop(120));
    
    // Crush section (right side)
    crushLabel.setBounds(rightColumn.removeFromTop(30));
    rightColumn.removeFromTop(10); // Spacing
    crushMixSlider.setBounds(rightColumn.removeFromTop(120));
}