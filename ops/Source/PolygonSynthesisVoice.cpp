/*
  ==============================================================================

    PolygonSynthesesVoice.cpp
    Created: 2 Jan 2020 6:07:28pm
    Author:  happy

  ==============================================================================
*/
#include <JuceHeader.h>
#include "PolygonSynthesisVoice.h"

bool PolygonSynthesisSound::appliesToNote( [[maybe_unused]] int midiNoteNumber )
{
	return true;
}

bool PolygonSynthesisSound::appliesToChannel( [[maybe_unused]] int midiChannel )
{
	return true;
}

bool PolygonSynthesisVoice::canPlaySound( SynthesiserSound* sound_ptr )
{
	return dynamic_cast< PolygonSynthesisSound* >( sound_ptr ) != nullptr;
}

void PolygonSynthesisVoice::startNote( int midiNoteNumber, float velocity, [[maybe_unused]] SynthesiserSound* sound, [[maybe_unused]] int currentPitchWheelPosition )
{
	currentAngle = 0.0;
	level = velocity * 0.15;
	tailOff = 0.0;

	auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz( midiNoteNumber );
	auto cyclesPerSample = cyclesPerSecond / getSampleRate();

	angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
}

void PolygonSynthesisVoice::stopNote( [[maybe_unused]] float velocity, bool allowTailOff )
{
	if( allowTailOff )
	{
		if( tailOff == 0.0 )
			tailOff = 1.0;
	}
	else
	{
		clearCurrentNote();
		angleDelta = 0.0;
	}
}

void PolygonSynthesisVoice::pitchWheelMoved( [[maybe_unused]] int newPitchWheelValue )
{
}

void PolygonSynthesisVoice::controllerMoved( [[maybe_unused]] int controllerNumber, [[maybe_unused]] int newControllerValue )
{
}

void PolygonSynthesisVoice::renderNextBlock( AudioBuffer<float>& outputBuffer, int startSample, int numSamples )
{
	if( angleDelta != 0.0 )
	{
		if( tailOff > 0.0 ) // [7]
		{
			while( --numSamples >= 0 )
			{
				auto currentSample = ( float )( std::sin( currentAngle ) * level * tailOff );

				for( auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample( i, startSample, currentSample );

				currentAngle += angleDelta;
				++startSample;

				tailOff *= 0.99; // [8]

				if( tailOff <= 0.005 )
				{
					clearCurrentNote(); // [9]

					angleDelta = 0.0;
					break;
				}
			}
		}
		else
		{
			while( --numSamples >= 0 ) // [6]
			{
				auto currentSample = ( float )( std::sin( currentAngle ) * level );

				for( auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample( i, startSample, currentSample );

				currentAngle += angleDelta;
				++startSample;
			}
		}
	}
}
