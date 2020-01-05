/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


static MidiBuffer filterMidiMessagesForChannel( const MidiBuffer& input, int channel )
{
	MidiMessage msg;
	int samplePosition;
	MidiBuffer output;

	for( MidiBuffer::Iterator it( input ); it.getNextEvent( msg, samplePosition );)
		if( msg.getChannel() == channel ) output.addEvent( msg, samplePosition );

	return output;
}

//==============================================================================
OpsAudioProcessor::OpsAudioProcessor() :
	max_voices( 8 ),
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor( BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput( "Input", AudioChannelSet::stereo(), true )
#endif
		.withOutput( "Output", AudioChannelSet::stereo(), true )
#endif
	)
#endif
{
	addParameter( freq_ratio = new AudioParameterFloat( "freq ratio", "freq ratio", 0.1f, 8.0f, 1.0f) );
	addParameter( vertex_count = new AudioParameterFloat( "vertex count", "vertex count", 2.0f, 32.0f, 2.0f) );
	addParameter( rotation_speed = new AudioParameterFloat( "rotation speed", "rotation speed", 0.0f, 20.0f * juce::MathConstants<float>::pi , 0.0f) );
	addParameter( collapse = new AudioParameterFloat( "collapse", "collapse", 0.0f, 1.0f, 0.0f) );
	addParameter( expand = new AudioParameterFloat( "expand", "expand", 0.0f, 1.0f, 0.0f) );

	voice_parameters = std::make_unique<VoiceParameters>();
	synth = std::make_unique<Synthesiser>();
	for( size_t i = 0; i < max_voices; ++i)
	{
		synth->addVoice( new PolygonSynthesisVoice(voice_parameters.get()) );
	}
	synth->addSound( new PolygonSynthesisSound() );
}

OpsAudioProcessor::~OpsAudioProcessor()
{
}

//==============================================================================
const String OpsAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool OpsAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool OpsAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool OpsAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double OpsAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int OpsAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int OpsAudioProcessor::getCurrentProgram()
{
	return 0;
}

void OpsAudioProcessor::setCurrentProgram( [[maybe_unused]] int index )
{
}

const String OpsAudioProcessor::getProgramName( [[maybe_unused]] int index )
{
	return {};
}

void OpsAudioProcessor::changeProgramName( [[maybe_unused]] int index, [[maybe_unused]] const String& newName )
{
}

//==============================================================================
void OpsAudioProcessor::prepareToPlay( double sampleRate, [[maybe_unused]] int samplesPerBlock )
{
	synth->setCurrentPlaybackSampleRate( sampleRate );
}

void OpsAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OpsAudioProcessor::isBusesLayoutSupported( const BusesLayout& layouts ) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused( layouts );
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if( layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo() )
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if( layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet() )
		return false;
#endif

	return true;
#endif
}
#endif

void OpsAudioProcessor::processBlock( AudioBuffer<float>& buffer, [[maybe_unused]] MidiBuffer& midiMessages )
{
	voice_parameters->collapse = collapse->get();
	voice_parameters->expand = expand->get();
	voice_parameters->freq_ratio = freq_ratio->get();
	voice_parameters->rotation_speed = rotation_speed->get();
	voice_parameters->vertex_count = vertex_count->get();

	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for( auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i )
		buffer.clear( i, 0, buffer.getNumSamples() );


	auto midiChannelBuffer = filterMidiMessagesForChannel( midiMessages, 1 );
	auto audioBusBuffer = getBusBuffer( buffer, false, 0 );

	synth->renderNextBlock( audioBusBuffer, midiChannelBuffer, 0, audioBusBuffer.getNumSamples() );
}

//==============================================================================
bool OpsAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OpsAudioProcessor::createEditor()
{
	return new GenericAudioProcessorEditor( *this );
}

//==============================================================================
void OpsAudioProcessor::getStateInformation( [[maybe_unused]] MemoryBlock& destData )
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void OpsAudioProcessor::setStateInformation( [[maybe_unused]] const void* data, [[maybe_unused]] int sizeInBytes )
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new OpsAudioProcessor();
}
