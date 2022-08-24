/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BasicDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicDistortionAudioProcessorEditor (BasicDistortionAudioProcessor&);
    ~BasicDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void updateToggleState (juce::Button* button, juce::String name);
    void modeMenuChanged();
  
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
  juce::Slider mSliderVol;//Volume slider
  juce::Label mLabelVol; //Volume label
  
  juce::Slider mSliderDistThresh;//Threshold for the distortion slider
  juce::Label mLabelDistThresh; //Threshold for the distortion label
  
  juce::Slider mSliderDistMod;//Dist mod value
  juce::Label mLabelDistMod; //Dist mod value label
  
  juce::Slider mSliderSoftAmount;//Soft Clip amount slider
  juce::Label mLabelSoftAmount; //Label
  
  juce::ToggleButton mDistButton {"DistMode"};
  
  juce::Label distLabel { {}, "Distortion Mode" };
  juce::Font textFont   { 12.0f };
  juce::ComboBox modeMenu;

  
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mVolSliderAttachment;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mVolDistThreshAttachment;
  
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mDistModeAttachment;
  
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDistValueAttachment;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mSoftAmountAttachment;
    BasicDistortionAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicDistortionAudioProcessorEditor)
};
