/*
  ==============================================================================

    PolygonSynthesesVoice.h
    Created: 2 Jan 2020 6:07:28pm
    Author:  Jens Hagen

  ==============================================================================
*/

#pragma once
#include "Polygon.h"
#include "ModulationSources.h"


struct VoiceParameters{
	float vertex_count;
	float rotation_speed;
	float collapse;
	float expand;
	float freq_ratio;
};

class PolygonSynthesisSound : public juce::SynthesiserSound
{
public:
	// Inherited via SynthesiserSound
	virtual bool appliesToNote( int midiNoteNumber ) override;
	virtual bool appliesToChannel( int midiChannel ) override;
};

class PolygonSynthesisVoice : public juce::SynthesiserVoice
{
public:

	PolygonSynthesisVoice() = delete;
	PolygonSynthesisVoice( VoiceParameters* voice_parameters ) :
		voice_parameters( voice_parameters )
	{}

	// voices do not own voice_parameter*
	~PolygonSynthesisVoice() override
	{
	}

	// Inherited via SynthesiserVoice
	virtual bool canPlaySound( SynthesiserSound* ) override;

	virtual void startNote( int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition ) override;

	virtual void stopNote( float velocity, bool allowTailOff ) override;

	virtual void pitchWheelMoved( int newPitchWheelValue ) override;

	virtual void controllerMoved( int controllerNumber, int newControllerValue ) override;

	virtual void renderNextBlock( AudioBuffer<float>& outputBuffer, int startSample, int numSamples ) override;

private:
	double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
	VoiceParameters* voice_parameters;
	Polygon polygon;
	env::AttRelEnv temp_env;
};


