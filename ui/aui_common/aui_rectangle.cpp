

















#include "c3.h"
#include "aui_rectangle.h"






BOOL Rectangle_Clip( RECT *clipee, RECT *clipper )
{
	BOOL hasNonzeroArea = TRUE; 

	
	if ( clipee->right < clipper->left )
	{
		clipee->left = clipee->right = clipper->left;
		hasNonzeroArea = FALSE;
	}
	else
	{
		
		if ( clipee->left > clipper->right )
		{
			clipee->left = clipee->right = clipper->right;
			hasNonzeroArea = FALSE;
		}
		else
		{
			
			if ( clipee->left < clipper->left )
				clipee->left = clipper->left;

			
			if ( clipee->right > clipper->right )
				clipee->right = clipper->right;
		}
	}

	
	if ( clipee->bottom < clipper->top )
	{
		clipee->top = clipee->bottom = clipper->top;
		hasNonzeroArea = FALSE;
	}
	else
	{
		
		if ( clipee->top > clipper->bottom )
		{
			clipee->top = clipee->bottom = clipper->bottom;
			hasNonzeroArea = FALSE;
		}
		else
		{
			
			if ( clipee->top < clipper->top )
				clipee->top = clipper->top;

			
			if ( clipee->bottom > clipper->bottom )
				clipee->bottom = clipper->bottom;
		}
	}

	return hasNonzeroArea;
}








sint32 Rectangle_Subtract( RECT *in, RECT *sub, RECT out[4] )
{
	
	Assert( in != NULL );
	if ( !in ) return -1;

	
	Assert( in->left <= in->right && in->top <= in->bottom );
	if ( in->left >= in->right || in->top >= in->bottom ) return 0;

	
	Assert( sub != NULL );
	if ( !sub ) return -1;

	
	Assert( sub->left <= sub->right && sub->top <= sub->bottom );
	if ( sub->left >= sub->right || sub->top >= sub->bottom
	||
	
	
	   ( in->left >= sub->right
	||   in->top >= sub->bottom
	||   in->right <= sub->left
	||   in->bottom <= sub->top ) )
	{
		CopyRect( out, in );
		return 1;
	}

	
	
	Assert( out != NULL );
	if ( !out ) return -1;

	if ( in->left < sub->left )
	{
		if ( in->top < sub->top )
		{
			if ( in->right > sub->right )
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, in->left, sub->top, sub->left, sub->bottom );
					SetRect( ++out, sub->right, sub->top, in->right, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 4;
				}
				else
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, in->left, sub->top, sub->left, in->bottom );
					SetRect( ++out, sub->right, sub->top, in->right, in->bottom );
					return 3;
				}
			}
			else
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, in->left, sub->top, sub->left, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 3;
				}
				else
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, in->left, sub->top, sub->left, in->bottom );
					return 2;
				}
			}
		}
		else
		{
			if ( in->right > sub->right )
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, sub->left, sub->bottom );
					SetRect( ++out, sub->right, in->top, in->right, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 3;
				}
				else
				{
					SetRect( out, in->left, in->top, sub->left, in->bottom );
					SetRect( ++out, sub->right, in->top, in->right, in->bottom );
					return 2;
				}
			}
			else
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, sub->left, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 2;
				}
				else
				{
					SetRect( out, in->left, in->top, sub->left, in->bottom );
					return 1;
				}
			}
		}
	}
	else
	{
		if ( in->top < sub->top )
		{
			if ( in->right > sub->right )
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, sub->right, sub->top, in->right, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 3;
				}
				else
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, sub->right, sub->top, in->right, in->bottom );
					return 2;
				}
			}
			else
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 2;
				}
				else
				{
					SetRect( out, in->left, in->top, in->right, sub->top );
					return 1;
				}
			}
		}
		else
		{
			if ( in->right > sub->right )
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, sub->right, in->top, in->right, sub->bottom );
					SetRect( ++out, in->left, sub->bottom, in->right, in->bottom );
					return 2;
				}
				else
				{
					SetRect( out, sub->right, in->top, in->right, in->bottom );
					return 1;
				}
			}
			else
			{
				if ( in->bottom > sub->bottom )
				{
					SetRect( out, in->left, sub->bottom, in->right, in->bottom );
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
	}
}






BOOL Rectangle_Consolidate( RECT *rect, RECT *rect1, RECT *rect2 )
{
	if ( Rectangle_HasZeroArea( rect1 ) )
	{
		*rect = *rect2;
		return FALSE;
	}

	if ( Rectangle_HasZeroArea( rect2 ) )
	{
		*rect = *rect1;
		return FALSE;
	}

	rect->left =
		rect1->left < rect2->left ? rect1->left : rect2->left;

	rect->top =
		rect1->top < rect2->top ? rect1->top : rect2->top;

	rect->right =
		rect1->right > rect2->right ? rect1->right : rect2->right;

	rect->bottom =
		rect1->bottom > rect2->bottom ? rect1->bottom : rect2->bottom;

	return TRUE;
}









BOOL Rectangle_SmartConsolidate( RECT *rect, RECT *rect1, RECT *rect2 )
{
	
	if ( !Rectangle_Intersect( rect1, rect2 ) )
	{
		
		if ( rect1->left == rect2->left && rect1->right == rect2->right )
		{
			if ( rect1->bottom == rect2->top )
			{
				rect->left = rect1->left;
				rect->top = rect1->top;
				rect->right = rect1->right;
				rect->bottom = rect2->bottom;
			}
			else if ( rect1->top == rect2->bottom )
			{
				rect->left = rect1->left;
				rect->top = rect2->top;
				rect->right = rect1->right;
				rect->bottom = rect1->bottom;
			}
			else
				return FALSE;
		}
		else if ( rect1->top == rect2->top && rect1->bottom == rect2->bottom )
		{
			if ( rect1->right == rect2->left )
			{
				rect->left = rect1->left;
				rect->top = rect1->top;
				rect->right = rect2->right;
				rect->bottom = rect1->bottom;
			}
			else if ( rect1->left == rect2->right )
			{
				rect->left = rect2->left;
				rect->top = rect1->top;
				rect->right = rect1->right;
				rect->bottom = rect1->bottom;
			}
			else
				return FALSE;
		}
		else
			return FALSE;

		return TRUE;
	}

	
	
	Rectangle_Consolidate( rect, rect1, rect2 );
	return 
		Rectangle_Area( rect ) <=
		Rectangle_Area( rect1 ) +
		Rectangle_Area( rect2 );
}








BOOL Rectangle_GetErasers(
	RECT *rect1, RECT *rect2,
	sint32 x, sint32 y,
	sint32 dx, sint32 dy,
	sint32 width, sint32 height )
{
	if ( dx >= width || dx <= -width || dy >= height || dy <= -height )
	{
		rect1->left = x - dx;
		rect1->top = y - dy;
		rect1->right = rect1->left + width;
		rect1->bottom = rect1->top + height;
		rect2->left = rect2->top = rect2->right = rect2->bottom = 0;
	}
	else if ( dx > 0 )
	{
		if ( dy > 0 )
		{
			rect1->left = x - dx;
			rect1->top = y - dy;
			rect1->right = x;
			rect1->bottom = y + height - dy;
			rect2->left = x;
			rect2->top = y - dy;
			rect2->right = x + width - dx;
			rect2->bottom = y;
		}
		else
		{
			rect1->left = x - dx;
			rect1->top = y - dy;
			rect1->right = x;
			rect1->bottom = y + height - dy;
			rect2->left = x;
			rect2->top = y + height;
			rect2->right = x + width - dx;
			rect2->bottom = y + height - dy;
		}
	}
	else
	{
		if ( dy > 0 )
		{
			rect1->left = x + width;
			rect1->top = y - dy;
			rect1->right = x + width - dx;
			rect1->bottom = y + height - dy;
			rect2->left = x - dx;
			rect2->top = y - dy;
			rect2->right = x + width;
			rect2->bottom = y;
		}
		else
		{
			rect1->left = x + width;
			rect1->top = y - dy;
			rect1->right = x + width - dx;
			rect1->bottom = y + height - dy;
			rect2->left = x - dx;
			rect2->top = y + height;
			rect2->right = x + width;
			rect2->bottom = y + height - dy;
		}
	}

	return TRUE;
}
