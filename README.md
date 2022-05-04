# MUSI6106-Project
## Requirements
1. CMake >=3.15
2. Eigen >=3.3
3. JUCE == 6.1

## Installation Instructions
1. Open up plugin_framework/ProcessorGraphTest in CMake
2. Run CMake and open the project in your IDE (Xcode, CLion, Visual Studio tested)
3. Run the project with the desired plugin target
4. Copy the generated plugin from the cmake-build-release to your plugin folder

### To Install Eigen:
a. Run `git clone https://gitlab.com/libeigen/eigen.git` within ProcessorGraphTest/3rd-party
b. Run the following code from within the eigen folder (cd eigen first)  
   `mkdir build`    
   `cd build `     
   `cmake .. `   
   `make`  
   `sudo make install`    

c. `brew install superlu`  (or equivalent for windows/linux)
d. `brew install clang-format`


We are making a JUCE amp simulator plugin with tone control and other basic effects in a signal chain. The plugin’s idea was based on the work from SmartGuitarAmp by GuitarML on GitHub (https://github.com/GuitarML/SmartGuitarAmp), and we are modifying it by introducing the idea of using our own linear regression model to decide the percentage of the output tone, gain, or amp within our amp simulator plugin. We are keeping the idea of using a machine-trained model of an amp simulator as in SmartGuitarAmp, and using waveshaping as a valve simulator, PedalNetRT, and other basic guitar amp simulation algorithms to create the amp tone control. 
Our end goal is to have an amp simulator with a few built-in amps and 4-5 effects (could include reverb, LPF, HPF, comb filters, flanger, chorus, and distortion effects) that are easy to control similarly to a real guitar amp, and as a stretch goal we hope to implement a linear regression model for tone control where the user can upload an audio file of what they want their amp to sound like and the machine learning model will adjust the tone control and gain knobs to be close to the amount of wet/dry tone and would decide which of our default amp models to use to sound the closest.
The plugin will have an intuitive GUI where users can upload an audio file to determine the tone of the amp as well as manually adjust parameters and effects. If we have time, we will incorporate a modular “drag and drop” signal chain within the GUI that will contain reverb, EQ, and compression. 
The plugin is targeted at guitar players who want to record into a DAW with the sound of their ideal amps, and is especially useful for an artist on the go. It is easier to use than other amp simulators, as you either get less choice in tone or too many controls with standard amp simulation plugins. Our plugin gets straight to the tone you want, no fuss.


## Implementation Plan
### Signal Flow Diagram
![plot](https://github.com/jocekav/MUSI6106-Project/blob/main/GuitarAmpPlugin.png)

### Class Structure Diagram
![plot](https://github.com/jocekav/MUSI6106-Project/blob/main/Guitar%20Amp%20Class%20Structure.jpg)

1. Build plugin backend
   1. Static wave shaper - MVP step 1
   2. Analog circuit modeling - MVP step 2
      1. Load wav files
      2. Implement IR convolution
   3. JSON load for pedalnet
2. Build GUI
   1. Create basic GUI for effects and tone control (includes reverb, basic LPF, HPF, and comb filters, flanger, and chorus effects)
   2. Put more knobs and effects into the basic amp model like SmartGuitar Amp’s GUI
      1. Create sketches of the layout for each effect’s parameters GUI in Figma, possibly change from SmartGuitar Amp’s design if it is not expandable enough
   3. Midterm Milestone: Implement the GUI (without sophisticated controls, can just be the visuals but with space to put the functionality)
   4. Implement the tone control (from amp simulator models or waveshaper model)
   5. Implement the other effects’ GUI components
   6. User testing for ease of use and understanding 
3. Implement PedalNet
   1. Get PedalNet running
   2. Evaluate and improve the performance either locally or on server
   2. Train multiple different models - for amp and distortion pedals
4. Figure out regression model for parameter estimation
   1. Research existing techniques
      1. Implement an example regression without audio input
   2. Implement regression model for audio input
   3. Midterm Milestone: Test on audio mapping - tone control
   4. Combine with current system
   5. Stretch: extend to other controls
5. Implement FX chain
   1. EQ 
      1. Parametric
      2. graphic
   2. Reverb
   3. Compression
