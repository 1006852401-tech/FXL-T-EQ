// ==========================================================
//  Seesaw Tilt EQ - PluginProcessor.h
// ==========================================================
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class SeesawTiltEQAudioProcessor : public juce::AudioProcessor
{
public:
    SeesawTiltEQAudioProcessor();
    ~SeesawTiltEQAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    static constexpr auto TILT_ID   = "tilt";
    static constexpr auto PIVOT_ID  = "pivot";
    static constexpr auto BYPASS_ID = "bypass";

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP Filter chains for stereo (Left/Right)
    using IIRFilter = juce::dsp::IIR::Filter<float>;
    using IIRCoeffs = juce::dsp::IIR::Coefficients<float>;

    IIRFilter lowShelfL, lowShelfR;
    IIRFilter highShelfL, highShelfR;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedTilt;
    float currentSampleRate = 44100.0f;
    float lastTiltParam = -999.0f;
    float lastPivotParam = -999.0f;

    void updateCoefficients (float tiltDb, float pivotFreq);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeesawTiltEQAudioProcessor)
};
