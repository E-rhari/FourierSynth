#  🎼 ∿ FourierSynth ∿ 🎵

FourierSynth is a simple additive synthesizer that creates adjustable harmonics as to create many timbres.

The name was created when I was naiver and didn't really know about know about additive synthesizes, and how common place the ideas of adding up sine waves is in digital signal processing. The idea originally was a synthesizer inspired by concepts from the Fourier Transform, hence the name.

## Compile
### Dependencies
[Download Juce](https://juce.com/download/) at `C:/install/JUCE` for windows users or `~/JUCE` for Mac and Linux users. Alternatively, you can install it anywhere you want and change the appropriate line of `CMakeList.txt`.

This program uses [CMake](https://cmake.org/download/) to compile, so be sure to have it installed

### Compilation
Execute
```shell
./build.sh
```
If not in a unix-like system, such as Windows, copy and paste the contents of the same shell file.

## Run
Plug it into a DAW (Digital Audio Workstation) of your choice and use it. You can also execute the standalone version without a host.

## Structure
Juce's framework is really big and robust and it can be a bit overwhelming. For those uninitiated, I preset a brief summary of this framework's structure as well as the particularities of the project's.

### Debug
An static class that prints in the terminal hosting the application.

### Oscillator
Responsible for generating the actual base value for the samples.

### Plugin Editor
Handles the program GUI interface.

### Plugin Processor
Bridges the Synth and the host.
It handles parameters, sample rates, presets and plugin configuration.

### Preset
Structure to model parameter presets.

### Synth
Responsible for the synthesizes of all notes.
Handles MIDI and the voices of the program.

### Voice
Handles the synthesizes of one note.

