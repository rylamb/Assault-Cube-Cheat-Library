#include "pch.h"
#include "glText.h"

/* Citation
https://guidedhacking.com/threads/how-to-get-started-with-opengl-hacks.11475/
*/

//Builds the set of ASCII chars we're going to use for text
void GL::Font::Build(int height)
{
	hdc = wglGetCurrentDC();	//Get current device context
	hglrc = wglGetCurrentContext();	//Get current rendering context
	base = glGenLists(96);	//Generate GL list of size 96
	HFONT hFont = CreateFontA(-height, 0, 0, 0,
		FW_MEDIUM, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Consolas");	//Creates our desired font - font we're using is named "Consolas"
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);	//Sets our new font and stores the old font for restoring later
	wglUseFontBitmaps(hdc, 32, 96, base);	//Chooses the ASCII chars we want to be using
	SelectObject(hdc, hOldFont);	//Set our old font back
	DeleteObject(hFont);

	bBuilt = true;
}

void GL::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...)
{
	//Set color and position
	glColor3ub(color[0], color[1], color[2]);
	glRasterPos2f(x, y);

	//Sets where text is going to be stored before printing
	char text[100];	//Max size 100 chars
	va_list args;

	//Format our text
	va_start(args, format);
	vsprintf_s(text, 100, format, args);
	va_end(args);

	//Print text
	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

//Centering text functions
//Centers on x and y axis
vec3 GL::Font::centerText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	vec3 text;
	text.x = x + (width - textWidth) / 2;
	text.y = y + textHeight;
	return text;
}

//Centers only on x axis
float GL::Font::centerText(float x, float width, float textWidth)
{
	if (width > textWidth)
	{
		float difference = width - textWidth;
		return (x + (difference / 2));
	}
	else
	{
		float difference = textWidth - width;
		return (x - (difference / 2));
	}
}