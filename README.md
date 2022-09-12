# JUCE_BasicDistortion
A basic distortion plugin that implements a cubic softclip and a hardclip distortion alrogrithm.

Menu

Used to select the distortion mode that is to be used. Half and fullwave rectification are either on or off and do not have any controls for the amount of distortion.


Sliders

---Volume: Controls the overall volume.

--Hardclip Controls
  -Threshold: Amplitude level at which hardclipping occurs per sample.
  -Mod: controls how many samples actually have the hardclipping applied. Ex. 2 means every other sample at the HC level actually gets hardclipped, 3              means every third sample, 4 every fourth sample and so on. 
  
--Softclip Controls
  -Softamt: Constant multiplier for the value being subtracted from the input signal
