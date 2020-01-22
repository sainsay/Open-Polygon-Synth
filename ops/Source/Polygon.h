/*
  ==============================================================================

	Polygon.h
	Created: 4 Jan 2020 12:20:35pm
	Author:  Jens Hagen

  ==============================================================================
*/

#pragma once
namespace ops
{
	struct Point {
		double x, y;
		Point Pow2() const {
			return { x * x, y * y };
		}
		double Lenght() const{
			return std::sqrt( x * x + y * y );
		}
		double Lenght2() const{
			return x * x + y * y;
		}
		Point& operator+= ( const Point& b ){
			this->x += b.x;
			this->y += b.y;
			return *this;
		}

	};
	Point operator- ( const Point& a, const Point& b );
	Point operator* ( const Point& a, const float v );
	Point GetNormal( const Point& p );

	using Line = struct{ Point a; Point b; };

	// row major
	struct Mat22{
		Mat22() = default;

		Mat22( const double rot ){
			v[0] = cos( rot );
			v[1] = -sin( rot );
			v[2] = sin( rot );
			v[3] = v[0];
		}
		double v[4];
	};
	Point operator* ( const Point& a, const Mat22& b );
	Point operator* ( const Mat22& a, const Point& b );
}

struct GlobalPolygonState
{
};

class Polygon
{
public:
	Polygon();
	Polygon( const uint8 max_line_count );
	void SetDrawPercentage( const double percentage );
	void Circularize();
	void Rotate( const double amount );
	void Collapse( const float amount );
	void CalcLength();

	void Begin();
	const ops::Point Sample( const double step );
	const ops::Point Sample( const double step, const double rot_step, const float collapse, const float expand );
	~Polygon();
	std::vector<ops::Line> lines;

private:

	double draw_percentage;
	double draw_lenght;
	double rotation;
	int index;



};
