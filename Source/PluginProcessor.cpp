// ==========================================================
//  Seesaw Tilt EQ - PluginProcessor.cpp
// ==========================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

SeesawTiltEQAudioProcessor::SeesawTiltEQAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

SeesawTiltEQAudioProcessor::~SeesawTiltEQAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SeesawTiltEQAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Single rotary knob centered at 0.0 dB (range: -6.0 dB to +6.0 dB, default: 0.0 dB)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { TILT_ID, 1 },
        "Tilt",
        juce::NormalisableRange<float> (-6.0f, 6.0f, 0.05f),
        0.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String (value, 1) + " dB"; },
        [](const juce::String& text) { return text.getFloatValue(); }
    ));

    // Pivot frequency (default: 650 Hz musical center)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { PIVOT_ID, 1 },
        "Pivot Frequency",
        juce::NormalisableRange<float> (200.0f, 2000.0f, 1.0f, 0.5f),
        650.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [](float value, int) { return juce::String ((int)value) + " Hz"; }
    ));

    // Bypass parameter
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { BYPASS_ID, 1 },
        "Bypass",
        false
    ));

    return { params.begin(), params.end() };
}

const juce::String SeesawTiltEQAudioProcessor::getName() const { return "Seesaw Tilt EQ"; }
bool SeesawTiltEQAudioProcessor::acceptsMidi() const { return false; }
bool SeesawTiltEQAudioProcessor::producesMidi() const { return false; }
bool SeesawTiltEQAudioProcessor::isMidiEffect() const { return false; }
double SeesawTiltEQAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SeesawTiltEQAudioProcessor::getNumPrograms() { return 1; }
int SeesawTiltEQAudioProcessor::getCurrentProgram() { return 0; }
void SeesawTiltEQAudioProcessor::setCurrentProgram (int) {}
const juce::String SeesawTiltEQAudioProcessor::getProgramName (int) { return {}; }
void SeesawTiltEQAudioProcessor::changeProgramName (int, const juce::String&) {}

void SeesawTiltEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = static_cast<float> (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;

    lowShelfL.prepare (spec);
    lowShelfR.prepare (spec);
    highShelfL.prepare (spec);
    highShelfR.prepare (spec);

    smoothedTilt.reset (sampleRate, 0.02); // 20ms parameter smoothing
    smoothedTilt.setCurrentAndTargetValue (*apvts.getRawParameterValue (TILT_ID));

    updateCoefficients (smoothedTilt.getCurrentValue(), *apvts.getRawParameterValue (PIVOT_ID));
}

void SeesawTiltEQAudioProcessor::releaseResources()
{
    lowShelfL.reset();
    lowShelfR.reset();
    highShelfL.reset();
    highShelfR.reset();
}

bool SeesawTiltEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void SeesawTiltEQAudioProcessor::updateCoefficients (float tiltDb, float pivotFreq)
{
    if (std::abs (tiltDb - lastTiltParam) < 0.001f && std::abs (pivotFreq - lastPivotParam) < 0.1f)
        return;

    lastTiltParam = tiltDb;
    lastPivotParam = pivotFreq;

    // Tilt EQ core physics:
    // When tilt is > 0: lowShelf gain is negative (-tilt), highShelf gain is positive (+tilt)
    // When tilt is < 0: lowShelf gain is positive (+|tilt|), highShelf gain is negative (-|tilt|)
    // Flat when tilt is 0
    const float lowGainLinear  = juce::Decibels::decibelsToGain (-tiltDb);
    const float highGainLinear = juce::Decibels::decibelsToGain (tiltDb);

    auto lowCoeffs  = IIRCoeffs::makeLowShelf (currentSampleRate, pivotFreq, 0.7071f, lowGainLinear);
    auto highCoeffs = IIRCoeffs::makeHighShelf (currentSampleRate, pivotFreq, 0.7071f, highGainLinear);

    *lowShelfL.coefficients  = *lowCoeffs;
    *lowShelfR.coefficients  = *lowCoeffs;
    *highShelfL.coefficients = *highCoeffs;
    *highShelfR.coefficients = *highCoeffs;
}

void SeesawTiltEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const bool isBypassed = *apvts.getRawParameterValue (BYPASS_ID) > 0.5f;
    if (isBypassed)
        return;

    const float targetTilt = *apvts.getRawParameterValue (TILT_ID);
    const float pivotFreq  = *apvts.getRawParameterValue (PIVOT_ID);
    smoothedTilt.setTargetValue (targetTilt);

    const int numSamples = buffer.getNumSamples();
    auto* channelDataL = buffer.getWritePointer (0);
    auto* channelDataR = totalNumInputChannels > 1 ? buffer.getWritePointer (1) : nullptr;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float currentTilt = smoothedTilt.getNextValue();
        updateCoefficients (currentTilt, pivotFreq);

        channelDataL[sample] = highShelfL.processSample (lowShelfL.processSample (channelDataL[sample]));

        if (channelDataR != nullptr)
        {
            channelDataR[sample] = highShelfR.processSample (lowShelfR.processSample (channelDataR[sample]));
        }
    }
}

bool SeesawTiltEQAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* SeesawTiltEQAudioProcessor::createEditor()
{
    return new SeesawTiltEQAudioProcessorEditor (*this);
}

void SeesawTiltEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SeesawTiltEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SeesawTiltEQAudioProcessor();
}
