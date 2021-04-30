#ifndef __TEST_PRIMITIVES_H__
#define __TEST_PRIMITIVES_H__

#include "../../aui_common/aui_surface.h"
#include "../primitives.h"

const sint32 TEST_WIDTH  = 200;
const sint32 TEST_HEIGHT = 200;

struct ClipTriangleTest {
	uint8       * testBuffer;
	aui_Surface * testSurface;
	uint8       * lockedBuffer;
	TRIANGLE_ID   activeTriangleIdentifier;
	sint32        activeTriangleTest;
};

/*
 * Test triangle clipping - helper function to show expected outcome
 *
 * Clipping function naming:
 * TestClipTriangle<number of rectangle-corners inside triangle>_<number of triangle-corners inside rectangle>
 */
void TestClippedTriangle16(aui_Surface & surface, const RECT & rect, TRIANGLE_ID triangleId, Pixel16 color)
{
	primitives_ClippedTriangle16(surface, rect, triangleId, color);

	sint32 xOffset = (TEST_WIDTH / 8) * 5;
	sint32 yOffset = (TEST_HEIGHT / 8) * 5;
	RECT verification = RECT { xOffset + rect.left / 4, yOffset + rect.top / 4, xOffset + rect.right / 4,
	                           yOffset + rect.bottom / 4 };
	primitives_ClippedTriangle16(surface, verification, triangleId, 0x07E00, 192);
	primitives_ClippedFrame16(surface, xOffset - 1, yOffset - 1, (TEST_WIDTH / 4) + 2, (TEST_HEIGHT / 4) + 2, 0xFFF);
	primitives_ClippedPaint16(surface, xOffset, yOffset, TEST_WIDTH / 4, TEST_HEIGHT / 4, 0xC618, 92);
}

/* Test 1
 *      +----------+
 *      |          |
 *      |    /|    |
 *      |   / |    |
 *      |  /__|    |
 *      |          |
 *      |__________|
 */
void TestClipTriangle_0_3_ABC_Diagonal(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 20, 20, 60, 60 }, triangleId, 0xf800);
}

/* Test 2
 *      +----------+
 *      |          |
 *      |    /|    |
 *      |   / |    |
 *      |  /__|    |
 *      |          |
 *      |__________|
 */
void TestClipTriangle_0_3_ABC_X_Major(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 20, 20, 80, 60 }, triangleId, 0xf800);
}

/* Test 3
 *      +----------+
 *      |          |
 *      |    /|    |
 *      |   / |    |
 *      |  /__|    |
 *      |          |
 *      |__________|
 */
void TestClipTriangle_0_3_ABC_Y_Major(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 20, 20, 60, 80 }, triangleId, 0xf800);
}

/* Test 4
 *         A           F +---------+ G
 *        /|             |         |
 *       / |             |         |
 *      /  |             |         |
 *     /   |             |         |
 *  B /____| C         E |_________| H
 */
void TestClipTriangle_0_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	// top-left
	TestClippedTriangle16(surface, RECT { -60, -80, -20, -20 }, triangleId, 0xf800);
	// top
	TestClippedTriangle16(surface, RECT { 20, -80, 80, -40 }, triangleId, 0xf800);
	// top-right
	TestClippedTriangle16(surface, RECT { surface.Width() + 20, -80, surface.Width() + 60, -40 }, triangleId,
			0xf800);
	// left
	TestClippedTriangle16(surface, RECT { -80, 20, -20, 60 }, triangleId, 0xf800);
	// right
	TestClippedTriangle16(surface, RECT { surface.Width() + 20, 20, surface.Width() + 60, 60 }, triangleId,
			0xf800);
	// bottom-left
	TestClippedTriangle16(surface, RECT { -60, surface.Height() + 20, -20, surface.Height() + 80 }, triangleId,
			0xf800);
	// bottom
	TestClippedTriangle16(surface, RECT { 20, surface.Height() + 20, 60, surface.Height() + 80 }, triangleId,
			0xf800);
	// bottom-right
	TestClippedTriangle16(surface, RECT { surface.Width() + 20, surface.Height() + 20, surface.Width() + 60,
			surface.Height() + 60 }, triangleId, 0xf800);
}

/* Test 5
 *     +------------+
 *     |            |
 *     |     /|     |
 *     |    / |     |
 *     |___/__|_____|
 *        /___|
 */
void TestClipTriangle_0_1_A(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 20, surface.Height() - 20, 60, surface.Height() + 20 }, triangleId,
			0xf800);
}

/* Test 6
 *             /|
 *            / |
 *     +----+/  |
 *     |    /   |
 *     |   /|   |
 *     |  /_|___|
 *     |    |
 *     |____|
 */
void TestClipTriangle_0_1_B(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { surface.Width() - 40, -20, surface.Width() + 40, 40 }, triangleId,
			0xf800);
}

/* Test 7
 *           /|
 *          / |
 *       +-/--|--+
 *       |/   |  |
 *       /    |  |
 *      /|    |  |
 *     /_|____|  |
 *       |       |
 *       |_______|
 */
void TestClipTriangle_0_1_C(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, -20, 20, 40 }, triangleId, 0xf800);
}

/* Test 8
 *       +--------+
 *       | /|     |
 *       |/ |     |
 *       /  |     |
 *      /|  |     |
 *     /_|__|     |
 *       |        |
 *       |________|
 */
void TestClipTriangle_0_2_AC(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, 20, 40, 60 }, triangleId, 0xf800);
}

/* Test 9
 *           /|
 *          / |
 *     +---/--|--+
 *     |  /___|  |
 *     |         |
 *     |_________|
 */
void TestClipTriangle_0_2_BC(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, -20, 20, 40 }, triangleId, 0xf800);
}

/* Test 10
 *            /|
 *         +-/-|---+
 *         |/  |   |
 *         /   |   |
 *        /|   |   |
 *       / |___|___|
 *      /      |
 *     /_______|
*/
void TestClipTriangle_1_E_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, -20, 80, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 11
 *           /|
 *          / |
 *         /  |
 *      +-/-+ |
 *      |/  | |
 *      /   | |
 *     /|   | |
 *    /_|___|_|
 *      |   |
 *      |___|
 */
void TestClipTriangle_1_G_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, -20, surface.Width() + 20, 40 }, triangleId, 0xf800);
}

/* Test 12
 *             /|
 *     +---+-+/ |
 *     |     /  |
 *     |    /|  |
 *     |___/_|  |
 *        /     |
 *       /______|
 */
void TestClipTriangle_1_H_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 80, -20, surface.Width() + 20, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 13
 *        +--------+
 *        |        |
 *        | /|     |
 *        |/ |     |
 *        /  |     |
 *       /|__|_____|
 *      /    |
 *     /_____|
 */
void TestClipTriangle_1_E_1_A(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, 80, 40, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 14
 *           /|
 *          / |
 *         /  |
 *        /   |
 *       /  +-|---+
 *      /   | |   |
 *     /____|_|   |
 *          |_____|
 */
void TestClipTriangle_1_F_1_C(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -20, -20, 40, 60 }, triangleId, 0xf800);
}

/* Test 15
 *            /|
 *           / |
 *          /  |
 *     +---/-+ |
 *     |  /__|_|
 *     |     |
 *     |_____|
 */
void TestClipTriangle_1_G_1_A(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 80, -20, surface.Width() + 20, 40 }, triangleId, 0xf800);
}

/* Test 16
 *           /|
 *          / |
 *         /  |
 *        / +-|--+
 *       /  | |  |
 *      /   | |  |
 *     /    |_|__|
 *    /_______|
 */
void TestClipTriangle_2_EF_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -120, -100, 40, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 17
 *
 *     +---+ /|
 *     |   |/ |
 *     |   /  |
 *     |  /|  |
 *     | / |  |
 *     |/  |  |
 *     /   |  |
 *    /|___|  |
 *   /________|
 */
void TestClipTriangle_2_EH_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -60, -10, surface.Width() + 20, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 18
 *           /|
 *          / |
 *         /  |
 *        /   |
 *       /    |
 *      /     |
 *     /      |
 *    / +---+ |
 *   /__|___|_|
 *      |___|
 */
void TestClipTriangle_2_FG_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -100, -80, surface.Width() + 20, 20 }, triangleId, 0xf800);
}

/* Test 19
 *              /|
 *             / |
 *            /  |
 *      +----/-+ |
 *      |   /  | |
 *      |  /   | |
 *      |_/____| |
 *       /_______|
 */
void TestClipTriangle_2_GH_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { 20, -40, surface.Width() + 20, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 20
 *           /|
 *          / |
 *         /  |
 *      +-/-+ |
 *      |/  | |
 *      /   | |
 *     /|___| |
 *    /_______|
 */
void TestClipTriangle_3_EGH_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -60, -40, surface.Width() + 20, surface.Height() + 20 }, triangleId, 0xf800);
}

/* Test 21
 *            /|
 *           / |
 *          /  |
 *         /   |
 *        /+--+|
 *       / |  ||
 *      /  |__||
 *     /_______|
 */
void TestClipTriangle_4_0(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -220, -210, surface.Width() + 10, surface.Height() + 10 }, triangleId,
			0xf800);
}

/* Test 22
 *            /|
 *           / |
 *          /  |
 *         /   |
 *        /+--+|
 *       / |  ||
 *      /  |__||
 *     /_______|
 */
void TestClipTriangle_4_0_TouchingCorner(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -210, -210, surface.Width() + 10, surface.Height() + 10 }, triangleId,
	                      0xf800);
}

/* Test 23
 *            /|
 *           / |
 *          /--|--+
 *         /|  |  |
 *        / |__|__|
 *       /     |
 *      /______|
 */
void TestClipTriangle_2_EF_0_TouchingCorner(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -210, -40, 40, surface.Height() + 10 }, triangleId,
	                      0xf800);
}

/* Test 24
 *     +--+   /|
 *     |  |  / |
 *     |__| /  |
 *         /   |
 *        /    |
 *       /     |
 *      /      |
 *     /_______|
 */
void TestClipTriangle_0_0_InSurroundingRect(aui_Surface & surface, TRIANGLE_ID triangleId)
{
	TestClippedTriangle16(surface, RECT { -10, -10, surface.Width() + 220, surface.Height() + 210 }, triangleId,
	                      0xf800);
}

void CopySurface(aui_Surface & targetSurface, aui_Surface & sourceSurface)
{
	sint32 startX = (targetSurface.Width() - sourceSurface.Width()) / 2;
	sint32 startY = (targetSurface.Height() - sourceSurface.Height()) / 2;
	primitives_ClippedFrame16(targetSurface, startX - 1, startY - 1, sourceSurface.Width() + 2,
			sourceSurface.Height() + 2, 0xFFF);

	Pixel16 * targetPixel = ((Pixel16 * ) targetSurface.Buffer()) + startX + startY * targetSurface.Height();
	Pixel16 * sourcePixel = (Pixel16 *) sourceSurface.Buffer();
	sint32    targetWidth = targetSurface.Width();
	sint32    sourceWidth = sourceSurface.Width();

	Pixel16 * endTargetPixel = targetPixel + sourceSurface.Height() * targetWidth;
	while (targetPixel < endTargetPixel) {
		memcpy(targetPixel, sourcePixel, sourceWidth * sizeof(Pixel16));
		targetPixel += targetWidth;
		sourcePixel += sourceWidth;
	}
}

void ExecuteClipTriangleTest(aui_Surface & surface, TRIANGLE_ID triangleId, sint32 testIdentifier)
{
	primitives_ClippedPaint16(surface, 0, 0, surface.Width(), surface.Height(), 0x0000);

	switch (testIdentifier)
	{
		case 1:
			TestClipTriangle_0_3_ABC_Diagonal(surface, triangleId);
			break;
		case 2:
			TestClipTriangle_0_3_ABC_X_Major(surface, triangleId);
			break;
		case 3:
			TestClipTriangle_0_3_ABC_Y_Major(surface, triangleId);
			break;
		case 4:
			TestClipTriangle_0_0(surface, triangleId);
			break;
		case 5:
			TestClipTriangle_0_1_A(surface, triangleId);
			break;
		case 6:
			TestClipTriangle_0_1_B(surface, triangleId);
			break;
		case 7:
			TestClipTriangle_0_1_C(surface, triangleId);
			break;
		case 8:
			TestClipTriangle_0_2_AC(surface, triangleId);
			break;
		case 9:
			TestClipTriangle_0_2_BC(surface, triangleId);
			break;
		case 10:
			TestClipTriangle_1_E_0(surface, triangleId);
			break;
		case 11:
			TestClipTriangle_1_G_0(surface, triangleId);
			break;
		case 12:
			TestClipTriangle_1_H_0(surface, triangleId);
			break;
		case 13:
			TestClipTriangle_1_E_1_A(surface, triangleId);
			break;
		case 14:
			TestClipTriangle_1_F_1_C(surface, triangleId);
			break;
		case 15:
			TestClipTriangle_1_G_1_A(surface, triangleId);
			break;
		case 16:
			TestClipTriangle_2_EF_0(surface, triangleId);
			break;
		case 17:
			TestClipTriangle_2_EH_0(surface, triangleId);
			break;
		case 18:
			TestClipTriangle_2_FG_0(surface, triangleId);
			break;
		case 19:
			TestClipTriangle_2_GH_0(surface, triangleId);
			break;
		case 20:
			TestClipTriangle_3_EGH_0(surface, triangleId);
			break;
		case 21:
			TestClipTriangle_4_0(surface, triangleId);
			break;
		case 22:
			TestClipTriangle_4_0_TouchingCorner(surface, triangleId);
			break;
		case 23:
			TestClipTriangle_2_EF_0_TouchingCorner(surface, triangleId);
			break;
		case 24:
			TestClipTriangle_0_0_InSurroundingRect(surface, triangleId);
			break;
	}
}

ClipTriangleTest * CreateClipTriangleTest()
{
	ClipTriangleTest * clipTriangleTest = new ClipTriangleTest();
	clipTriangleTest->testBuffer = (uint8 *)malloc(TEST_WIDTH * TEST_HEIGHT * 2);

	AUI_ERRCODE err;
	clipTriangleTest->testSurface = new aui_Surface(&err, TEST_WIDTH, TEST_HEIGHT, 16, 2 * TEST_WIDTH, clipTriangleTest->testBuffer);
	clipTriangleTest->testSurface->Lock(NULL, (LPVOID *)&(clipTriangleTest->lockedBuffer), 0);

	clipTriangleTest->activeTriangleIdentifier = TI_RIGHT_BOTTOM;
	clipTriangleTest->activeTriangleTest = 0;

	return clipTriangleTest;
}

void DestroyClipTriangleTest(ClipTriangleTest * clipTriangleTest)
{
	clipTriangleTest->testSurface->Unlock(clipTriangleTest->lockedBuffer);
	delete clipTriangleTest->testSurface;
	free(clipTriangleTest->testBuffer);
	delete clipTriangleTest;
}

bool DoTestClipTriangle(aui_Surface & visualSurface, ClipTriangleTest * clipTriangleTest)
{
	static const sint32 LAST_TEST_IDENTIFIER = 24;

	Assert(clipTriangleTest);
	if (!clipTriangleTest) {
		return false;
	}

	if (clipTriangleTest->activeTriangleTest == LAST_TEST_IDENTIFIER && clipTriangleTest->activeTriangleIdentifier == TI_RIGHT_TOP)
	{
		// Testing done
		return false;
	}

	// next test
	if (clipTriangleTest->activeTriangleTest == LAST_TEST_IDENTIFIER)
	{
		clipTriangleTest->activeTriangleTest = 1;
		switch (clipTriangleTest->activeTriangleIdentifier) {
			case TI_RIGHT_BOTTOM:
				clipTriangleTest->activeTriangleIdentifier = TI_LEFT_BOTTOM;
				break;
			case TI_LEFT_BOTTOM:
				clipTriangleTest->activeTriangleIdentifier = TI_LEFT_TOP;
				break;
			case TI_LEFT_TOP:
				clipTriangleTest->activeTriangleIdentifier = TI_RIGHT_TOP;
				break;
			default:
				Assert(false);
		}
	}
	else
	{
		clipTriangleTest->activeTriangleTest++;
	}

	ExecuteClipTriangleTest(*(clipTriangleTest->testSurface), clipTriangleTest->activeTriangleIdentifier,
			clipTriangleTest->activeTriangleTest);

	{
		char testNumberBuffer[20];
		sprintf(testNumberBuffer, "%d", clipTriangleTest->activeTriangleTest);
		primitives_DrawText(clipTriangleTest->testSurface, 10, 10, testNumberBuffer, 0xFFFFFFFF, false);
	}

	CopySurface(visualSurface, *(clipTriangleTest->testSurface));
	return true;
}

void TestClipRectangle(aui_Surface & visualSurface)
{
	Assert(visualSurface.Width() > TEST_WIDTH);
	Assert(visualSurface.Height() > TEST_HEIGHT);

	static ClipTriangleTest * s_ClipTriangleTest = NULL;
	static bool s_TestDone = false;
	if (!s_ClipTriangleTest && !s_TestDone) {
		s_ClipTriangleTest = CreateClipTriangleTest();
	}

	if (s_ClipTriangleTest)
	{
		uint8 *pSurfBase;
		visualSurface.Lock(NULL, (LPVOID *)&pSurfBase, 0);
		if (!DoTestClipTriangle(visualSurface, s_ClipTriangleTest))
		{
			DestroyClipTriangleTest(s_ClipTriangleTest);
			s_ClipTriangleTest = NULL;
			s_TestDone = true;
		}
		visualSurface.Unlock(pSurfBase);
	}
}
#endif
