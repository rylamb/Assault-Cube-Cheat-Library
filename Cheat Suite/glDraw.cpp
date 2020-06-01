#include "pch.h"
#include "glDraw.h"


/* Citation
https://guidedhacking.com/threads/how-to-get-started-with-opengl-hacks.11475/
*/

//Setting up our othographic projection matrix
//Allows us to do 2D stuff in our 3D game
//Call this before we draw
void GL::SetupOrtho()
{
	glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glPushMatrix();		//Push stuff to get into diff drawing mode - pop this off the stack later
	GLint viewport[4];	//4 ints representing x & y (position of view port) and width & height of the window
	glGetIntegerv(GL_VIEWPORT, viewport);	//Get the current values of the window and store in viewport
	glViewport(0, 0, viewport[2], viewport[3]);	//Set our own viewport to overlay the first viewport (Top left of window is 0, 0; 2 = width & 3 = height)
	glMatrixMode(GL_PROJECTION); //Setting this matrix mode allows us to draw
	glLoadIdentity();	//Blank view matrix
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1); //Creates orthographic projection matrix (for 2D drawing)
	glMatrixMode(GL_MODELVIEW); //Allows us to draw models (boxes, text, etc.)
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);	//Only draws stuff to the top layer - should never be hidden behind anything on the screen
}

//Reverses the settings we've set in SetupOrtho
void GL::RestoreGL()
{
	glPopMatrix();
	glPopAttrib();
}

// 
void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);	//Determines the color of the rectangle

	//glBenin/End used for drawing in immediate mode
	glBegin(GL_QUADS);	//What OpenGL uses for drawing rectangles

	// the 4 corners of the rectangle
	// want to start in top left and draw clockwise
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);

	glEnd();
}

void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);

	//same as before; the 0.5f allows for drawing over top of something made by DrawFilledRect w/o gaps
	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();
}