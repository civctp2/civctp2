










#ifndef __AUI_ANIMATE_H__
#define __AUI_ANIMATE_H__



class aui_Animate
{
public:
	
	struct DPOINT
	{
		double x;
		double y;
	};

	static void Hermite(
		POINT *thePoint,
		double t,
		const POINT *keyFrame,
		const int nNumFrames );
};


#endif 
