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
        Point Pow2(){
            return { x * x, y * y };
        }
        double Lenght(){
            return std::sqrt( x*x + y*y );
        } 
        double Lenght2(){
            return x*x + y*y ;
        }
        Point& operator+= ( const Point& b ){
            this->x += b.x;
            this->y += b.y;
            return *this;
        }
    
    };
    Point operator- ( const Point& a, const Point& b );

    using Line = struct{ Point a; Point b; };

    // row major
    struct Mat22{
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

struct PolygonState
{
    int index;
    double lenght;
    double rotation;
};

class Polygon
{
public:
	Polygon();
    Polygon( const uint8 max_line_count );
    void SetDrawPercentage( const double percentage );
    void Circularize();
    void Rotate( const double amount);
    void Collapse( const float amount );
    void CalcLength();

    void Begin();
    const ops::Point Sample( const double step );
    ~Polygon();
    std::vector<ops::Line> lines;

private:
    double draw_percentage;
    double draw_lenght;
};
