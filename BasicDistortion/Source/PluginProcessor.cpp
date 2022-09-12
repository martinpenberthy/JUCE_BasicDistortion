/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDistortionAudioProcessor::BasicDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters() )//Constructor for apvts
#endif
{
  
  apvts.state.addListener(this);

  
}

BasicDistortionAudioProcessor::~BasicDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String BasicDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
  
}

void BasicDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

  //Get gain value from the slider
 // auto gainValue = apvts.getRawParameterValue("GAIN")->load();
 // auto distThreshold = apvts.getRawParameterValue("THRESHOLD")->load();
    
  
  // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  
    switch(distMode)
    {
      //HARDCLIP
      case 1:
        for(int i = 0; i < totalNumOutputChannels; ++i)
        {
          for(int j = 0; j < buffer.getNumSamples(); ++j)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            distThreshold = apvts.getRawParameterValue("THRESHOLD")->load();
            distModValue = apvts.getRawParameterValue("DISTVALUE")->load();
            
            auto sample = buffer.getSample(i, j);
            //Process left channel
            if(std::abs(sample) > distThreshold && j % distModValue == 0)
            {
              if(sample > 0.0f)
                buffer.setSample(i, j, (distThreshold * gainValue) / distThreshold);
              else
                buffer.setSample(i, j, (-distThreshold * gainValue) / distThreshold);
            }else
              buffer.setSample(i, j, sample * gainValue);
        
          }
        }
        break;
        
        //SOFTCLIP
      case 2:
        for(int i = 0; i < totalNumOutputChannels; ++i)
        {
          for(int j = 0; j < buffer.getNumSamples(); ++j)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            //distThreshold = apvts.getRawParameterValue("THRESHOLD")->load();
            distSoftAmount = apvts.getRawParameterValue("SOFTAMT")->load();
            
            
            //Process left sample
            auto sample = buffer.getSample(i, j);
            sample = sample - (float)distSoftAmount * ((1.0f / 3.0f) * pow(sample, 3)) * gainValue;
            buffer.setSample(i, j, sample);

          }
        }
        break;
        
        //HALFWAVE RECT
      case 3:
        for(int i = 0; i < totalNumOutputChannels; ++i)
        {
          for(int j = 0; j < buffer.getNumSamples(); ++j)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
              
            //Process left channel
            auto sample =  buffer.getSample(i, j);
            if(sample < 0.0f)
              buffer.setSample(i, j, 0.0f);
            else
              buffer.setSample(i, j, sample * gainValue);
          }
        }
        break;
        
      //FULLWAVE RECT
      case 4:
        for(int i = 0; i < totalNumOutputChannels; ++i)
        {
          for(int j = 0; j < buffer.getNumSamples(); ++j)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            
            //Process left channel
            auto sample =  buffer.getSample(i, j);
            if(sample < 0.0f)
              buffer.setSample(i, j, (sample * -1) * gainValue);
            else
              buffer.setSample(i, j, sample * gainValue);

          }
        }
        break;
        
      default:
        for(auto i = 0; i < totalNumOutputChannels; ++i)
        {
          for(auto j = 0; j < buffer.getNumSamples(); ++j)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            auto sample = buffer.getSample(i, j);

            buffer.setSample(i, j, sample * gainValue);
          }
        }
        break;
    }//END SWITCH
    
  /*
    //Get write pointers for both channels
    float* const leftChannel = buffer.getWritePointer(0, 0);
    float* const rightChannel = buffer.getWritePointer(1, 0);
    //distProcess = !distProcess
    
    //if(apvts.getParameter("DISTMODE")->getValue())
    switch(distMode)
    {
      //HARDCLIP
      case 1:
          //Loop through samples
          for(auto i = 0; i < buffer.getNumSamples(); ++i)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            distThreshold = apvts.getRawParameterValue("THRESHOLD")->load();
            distModValue = apvts.getRawParameterValue("DISTVALUE")->load();
            
            //Process left channel
            if(std::abs(leftChannel[i]) > distThreshold && i % distModValue == 0)
            {
              if(leftChannel[i] > 0.0f)
                leftChannel[i] = (distThreshold * gainValue) / distThreshold;
              else
                leftChannel[i] = (-distThreshold * gainValue) / distThreshold;
            }else
              leftChannel[i] = leftChannel[i] * gainValue;
            
            //Process right channel
            if(std::abs(rightChannel[i]) > distThreshold && i % distModValue == 0)
            {
              if(rightChannel[i] > 0.0f)
                rightChannel[i] = (distThreshold * gainValue) / distThreshold;
              else
                rightChannel[i] = (-distThreshold * gainValue) / distThreshold;
            }else
              rightChannel[i] = rightChannel[i] * gainValue;
          }
          break;
      
      //SOFTCLIP
      case 2:
          for(auto i = 0; i < buffer.getNumSamples(); ++i)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            //distThreshold = apvts.getRawParameterValue("THRESHOLD")->load();
            distSoftAmount = apvts.getRawParameterValue("SOFTAMT")->load();
            
            
            //Process left sample
            auto sample = leftChannel[i];
            leftChannel[i] = (sample - (float)distSoftAmount * ((1.0f / 3.0f) * pow(sample, 3))) * gainValue;
            
            //Process right sample
            sample = rightChannel[i];
            rightChannel[i] = (sample - (float)distSoftAmount * ((1.0f / 3.0f) * pow(sample, 3))) * gainValue;
          }
          break;
        
      //HALFWAVE RECT
      case 3:
        for(auto i = 0; i < buffer.getNumSamples(); ++i)
        {
          gainValue = apvts.getRawParameterValue("GAIN")->load();
          
          //Process left channel
          auto sample = leftChannel[i];
          if(sample < 0.0f)
            leftChannel[i] = 0.0f;
          else
            leftChannel[i] = sample * gainValue;
          
          //Process right channel
          sample = rightChannel[i];
          if(sample < 0.0f)
            rightChannel[i] = 0.0f;
          else
            rightChannel[i] = rightChannel[i] * gainValue;
        }
        break;
        
      case 4:
        for(auto i = 0; i < buffer.getNumSamples(); ++i)
        {
          gainValue = apvts.getRawParameterValue("GAIN")->load();
          
          //Process left channel
          auto sample = leftChannel[i];
          if(sample < 0.0f)
            leftChannel[i] = (sample * -1) * gainValue;
          else
            leftChannel[i] = sample * gainValue;
          
          //Process right channel
          sample = rightChannel[i];
          if(sample < 0.0f)
            rightChannel[i] = (sample * -1) * gainValue;
          else
            rightChannel[i] = rightChannel[i] * gainValue;
        }
        break;
        
      default:
          for(auto i = 0; i < buffer.getNumSamples(); ++i)
          {
            gainValue = apvts.getRawParameterValue("GAIN")->load();
            
            leftChannel[i] = leftChannel[i] * gainValue;
            rightChannel[i] = rightChannel[i] * gainValue;
          }
          break;
    }//End switch
    */

}//End processBlock

//==============================================================================
bool BasicDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicDistortionAudioProcessor::createEditor()
{
    return new BasicDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void BasicDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
  
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml (state.createXml());
  copyXmlToBinary (*xml, destData);
}

void BasicDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
  std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
          apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicDistortionAudioProcessor();
}


/*
  This function is called in the initializer list for the HelloSamplerAudioProcessor constructor
  as a parameter for the AudioParameterValueTreeState and sets up the parameters needed
  for this plugin.
 */
juce::AudioProcessorValueTreeState::ParameterLayout BasicDistortionAudioProcessor::createParameters()
{
  juce::AudioProcessorValueTreeState::ParameterLayout params;
  //Make a vector of RangedAudioParameters to store all the parameters in
  //std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
  
  //Add a float parameter for the Gain value
  params.add(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 1.0f, 0.25f));
  params.add(std::make_unique<juce::AudioParameterFloat>("THRESHOLD", "Threshold", 0.1f, 1.0f, 1.0f));
  params.add(std::make_unique<juce::AudioParameterBool>("DISTMODE", "DistMode", true));
  params.add(std::make_unique<juce::AudioParameterInt>("DISTVALUE", "DistValue", 0, 100, 1));
  params.add(std::make_unique<juce::AudioParameterInt>("SOFTAMT", "SoftClipAmount", 0, 25, 1));
  //Return ParameterLayout
  //return {params.begin(), params.end()};
  return params;
}


/*
  Override of the valueTreePropertyChanged function. Similar to the valueChanged
  function used with Sliders and SliderListeners. If the value has changed, the
  flag is set which then causes the ADSR values to be updated from the processBlock
  function
 */
void BasicDistortionAudioProcessor::valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{

}
