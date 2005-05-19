
#include "c3.h"
#include "geom2d.h"


#include "Memory_Manager.h"

extern Memory_Manager *g_memory_Point2d; 

void* Point2d::operator new(size_t byte_size)
{
    Assert(sizeof(Point2d)==byte_size);
    return g_memory_Point2d->Alloc(); 
}
void Point2d::operator delete(void *ptr)
{
    g_memory_Point2d->Dealloc(ptr); 
}

Real Vector2d::norm() const
{
	return sqrt(x * x + y * y);
}

inline void Vector2d::normalize()
{
	Real len;

    len = sqrt(x * x + y * y);

	Assert(0.0 < len);

	x /= len;
	y /= len;
}

inline Vector2d Vector2d::operator+(const Vector2d& v) const
{
	return Vector2d(x + v.x, y + v.y);
}

inline Vector2d Vector2d::operator-(const Vector2d& v) const
{
	return Vector2d(x - v.x, y - v.y);
}

inline Vector2d operator*(Real c, const Vector2d& v)
{
	return Vector2d(c * v.x, c * v.y);
}

inline Real dot(const Vector2d& u, const Vector2d& v)
{
	return u.x * v.x + u.y * v.y;
}




inline Point2d Point2d::operator+(const Vector2d& v) const
{
	return Point2d(x + v.x, y + v.y);
}

Vector2d Point2d::operator-(const Point2d& p) const
{
	return Vector2d(x - p.x, y - p.y);
}

int Point2d::operator==(const Point2d& p) const
{
	return ((*this - p).norm() < EPS);
}



Line::Line(const Point2d& p, const Point2d& q)


{
    
	Vector2d t = q - p;
	Real len = t.norm();
	a =   t.y / len;
	b = - t.x / len;
	c = -(a*p.x + b*p.y);
}

Real Line::eval(const Point2d& p) const

{
	return (a * p.x + b* p.y + c);
}

int Line::classify(const Point2d& p) const


{
	Real d = eval(p);
	return (d < -EPS) ? -1 : (d > EPS ? 1 : 0);
}
 