#ifndef __AUI_RECTANGLE_H__
#define __AUI_RECTANGLE_H__






inline BOOL Rectangle_Intersect( RECT *rect1, RECT *rect2 )
{
	return rect1->left < rect2->right
		&& rect1->right > rect2->left
		&& rect1->top < rect2->bottom
		&& rect1->bottom > rect2->top;
}




inline BOOL Rectangle_Subset( RECT *rect1, RECT *rect2 )
{
	return rect1->left >= rect2->left
		&& rect1->top >= rect2->top
		&& rect1->right <= rect2->right
		&& rect1->bottom <= rect2->bottom;
}




inline BOOL Rectangle_HasZeroArea( RECT *rect )
{
	return rect->left == rect->right || rect->top == rect->bottom;
}




inline sint32 Rectangle_Area( RECT *rect )
{
	return ( rect->right - rect->left ) * ( rect->bottom - rect->top );
}






BOOL Rectangle_Clip( RECT *clipee, RECT *clipper );








sint32 Rectangle_Subtract( RECT *in, RECT *sub, RECT out[4] );






BOOL Rectangle_Consolidate( RECT *rect, RECT *rect1, RECT *rect2 );









BOOL Rectangle_SmartConsolidate( RECT *rect, RECT *rect1, RECT *rect2 );








BOOL Rectangle_GetErasers(
	RECT *rect1, RECT *rect2,
	sint32 x, sint32 y,
	sint32 dx, sint32 dy,
	sint32 width, sint32 height );

#endif
