










#include "c3.h"

#include "aui_animate.h"







void aui_Animate::Hermite(
	POINT *thePoint,
	double t,
	const POINT *keyFrame,
	const int nNumFrames )
{
	if ( nNumFrames == 2 )
	{
		

		double effect = t*t*t; 
		thePoint->x = int(
			( 1.0 - effect )*(double)keyFrame[0].x +
			effect*(double)keyFrame[1].x + 0.5 );
		thePoint->y = int(
			( 1.0 - effect )*(double)keyFrame[0].y +
			effect*(double)keyFrame[1].y + 0.5 );
	}

	else
	{
		

		DPOINT D0;
		DPOINT D1;

		int i = (int)t;

		if ( i == 0 )
		{
			D0.x = double(keyFrame[1].x - keyFrame[0].x);
			D0.y = double(keyFrame[1].y - keyFrame[0].y);
			D1.x = double(keyFrame[2].x - keyFrame[0].x)/2.0;
			D1.y = double(keyFrame[2].y - keyFrame[0].y)/2.0;
		}
		else if ( i < nNumFrames - 2 )
		{
			D0.x = double(keyFrame[i + 1].x - keyFrame[i - 1].x)/2.0;
			D0.y = double(keyFrame[i + 1].y - keyFrame[i - 1].y)/2.0;
			D1.x = double(keyFrame[i + 2].x - keyFrame[i].x)/2.0;
			D1.y = double(keyFrame[i + 2].y - keyFrame[i].y)/2.0;
		}
		else if ( i == nNumFrames - 2 )
		{
			D0.x = double(keyFrame[i + 1].x - keyFrame[i - 1].x)/2.0;
			D0.y = double(keyFrame[i + 1].y - keyFrame[i - 1].y)/2.0;
			D1.x = double(keyFrame[i + 1].x - keyFrame[i].x);
			D1.y = double(keyFrame[i + 1].y - keyFrame[i].y);
		}
		else
		{
			thePoint->x = keyFrame[i].x;
			thePoint->y = keyFrame[i].y;
			return;
		}

		t -= (double)i;

		
		double f1 = 2.0*t*t*t - 3.0*t*t + 1.0;
		double f2 = t*t*t - 2.0*t*t + t;
		double f3 = t*t*t - t*t;
		double f4 = -2.0*t*t*t + 3.0*t*t;

		thePoint->x = int(
			(double)keyFrame[i].x*f1 +
			D0.x*f2 +
			D1.x*f3 +
			(double)keyFrame[i + 1].x*f4 + 0.5 );
		thePoint->y = int(
			(double)keyFrame[i].y*f1 +
			D0.y*f2 +
			D1.y*f3 +
			(double)keyFrame[i + 1].y*f4 + 0.5 );
	}
}
