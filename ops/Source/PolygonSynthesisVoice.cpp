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
	temp_env.ResetState();
	currentAngle = 0.0;
	level = velocity * 0.15;
	tailOff = 0.0;

	auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz( midiNoteNumber );
	auto cyclesPerSample = cyclesPerSecond / getSampleRate();

	angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
}

void PolygonSynthesisVoice::stopNote( [[maybe_unused]] float velocity, [[maybe_unused]] bool allowTailOff )
{
	temp_env.SetState( env::AttRelEnv::State::Release );
}

void PolygonSynthesisVoice::pitchWheelMoved( [[maybe_unused]] int newPitchWheelValue )
{
}

void PolygonSynthesisVoice::controllerMoved( [[maybe_unused]] int controllerNumber, [[maybe_unused]] int newControllerValue )
{
}

void PolygonSynthesisVoice::renderNextBlock( AudioBuffer<float>& outputBuffer, int startSample, int numSamples )
{
	if( outputBuffer.getNumChannels() < 1 )
	{
		return;
	}

	polygon.SetDrawPercentage( voice_parameters->vertex_count / double( polygon.lines.size() ) );

	if( angleDelta != 0.0 )
	{
		auto corrected_delta = angleDelta * voice_parameters->freq_ratio;

		while( --numSamples >= 0 )
		{
			temp_env.Update( 1.0 / getSampleRate() );
			float amp = temp_env.Sample();

			ops::Point sample_point = polygon.Sample( currentAngle, voice_parameters->rotation_speed / getSampleRate(), voice_parameters->collapse, voice_parameters->expand );
			auto current_sample_l = float( sample_point.x * level * amp );
			auto current_sample_r = float( sample_point.y * level * amp );

			if( outputBuffer.getNumChannels() > 1 )
			{
				outputBuffer.addSample( 0, startSample, current_sample_l );
				outputBuffer.addSample( 1, startSample, current_sample_r );
			}
			else{
				outputBuffer.addSample( 0, startSample, current_sample_l );
			}

			currentAngle += corrected_delta;
			currentAngle = currentAngle > juce::MathConstants<double>::twoPi ? currentAngle - juce::MathConstants<double>::twoPi : currentAngle;
			++startSample;

		}
	}
}
