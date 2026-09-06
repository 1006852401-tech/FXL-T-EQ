// ==========================================================
//  Seesaw Tilt EQ - PluginEditor.h
// ==========================================================
#pragma once

#include "PluginProcessor.h"

class SeesawTiltEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SeesawTiltEQAudioProcessorEditor (SeesawTiltEQAudioProcessor&);
    ~SeesawTiltEQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SeesawTiltEQAudioProcessor& audioProcessor;

    juce::Slider tiltSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tiltAttachment;

    juce::ToggleButton bypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeesawTiltEQAudioProcessorEditor)
};
