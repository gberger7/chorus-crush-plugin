# Chorus Crush

A simple Chorus and Bitcrusher audio effect plugin built with JUCE 
C++. Available as VST3 and AU for Logic Pro, Ableton Live, Reaper, 
and any VST3/AU compatible DAW.

## Overview

Chorus Crush combines two classic effects in one minimal interface. 
The Chorus dial adds lush modulation and depth inspired by the 
classic UAD Brigade chorus — that warm, analog bucket-brigade 
character that sits beautifully on guitars, synths, and vocals. 
The Bitcrusher dial introduces lo-fi grit and digital texture on 
the other end of the spectrum. Use them independently or blend 
both together. Two knobs. Instant results.

## Technical Stack

- **Language:** C++17
- **Framework:** JUCE
  - juce_audio_processors / juce_audio_plugin_client
  - juce_audio_basics / juce_dsp / juce_gui_basics
- **Build System:** CMake 3.15+
- **Plugin Formats:** VST3 and AU
- **DSP Components:** Dedicated Chorus and BitCrusher 
  processing classes

## Building From Source

### Build
```bash
git clone https://github.com/YOURUSERNAME/chorus-crush-plugin.git
cd chorus-crush-plugin
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Installation

Copy the built plugin to your DAW plugin folder:

**VST3:** `~/Library/Audio/Plug-Ins/VST3/`

**AU:** `~/Library/Audio/Plug-Ins/Components/`

Restart your DAW and rescan plugins.

## Status

Complete and fully functional. Tested in Logic Pro X.

## Demo

Coming soon.
