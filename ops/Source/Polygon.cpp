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
	draw_lenght( 0.0 ),
	rotation(0.0),
	index(0)
{
	lines.resize( 32 );
}

Polygon::Polygon( const uint8 max_line_count ) :
	draw_percentage( 1.0 ),
	draw_lenght( 0.0 ),
	rotation(0.0),
	index(0)
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
	}
	if( frac > 0.0 )
	{
		auto x = std::cos( rot );
		auto y = std::sin( rot );
		lines[int( loop_amount )].a = { x,y };
		rot -= step_size * frac;
		x = std::cos( rot );
		y = std::sin( rot );
		lines[int( loop_amount )].b = { x,y };
	}
}

void Polygon::Rotate( [[maybe_unused]] const double amount )
{
	rotation += amount;
	double temp;
	rotation = std::modf( rotation, &temp );

	auto rot_mat = ops::Mat22( rotation );
	int to_rot = int( std::ceil( double( lines.size() ) * draw_percentage ) );

	for( int i = 0; i < to_rot; ++i )
	{
		lines[i].a = lines[i].a * rot_mat;
		lines[i].b = lines[i].b * rot_mat;
	}
}

void Polygon::Collapse( const float value )
{
	float amount = std::abs( value - 1.0f );
	int to_draw = int(std::ceil( double( lines.size() ) * draw_percentage ));
	for( size_t i = 0; i < to_draw; i++ )
	{
		lines[i].a.x *= amount;
		lines[i].a.y *= amount;
	}
}

void Polygon::CalcLength()
{
	draw_lenght = 0.0;
	int to_draw = int( std::ceil( double( lines.size() ) * draw_percentage ) );
	for( size_t i = 0; i < to_draw; i++ )
	{
		draw_lenght += ( lines[i].a - lines[i].b ).Lenght();
	}
}

void Polygon::Begin()
{
}

const ops::Point Polygon::Sample( const double sample_rotation )
{
	auto sample_point = draw_lenght * ( sample_rotation / juce::MathConstants<double>::twoPi );
	int to_draw = int(std::ceil( double( lines.size() ) * draw_percentage));
	
	int i = 0;
	do
	{
		double len = ( lines[i].a - lines[i].b ).Lenght();
		if( sample_point > len )
		{
			sample_point -= len;
			i++;
			if( i >= (to_draw) )
			{
				i = 0;
			}
		}
		else
		{
			break;
		}
	} while( true );
	ops::Line sample_line = lines[i];
	auto frac = sample_point / ( lines[i].a - lines[i].b ).Lenght();
	// lerp to point
	ops::Point result = { std::lerp( sample_line.a.x, sample_line.b.x, frac ), std::lerp( sample_line.a.y, sample_line.b.y, frac ) };
	return result;
}

Polygon::~Polygon()
{
}

ops::Point ops::operator-( const ops::Point& a, const ops::Point& b )
{
	return { a.x - b.x, a.y - b.y };
}

ops::Point ops::operator*( const ops::Point& a, const ops::Mat22& b )
{
	return { a.x * b.v[0] + a.y * b.v[1], a.x * b.v[2] + a.y * b.v[3] };
}

ops::Point ops::operator*( const ops::Mat22& a, const ops::Point& b )
{
	return b*a;
}
