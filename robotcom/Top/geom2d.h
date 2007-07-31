#pragma once 

#ifndef __GEOM2D_H__
#define __GEOM2D_H__ 1




#ifndef ABS
#define ABS(a)	((a) >= 0 ? (a) : -(a))
#endif

#ifndef MAX
#define MAX(a, b)	((a) >= (b) ? (a) : (b))
#define MIN(a, b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define EPS 1e-6

typedef double	Real;


class istream; 
class ostream; 

class Vector2d {
public:
	Real x, y;
    Vector2d()					{ x = 0; y = 0; }
	Vector2d(Real a, Real b)	{ x = a; y = b; }
	Real norm() const;
	void normalize();
	Vector2d operator+(const Vector2d&) const;
	Vector2d operator-(const Vector2d&) const;
	friend Vector2d operator*(Real, const Vector2d&);
	friend Real dot(const Vector2d&, const Vector2d&);



};

class Point2d {
public:
	Real x, y;
	Point2d()					{ x = 0; y = 0; }
	Point2d(Real a, Real b)		{ x = a; y = b; }
	Point2d(const Point2d& p)	{ *this = p; }
	Point2d operator+(const Vector2d&) const;
	Vector2d operator-(const Point2d&) const;
	int operator==(const Point2d&) const;



    
    void* operator new(size_t); 
    void operator delete(void *);

};

class Line {
public:
    Line()	{}
	Line(const Point2d&, const Point2d&);
	Real eval(const Point2d&) const;
	int classify(const Point2d&) const;

private:
	Real a, b, c;
};

#endif __GEOM2D_H__

