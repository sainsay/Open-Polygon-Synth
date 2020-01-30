/*
  ==============================================================================

	ModulationSources.h
	Created: 28 Jan 2020 7:29:41am
	Author:  Jens Hagen

  ==============================================================================
*/

#pragma once
#include <tuple>
#include <cmath>

namespace env{
	class AttRelEnv
	{
	public:
		enum class State{
			Attack = 0,
			Release,
			Sustain
		};

		AttRelEnv() :
			attack_time( 0.05f ),
			release_time( 0.5f ),
			has_sustain( true ),
			current_state(State::Attack),
			current_step(0.0f)
		{
		}

		AttRelEnv( float att, float rel, bool sus ) :
			attack_time( att ),
			release_time( rel ),
			has_sustain( sus ),
			current_state( State::Attack ),
			current_step( 0.0f )
		{

		}

		void SetValues( float att, float rel, bool sus ){
			attack_time = att;
			release_time = rel;
			has_sustain = sus;
		}

		void ResetState(){
			current_state = State::Attack;
			current_step = 0.0f;
		}

		auto CurrentState() const
		{
			return std::make_tuple(current_state, current_step);
		}

		float Sample() const {
			switch( current_state )
			{
			case State::Attack:
				return float(std::lerp( 0.0, 1.0, current_step / attack_time ));
				break;
			case State::Release:
				return float(std::lerp( 1.0, 0.0, current_step / release_time ));
				break;
			case State::Sustain:
				return 1.0f;
				break;
			default:
				break;
			}
			return 0.0f;
		}

		void SetState(State state){
			current_state = state;
			current_step = 0.0;
		}

		void Update( double step_size ){
			current_step += step_size;
			if( current_state == State::Attack && current_step > attack_time )
			{
				current_step = 0.0;
				if( has_sustain )
				{
					current_state = State::Sustain;
				}
				else
				{
					current_state = State::Release;
				}
			}
			else if( current_state == State::Release && current_step > release_time )
			{
				current_step = release_time;
			}
			
		}

		~AttRelEnv()
		{
		}

	private:
		State current_state;
		double current_step;

		float attack_time;
		float release_time;
		bool has_sustain;


	};

}

