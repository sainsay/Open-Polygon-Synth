/*
  ==============================================================================

	Polygon.cpp
	Created: 4 Jan 2020 12:20:35pm
	Author:  Jens Hagen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Polygon.h"


Polygon::Polygon() :
	draw_percentage( 1.0 ),
	draw_lenght( 0.0 )
{
	lines.resize( 32 );
}

Polygon::Polygon( const uint8 max_line_count ) :
	draw_percentage( 1.0 ),
	draw_lenght( 0.0 )
{
	lines.resize( max_line_count );
}

void Polygon::SetDrawPercentage( const double percentage )
{
	draw_percentage = percentage;
}

void Polygon::Circularize()
{
	auto arr_size = lines.size();
	auto to_draw = double( arr_size ) * draw_percentage;
	double loop_amount;
	double frac = std::modf( to_draw, &loop_amount );
	draw_lenght = 0.0;
	auto rot = juce::MathConstants<double>::twoPi;
	auto step_size = juce::MathConstants<double>::twoPi / to_draw;
	for( int i = 0; i < int( loop_amount ); ++i ){
		auto x = std::cos( rot );
		auto y = std::sin( rot );
		lines[i].a = { x,y };
		rot -= step_size;
		x = std::cos( rot );
		y = std::sin( rot );
		lines[i].b = { x,y };
		draw_lenght += ( lines[i].a - lines[i].b ).Lenght();

	}
	if( frac > 0.0 )
	{
		auto x = std::cos( rot );
		auto y = std::sin( rot );
		lines[int( loop_amount ) + 1].a = { x,y };
		rot -= step_size * frac;
		x = std::cos( rot );
		y = std::sin( rot );
		lines[int( loop_amount ) + 1].a = { x,y };
		draw_lenght += ( lines[int( loop_amount ) + 1].a - lines[int( loop_amount ) + 1].b ).Lenght();
	}
}

void Polygon::Rotate( [[maybe_unused]] const double amount )
{
}

const ops::Point Polygon::Sample( const double rotation ) const
{
	auto sample_point = draw_lenght * ( rotation / juce::MathConstants<double>::twoPi );
	sample_point *= sample_point; // gets rid of many sqrt()
	auto to_draw = std::ceil( double( lines.size() ) * draw_percentage);
	int i = 0;
	double len2 = 0.0;
	len2 = ( lines[i].a - lines[i].b ).Lenght2();
	for( ; sample_point > len2; i++ )
	{
		if( i >= to_draw )
		{
			i = 0;
		}
		len2 = ( lines[i].a - lines[i].b ).Lenght2();
		sample_point -= len2;
	}
	ops::Line sample_line = lines[i];
	auto frac = sample_point / ( lines[i].a - lines[i].b ).Lenght2();

	// lerp to point
	ops::Point result = { std::lerp( sample_line.a.x, sample_line.b.x, frac ), std::lerp( sample_line.a.y, sample_line.b.y, frac ) };
	return result;
}

Polygon::~Polygon()
{
}

ops::Point ops::operator-( const ops::Point a, const ops::Point b )
{
	return { a.x - b.x, a.y - b.y };
}
