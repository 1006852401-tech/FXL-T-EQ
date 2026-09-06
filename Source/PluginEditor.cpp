// ==========================================================
//  Seesaw Tilt EQ - PluginEditor.cpp
// ==========================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

SeesawTiltEQAudioProcessorEditor::SeesawTiltEQAudioProcessorEditor (SeesawTiltEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (380, 480);

    // Single master rotary knob
    tiltSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    tiltSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 24);
    tiltSlider.setDoubleClickReturnValue (true, 0.0); // Double click resets to 0 center
    addAndMakeVisible (tiltSlider);

    tiltAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.apvts, SeesawTiltEQAudioProcessor::TILT_ID, tiltSlider);

    // Bypass button
    bypassButton.setButtonText ("BYPASS");
    addAndMakeVisible (bypassButton);

    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.apvts, SeesawTiltEQAudioProcessor::BYPASS_ID, bypassButton);
}

SeesawTiltEQAudioProcessorEditor::~SeesawTiltEQAudioProcessorEditor()
{
}

void SeesawTiltEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // High-end hardware rack faceplate styling
    g.fillAll (juce::Colour (0xff1a1d21));

    // Outer metal border
    g.setColour (juce::Colour (0xff2d333b));
    g.drawRect (getLocalBounds(), 2);

    // Header Title
    g.setColour (juce::Colour (0xfff0f6fc));
    g.setFont (juce::FontOptions (18.0f, juce::Font::bold));
    g.drawText ("SEESAW TILT EQ", 0, 24, getWidth(), 24, juce::Justification::centred);

    g.setColour (juce::Colour (0xff8b949e));
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("PIVOT: 650 Hz  |  SINGLE-KNOB TILT", 0, 48, getWidth(), 18, juce::Justification::centred);

    // Knob Labels
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xffd29922)); // Warm amber for bass
    g.drawText ("< WARM / BASS", 40, 280, 100, 20, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xff58a6ff)); // Cool blue for treble
    g.drawText ("BRIGHT / TREBLE >", getWidth() - 140, 280, 100, 20, juce::Justification::centredRight);

    // Center Detent 0 Mark
    g.setColour (juce::Colour (0xffffffff));
    g.drawText ("0 (FLAT)", 0, 100, getWidth(), 18, juce::Justification::centred);
}

void SeesawTiltEQAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    tiltSlider.setBounds (area.getCentreX() - 90, 120, 180, 180);
    bypassButton.setBounds (area.getCentreX() - 45, 420, 90, 28);
}
