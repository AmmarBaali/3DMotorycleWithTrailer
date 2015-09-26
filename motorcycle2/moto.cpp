/*
 *		This Code Was Created By Yan Wang 2008
 *		A Motorcycle and Trailer
 */

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "Glaux.h"		// Header File For The Glaux Library
#include <math.h>

#define GL_PI	3.1415926f

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLfloat	rtri;				// Angle For The Triangle ( NEW )
GLfloat	rquad;				// Angle For The Quad ( NEW )

GLUquadricObj *quadratic;	// Storage For Our Quadratic Objects ( NEW )

const float piover180 = 0.0174532925f;
float heading;
float xpos;
float zpos;
float zoom = 0;
GLfloat	yrot;				// Y Rotation
GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;
GLfloat lookupdown = 0.0f;
int		wire = 0;
int     perspective = 0;
bool	wp;					// W Pressed? 
bool    pp, op, zp, zout = true;

GLsizei w_window, h_window;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	w_window = width;
	h_window = height;

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	quadratic=gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	return TRUE;										// Initialization Went OK
}

GLvoid glDrawCube(float x, float y, float z)
{
	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f( x, -y, -z);							// Top Right Of The Quad (Bottom)
		glVertex3f(-x, -y, -z);							// Top Left Of The Quad (Bottom)
		glVertex3f(-x, -y, z);							// Bottom Left Of The Quad (Bottom)
		glVertex3f( x, -y, z);							// Bottom Right Of The Quad (Bottom)
		
		glVertex3f( x, y, z);							// Top Right Of The Quad (Front)
		glVertex3f(-x, y, z);							// Top Left Of The Quad (Front)
		glVertex3f(-x, -y, z);							// Bottom Left Of The Quad (Front)
		glVertex3f( x, -y, z);							// Bottom Right Of The Quad (Front)
		
		glVertex3f( x, y, -z);							// Top Right Of The Quad (Back)
		glVertex3f(-x, y, -z);							// Top Left Of The Quad (Back)
		glVertex3f(-x,-y, -z);							// Bottom Left Of The Quad (Back)
		glVertex3f( x,-y, -z);							// Bottom Right Of The Quad (Back)

		glVertex3f(-x, y, z);							// Top Right Of The Quad (Left)
		glVertex3f(-x, y,-z);							// Top Left Of The Quad (Left)
		glVertex3f(-x,-y,-z);							// Bottom Left Of The Quad (Left)
		glVertex3f(-x,-y, z);							// Bottom Right Of The Quad (Left)

		glVertex3f( x, y,-z);							// Top Right Of The Quad (Right)
		glVertex3f( x, y, z);							// Top Left Of The Quad (Right)
		glVertex3f( x,-y, z);							// Bottom Left Of The Quad (Right)
		glVertex3f( x,-y,-z);							// Bottom Right Of The Quad (Right)
	glEnd();											// Done Drawing The Quad
}


GLvoid gldrawFan(float radius, int num)
{
	int iPivot = 1;
	float x, y, z = 0, angle;

	glPushMatrix();
	// Begin a triangle fan
	glBegin(GL_TRIANGLE_FAN);

	// Center of fan is at the origin
	glTranslatef(0.0f, 0.0f, -0.5f);
	glVertex2f(0.0f, 0.0f);
	for(angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI/num))
	{
		// Calculate x and y position of the next vertex
		x = radius*(float)sin(angle);
		y = radius*(float)cos(angle);
	
		// Alternate color between red and green
		if((iPivot %2) == 0)
			glColor3f(1.0f, 1.0f, 1.0f);
		else
			glColor3f(0.0f, 0.0f, 0.0f);
			
		// Increment pivot to change color next time
		iPivot++;

		glVertex2f(x, y);
	}

// Done drawing the fan that covers the bottom
	glEnd();

	glPopMatrix();
}

GLvoid gldrawTorus(float innerRadius, float outerRadius, int numInner, int numOuter)
{
	int		numMajor = numOuter;
	int		numMinor = numInner;
	float   majorRadius = outerRadius;
	float   minorRadius = innerRadius;
	double  majorStep   = 2.0f * GL_PI/numMajor;
	double  minorStep   = 2.0f * GL_PI/numMinor;
	int     i, j;

	for(i=0; i<numMajor; ++i)
	{
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);

		glBegin(GL_TRIANGLE_STRIP);
		for(j=0; j<= numMinor; ++j)
		{
			double  b = j * minorStep;
			GLfloat c = (GLfloat)cos(b);
			GLfloat r = minorRadius * c + majorRadius;
			GLfloat z = minorRadius * (GLfloat)sin(b);

			glNormal3f(x0*c, y0*c, z/minorRadius);
			glVertex3f(x0*r, y0*r, z);

			glNormal3f(x1*c, y1*c, z/minorRadius);
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}

GLvoid glDrawWheel(float inner, float outer) 
{
	int numOuter = 32;
	int numInner = 24;
	float innerRadius = inner;
	float outerRadius = outer;
	
	//gluDisk(quadratic, innerRadius,outerRadius,numInner,numOuter);	// Draw A Disc (CD Shape) With An Inner Radius Of 0.5, And An Outer Radius Of 2.  Plus A Lot Of Segments ;)
	glColor3f(0.2f,0.2f,0.2f);							// Set The Color To Grey
	gldrawTorus(innerRadius, outerRadius, numInner, numOuter);

	glColor3f(0.8f,0.8f,0.8f);							// Set The Color To Grey
	gldrawTorus(innerRadius, outerRadius - innerRadius, numInner, numOuter);

	gldrawFan(outerRadius - 2*innerRadius, 5);
}

GLvoid gldrawCurve(float Radius, int num, float length, float width)
{
	double  Step   = 2.0f * GL_PI/num;
	int     i, j;

	for(i=0; i < num; ++i)
	{
		double a0 = i * Step;
		if(a0 >= GL_PI/length)
			continue;
		double a1 = a0 + Step;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);

		glBegin(GL_TRIANGLE_STRIP);
		for(j=0; j<= num; ++j)
		{
			double  b = j * Step;
			if(b > GL_PI/width && b< (2*width-1)*GL_PI/width)
				continue;
			GLfloat c = (GLfloat)cos(b);
			GLfloat r = (Radius - 0.05f) * c + Radius;
			GLfloat z = (Radius - 0.05f) * (GLfloat)sin(b);

			glNormal3f(x0*c, y0*c, z/(Radius - 0.05f));
			glVertex3f(x0*r, y0*r, z);

			glNormal3f(x1*c, y1*c, z/ (Radius - 0.05f));
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}

GLvoid gldrawCircle(float Radius)
{
	float x, y, angle;
	glBegin(GL_POLYGON);
	for(angle = 0.0; angle<= (2.0*GL_PI); angle +=0.1f)
	{
		x= Radius * (float)sin(angle);
		y= Radius * (float)cos(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	if(op || pp)
	{
		if(perspective == 0){
			glViewport(0,0,w_window,h_window);						// Reset The Current Viewport
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix
			glOrtho(-10.0f, (w_window-10.0f)/80, -5.0f, (h_window-5.0f)/80, -60.0f,60.0f);				// Create Ortho 640x480 View (0,0 At Top Left)
			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glLoadIdentity();									// Reset The Modelview Matrix
		}
		else{
			ReSizeGLScene(w_window, h_window);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

    if(wire == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//draw front wheel
	glLoadIdentity();									// Reset The Current Modelview Matrix
	//glRotatef(rtri,0.0f,1.0f,0.0f);						// Rotate The Triangle On The Y axis ( NEW )

	int numOuter = 32;
	int numInner = 24;
	float innerRadius = 0.15f;
	float outerRadius = 0.8f;

	GLfloat sceneroty = 360.0f - yrot;
	GLfloat xtrans = -xpos;
	GLfloat ztrans = -zpos;
	GLfloat ytrans = -walkbias-0.25f;


	glTranslatef(0.0f, 0.0f, -10.0f);						// Move Right 1.5 Units And Into The Screen 7.0

 
	glTranslatef(xtrans, ytrans, ztrans);
	glRotatef(lookupdown,1.0f,0,0);
	glRotatef(sceneroty,0,1.0f,0);

//	glTranslatef(xtrans, ytrans, ztrans);
	glTranslatef(0.0f, 0.0f, zoom);

	glPushMatrix();
	glTranslatef(-4.5f, 0.0f, 0.0f);						// Move Right 1.5 Units And Into The Screen 7.0
	glColor3f(0.2f,0.2f,0.2f);							// Set The Color To Grey
	gldrawTorus(innerRadius, outerRadius, numInner, numOuter);

	glColor3f(0.8f,0.8f,0.8f);							// Set The Color To Grey
	gldrawTorus(innerRadius / 2, outerRadius - innerRadius, numInner, numOuter);

	gldrawFan(outerRadius - 1.5f*innerRadius, 15);
	glPopMatrix();
	
	//draw shaft
	glPushMatrix();
	glTranslatef(-4.5f, 0.0f, 0.2f);						
	glRotatef(180, 0.0f, 1.0f, 0.0f);					
	glColor3f(0.8f,0.8f,0.8f);							
	gluCylinder(quadratic,0.05f,0.05f,0.4f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4.5f, -0.05f, -0.2f);					
	glRotatef(-30, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.8f,0.8f,0.8f);							
	gluCylinder(quadratic,0.05f,0.05f,1.3f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4.5f, -0.05f, 0.2f);					
	glRotatef(-30, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.8f,0.8f,0.8f);							
	gluCylinder(quadratic,0.05f,0.05f,1.3f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4.5f, 0.0f, 0.0f);					
	glRotatef(18, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.9f, 0.8f, 0.2f);					
	glRotatef(180, 0.0f, 1.0f, 0.0f);				
	glColor3f(0.8f,0.8f,0.8f);						
	gluCylinder(quadratic,0.05f,0.05f,0.4f,32,32);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4.5f, 0.0f, 0.0f);					
	glRotatef(12, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.85f, 0.85f, 0.0f);					
	glRotatef(-30, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.5f,0.5f,0.5f);							
	gluCylinder(quadratic,0.1f,0.1f,1.5f,32,32);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.5f, 0.1f, 0.0f);				
	glColor3f(0.5f,0.5f,0.5f);							
	gldrawCurve(0.9f, 32, 1.5f, 1.0f);
	glPopMatrix();

	//draw handle bars
	glPushMatrix();
	glTranslatef(-4.7f, -0.4f, 0.0f);						
	glRotatef(-25, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.0f, 3.0f, 0.0f);						
	glRotatef(180, 0.0f, 1.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.06f,0.06f,0.6f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4.7f, -0.4f, 0.6f);						
	glRotatef(-25, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.0f, 3.0f, 0.0f);						
	glRotatef(180, 0.0f, 1.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.06f,0.06f,0.6f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.45f, 2.3f, 0.6f);						
	glRotatef(-20, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.06f,0.06f,0.8f,32,32);		
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.45f, 2.3f, -0.6f);						
	glRotatef(-20, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.06f,0.06f,0.8f,32,32);		
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-4.4f, 0.3f, 1.1f);						
	glRotatef(-25, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.0f, 3.0f, 0.0f);						
	glRotatef(180, 0.0f, 1.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.08f,0.08f,0.6f,32,32);		

	gldrawCircle(0.08f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-4.4f, 0.3f, -0.6f);						
	glRotatef(-25, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.0f, 3.0f, 0.0f);						
	glRotatef(180, 0.0f, 1.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.08f,0.08f,0.6f,32,32);		

	gldrawCircle(0.08f);
	glPopMatrix();
	
	//draw cover of wheel
	glPushMatrix();
	glTranslatef(-4.5f, 0.1f, 0.0f);				
	gldrawCurve(0.6f, 32, 1.5, 5.0);
	glPopMatrix();

	//draw light
	glPushMatrix();
	glTranslatef(-4.5f, 0.0f, 0.0f);					
	glRotatef(-5, 0.0f, 0.0f, 1.0f);				
	glTranslatef(0.65f, 0.0f, 0.0f);				
	glRotatef(-90, 0.0f, 1.0f, 0.0f);				
	glTranslatef(0.0f, 2.0f, 0.0f);				
	glColor3f(0.8f,0.8f,0.8f);							
	gluCylinder(quadratic,0.1f,0.3f,0.5f,32,32);

	glColor3f(0.8f,0.8f,0.0f);							
	glTranslatef(0.0f, 0.0f, 0.5f);				
	gldrawCircle(0.3f);
	glPopMatrix();

	//draw windscreen
	glPushMatrix();
	glColor3f(0.6f,0.6f,0.6f);							
	glTranslatef(-3.6f, 2.2f, 0.0f);				
	glRotatef(-15, 0.0f, 0.0f, 1.0f);				
	glRotatef(-90, 0.0f, 1.0f, 0.0f);				

	glBegin(GL_QUADS);
	glVertex3f(0.6f, 0.6f, 0.0f);
	glVertex3f(-0.6f, 0.6f, 0.0f);
	glVertex3f(-0.6f, -0.6f, 0.0f);
	glVertex3f(0.6f, -0.6f, 0.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.5f, 2.6f, 0.0f);				
	glRotatef(-15, 0.0f, 0.0f, 1.0f);				
	glRotatef(-90, 0.0f, 1.0f, 0.0f);				
	gldrawCircle(0.6f);
	glPopMatrix();
	
	
	//draw Rear wheel
	glPushMatrix();
	glTranslatef(-0.5f, 0.0f, 0.0f);				

	glColor3f(0.2f,0.2f,0.2f);						
	gldrawTorus(innerRadius, outerRadius, numInner, numOuter);

	glColor3f(0.8f,0.8f,0.8f);						
	gldrawTorus(innerRadius / 2, outerRadius - innerRadius, numInner, numOuter);

	gldrawFan(outerRadius - 1.5f*innerRadius, 15);
	glPopMatrix();

	//draw exhaust pipe
	glPushMatrix();
	glTranslatef(-2.0f, 0.3f, 0.2f);				
	glRotatef(90, 0.0f, 0.0f, 1.0f);					
	glRotatef(90, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.4f,0.4f,0.4f);						
	gluCylinder(quadratic,0.15f,0.15f,2.0f,32,32);		
	glPopMatrix();

	//draw seat
	glPushMatrix();
	glTranslatef(-1.5f, 1.0f, 0.0f);					
	glRotatef(180,1.0f,0.0f,0.0f);						
	glColor3f(0.4f,0.4f,0.4f);						
	glDrawCube(2.0f, 0.4f, 0.4f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.4f, 1.2f, 0.0f);					
	glRotatef(180,1.0f,0.0f,0.0f);						
	glColor3f(0.6f,0.6f,0.6f);						
	glDrawCube(0.6f, 0.6f, 0.7f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f, 1.0f, 0.0f);				
	glRotatef(90, 0.0f, 0.0f, 1.0f);					
	glRotatef(90, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);						
	gluCylinder(quadratic,0.05f,0.05f,0.5f,32,32);		
	glPopMatrix();

	//draw gas tank
	glPushMatrix();
	glTranslatef(-3.0f, 0.8f, -0.6f);				
	glColor3f(0.2f,0.2f,0.2f);						
	gluCylinder(quadratic,0.5f,0.5f,1.3f,32,32);		
	
	glColor3f(0.7f,0.7f,0.7f);						
	gldrawCircle(0.5f);
	glTranslatef(0.0f, 0.0f, 1.45f);				
	gldrawCircle(0.5f);

	glTranslatef(-0.2f, 0.2f, 0.0f);				
	glColor3f(0.5f,0.5f,0.5f);						
	gluCylinder(quadratic,0.1f,0.1f,0.05f,32,32);		
	gldrawCircle(0.1f);
	glPopMatrix();

	//draw trailer
	//draw cargo of the trailer
	glPushMatrix();
	glTranslatef(3.0f, 0.5f, 0.0f);
	glColor3f(0.5f,0.5f,0.5f);							
	glDrawCube(1.5f, 0.6f, 1.0f);

	//draw wheels of the trailer
	glTranslatef(0.0f, -0.8f, 1.0f);						
	glDrawWheel(0.1f, 0.6f);

	glTranslatef(0.0f, 0.0f, -2.0f);					
	glDrawWheel(0.1f, 0.6f);
	glPopMatrix();

	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	gluDeleteQuadric(quadratic);						// Delete The Quadratic To Free System Resources

	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","OpenGL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			// LoWord Can Be WA_INACTIVE, WA_ACTIVE, WA_CLICKACTIVE,
			// The High-Order Word Specifies The Minimized State Of The Window Being Activated Or Deactivated.
			// A NonZero Value Indicates The Window Is Minimized.
			if ((LOWORD(wParam) != WA_INACTIVE) && !((BOOL)HIWORD(wParam)))
				active=TRUE;						// Program Is Active
			else
				active=FALSE;						// Program Is No Longer Active

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("MotorCycle",800,600,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("MotorCycle",800,600,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
			if (keys[VK_UP])
			{
				lookupdown+= 1.0f;
				//glTranslatef(xtrans, ytrans, 0.0f);
			}

			if (keys[VK_DOWN])
			{
				lookupdown-= 1.0f;
			}
			if (keys[VK_RIGHT])
			{
				heading += 1.0f;
				yrot = heading;
			}

			if (keys[VK_LEFT])
			{
				heading -= 1.0f;	
				yrot = heading;
			}
			if (keys['F'])
			{

				xpos -= (float)sin(heading*piover180) * 0.05f;
				zpos -= (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle >= 359.0f)
				{
					walkbiasangle = 0.0f;
				}
				else
				{
					walkbiasangle+= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if (keys['B'])
			{
				xpos += (float)sin(heading*piover180) * 0.05f;
				zpos += (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle <= 1.0f)
				{
					walkbiasangle = 359.0f;
				}
				else
				{
					walkbiasangle-= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}
/*
			if (keys['Z'] && !zp)
			{
				zp = true;
				if(zout)
				{
					zoom += 5.0f;
					zout = false;
				}
				else
				{
					zoom -= 5.0f;
					zout = true;
				}
			}
			if(!keys['Z'])
				zp = false;
*/
			if (keys[VK_NEXT])
			{

				xpos -= (float)sin(heading*piover180) * 0.05f;
				zpos -= (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle >= 359.0f)
				{
					walkbiasangle = 0.0f;
				}
				else
				{
					walkbiasangle+= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if (keys[VK_PRIOR])
			{
				xpos += (float)sin(heading*piover180) * 0.05f;
				zpos += (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle <= 1.0f)
				{
					walkbiasangle = 359.0f;
				}
				else
				{
					walkbiasangle-= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if(keys['C'])
			{
				heading = 0;
				xpos = 0;
				zpos = 0;
				zoom = 0;
				yrot = 0;				// Y Rotation
				walkbias = 0;
				walkbiasangle = 0;
				lookupdown = 0.0f;
				wire = 0;
				perspective = 1;
				wp = false;
				pp = false;
				op = false;
				zp = false;
				zout = true;
				DrawGLScene();
			}
			if(keys['W'] && !wp)
			{
				wp = true;
				if(wire == 0)
					wire = 1;
				else
					wire = 0;
			}
			if(!keys['W'])
			{
				wp = false;
			}
			if(keys['P'] && !pp)
			{
				pp = true;
				perspective = 1;
			}
			if(!keys['P'])
				pp = false;
			if(keys['O'] && !op)
			{
				op = true;
				perspective = 0;
			}
			if(!keys['O'])
				op = false;
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
