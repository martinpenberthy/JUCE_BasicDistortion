/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDistortionAudioProcessorEditor::BasicDistortionAudioProcessorEditor (BasicDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

  
  mSliderVol.setSliderStyle (juce::Slider::LinearBarVertical);
  //mSliderVol.setTextBoxStyle(juce::Slider::TextBoxRight, true, 40, 20);
  mSliderVol.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 100));
  mLabelVol.attachToComponent(&mSliderVol, true);
  mLabelVol.setText("Volume", juce::dontSendNotification);
  
  mSliderDistThresh.setSliderStyle (juce::Slider::LinearBarVertical);
 // mSliderDistThresh.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
  mSliderDistThresh.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 100));
  mLabelDistThresh.attachToComponent(&mSliderDistThresh, true);
  mLabelDistThresh.setText("Thresh", juce::dontSendNotification);
  
  mSliderDistMod.setSliderStyle (juce::Slider::LinearBarVertical);
  //mSliderDistMod.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
  mSliderDistMod.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 100));
  mLabelDistMod.attachToComponent(&mSliderDistMod, true);
  mLabelDistMod.setText("Mod", juce::dontSendNotification);
  
  
  mSliderSoftAmount.setSliderStyle (juce::Slider::LinearBarVertical);
  //mSliderSoftAmount.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
  mSliderSoftAmount.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 100));
  mLabelSoftAmount.attachToComponent(&mSliderSoftAmount, true);
  mLabelSoftAmount.setText("SoftAmt", juce::dontSendNotification);
  
  addAndMakeVisible (distLabel);
  modeMenu.addItem ("Hard Clip",  1);
  modeMenu.addItem ("Soft Clip",   2);

  modeMenu.onChange = [this] { modeMenuChanged(); };
  modeMenu.setSelectedId (0);
  
  distLabel.attachToComponent(&modeMenu, true);
  distLabel.setFont (textFont);
  distLabel.setColour(juce::Label::textColourId, juce::Colours::white);
  //mDistButton.onClick = [this] { updateToggleState (&mDistButton, "DistMode"); };
  //mDistButton.setToggleState(false, juce::dontSendNotification);
  
  //Add to the editor component
  addAndMakeVisible(mSliderVol);
  addAndMakeVisible(mSliderDistThresh);
  addAndMakeVisible(mSliderDistMod);
  addAndMakeVisible(mSliderSoftAmount);
  addAndMakeVisible(mDistButton);
  addAndMakeVisible(modeMenu);

  
  mVolSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", mSliderVol);
  
  mVolDistThreshAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "THRESHOLD", mSliderDistThresh);
  
  mDistModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DISTMODE", mDistButton);
  
  
  mDistValueAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DISTVALUE", mSliderDistMod);
  
  mSoftAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SOFTAMT", mSliderSoftAmount);
  
  setSize (600, 600);
}

BasicDistortionAudioProcessorEditor::~BasicDistortionAudioProcessorEditor()
{
}

//==============================================================================
void BasicDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll (juce::Colours::black);

}

void BasicDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
  
  //mSliderVol.setBoundsRelative(0.5f, 0.4f, 0.05f, 0.3f);
  mSliderVol.setBounds(getWidth()/2 + 200, getHeight() /2 - 50, 40, 300);
  mSliderDistThresh.setBounds(getWidth()/2, getHeight() /2 - 50, 40, 300);
  
  mDistButton.setBounds(getWidth()/4, getHeight()/4, 20, 20);
  
  mSliderDistMod.setBounds(getWidth()/2 - 100, getHeight()/2 - 50, 40, 300);
  mSliderSoftAmount.setBounds(getWidth()/2 - 200, getHeight()/2 - 50, 40, 300);
  modeMenu.setBounds(getWidth()/4 + 100, getHeight()/4 - 50, 100, 20);
}

void BasicDistortionAudioProcessorEditor::modeMenuChanged()
{
    switch (modeMenu.getSelectedId())
    {
        case 1:
            audioProcessor.distMode = 1;
            break;
        case 2:
            audioProcessor.distMode = 2;
            break;
      
      default:
        audioProcessor.distMode = 0;
          break;
    }

  //distLabel.setFont (textFont);
}


/*void BasicDistortionAudioProcessorEditor::updateToggleState (juce::Button* button, juce::String name)
{
 //std::cout << "state toggle" << std::endl;
  //audioProcessor.distMode = !audioProcessor.distMode;
}

*/
