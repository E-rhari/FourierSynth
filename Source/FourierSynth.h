#pragma once

#include "JuceHeader.h"
#include <numbers>


class FourierSynth
{
private:
	juce::Slider frequencySlider;

	float currentAngle = 0.f;
	float deltaAngle = 0.f;
	float currentSampleRate = 0.f;


public:
	FourierSynth() {
		frequencySlider.setRange(440.0, 880.0);
		frequencySlider.onValueChange = [this] {
			if (currentSampleRate > 0.0)
				updateDeltaAngle();
			};

	}

	void prepareToPlay(double sampleRate, int samplesPerBlock) {
		currentSampleRate = sampleRate;
		updateDeltaAngle();
	}

	void updateDeltaAngle() {
		auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
		deltaAngle = cyclesPerSample * 2.0 * std::numbers::pi;
	}

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
		float level = 1.f;

		float* leftChannel = buffer.getWritePointer(0);
		float* rightChannel = buffer.getWritePointer(1);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
			float currentSample = (float)std::sin(currentAngle);
			leftChannel[sample] = currentSample * level;
			rightChannel[sample] = currentSample * level;
			currentAngle += deltaAngle;
		}
	}
};