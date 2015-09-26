#include <GL/glut.h>
#include <math.h>

GLUquadricObj *quadratic;
static int isWire = 0; // Is wireframe?
static int distance = 10;
static float angleH = 0;
static float angleV = 0;

static float R = 2.0; // Radius of hemisphere.
static int p = 4; // Number of longitudinal slices.
static int q = 6; // Number of latitudinal slices.

#define PI 3.14159265358979324

static unsigned int pipe, seat, cover, wheel, wheelCenter, cycles; // parts of the motorcycle to make as display lists.
GLUquadricObj *cylinder;

void drawCoordinates();
void drawMotorcycle();
void drawTrailer();
void drawHemisphere();
void drawCylinder(float x, float y, float z);
void drawHandle(float x, float y, float z);
void drawLight();
void drawBase();
void setup();

void display () {

    /* clear window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(distance*cos(angleH), distance*cos(angleV), distance*sin(angleH), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    /* future matrix manipulations should affect the modelview matrix */
    glMatrixMode(GL_MODELVIEW);

    if (isWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPushMatrix();

        drawCoordinates();

        glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);   // Move the motorcycle around the world space
            drawMotorcycle();
            drawTrailer();
        glPopMatrix();

    glPopMatrix();

    /* flush drawing routines to the window */
    glFlush();
}

void drawCoordinates()
{
    /***************** DRAW AXIS *****************/
    glPushMatrix();
        GLUquadricObj *xAxis; 
        xAxis=gluNewQuadric();
        glColor3f(1,0,0);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(xAxis,0.05,0.05,1,5,5); 
    glPopMatrix();

    glPushMatrix();
        GLUquadricObj *yAxis; 
        yAxis=gluNewQuadric();
        glColor3f(0,1,0);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(yAxis,0.05,0.05,1,5,5); 
    glPopMatrix();

    glPushMatrix();
        GLUquadricObj *zAxis; 
        zAxis=gluNewQuadric();
        glColor3f(0,0,1);
        gluCylinder(zAxis,0.05,0.05,1,5,5); 
    glPopMatrix();
    /***************** END OF DRAW AXIS *****************/
}

void drawMotorcycle()
{
    //DRAW ENGINE
    glPushMatrix();
        //drawCoordinates();
        glColor3f(.6, 0, 0);
        glScalef(1.4, 0.8, 1.0);
        glutSolidSphere(1,8,8);
        glPushMatrix();
            //drawCoordinates();
        glPopMatrix();
    glPopMatrix();

    //DRAW PIPES UNDER ENGINE
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glRotatef(80, 0, 1, 0);
        glTranslatef(0.5, 1.0, -1.5);
        glCallList(pipe);
        glTranslatef(0.0, -2.0, 0.0);
        glCallList(pipe);
    glPopMatrix();

    //DRAW SEAT
    glPushMatrix();
        glPushMatrix();
            glRotatef(15, 0, 0, 1);
            glTranslatef(-2.0, -0.4, 0.0);
            glScalef(2.0, 0.2, 1.2);
            glCallList(seat);
        glPopMatrix();

        //DRAW BACK SEAT
        glRotatef(-40, 0, 0, 1);
        glTranslatef(-2.3, -2.8, 0.0);
        glScalef(2.0, 0.2, 1.2);
        glCallList(seat);
    glPopMatrix();

    //DRAW FRONT PLATE
    glPushMatrix();
        glRotatef(120, 0, 0, 1);
        glTranslatef(0.8, -1.3, 0.0);
        glScalef(2.0, 0.2, 1.35);
        glColor3f(0.5,0.0,0.0);
        glutSolidCube(1);
        glPushMatrix();
            //drawCoordinates();
        glPopMatrix();
    glPopMatrix();

    //DRAW FRONT PIPES
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glRotatef(-30, 0, 1, 0);
        glTranslatef(1.3, -0.9, -5.7);
        glScalef(1.0, 1.0, 2.5);
        glCallList(pipe);
        glTranslatef(0.0, 1.7, 0.0);
        glCallList(pipe);
    glPopMatrix();

    //DRAW WHEEL COVERS
    glPushMatrix();
        glTranslatef(3.5, -3.0, 0.0);
        glScalef(1.0, 0.5, 1.0);
        glRotatef(45, 0, 0, 1);
        glCallList(cover);
        glTranslatef(-5.5, 0.0, 0.0);
        glRotatef(-100, 0, 0, 1);
        glTranslatef(-8.5, 0.2, 0.0);
        glCallList(cover);
    glPopMatrix();

    //DRAW WHEELS
    glPushMatrix();
        glTranslatef(3.9, -4.1, 0.0);
        glCallList(wheel);
        glTranslatef(-9.2, 2.0, 0.0);
        glCallList(wheel);
    glPopMatrix();

    //DRAW WHEEL CENTER PIECES
    glPushMatrix();
        glTranslatef(3.9, -4.1, 0.0);
        glCallList(wheelCenter);
        glTranslatef(-9.2, 2.0, 0.0);
        glCallList(wheelCenter);
    glPopMatrix();

    //DRAW CYCLES AROUND WHEELS
    glPushMatrix();
        glTranslatef(3.9, -4.1, 0.0);
        glRotatef(-90, 1, 0, 0);

        for(int i=0; i<8; i++)
        {
            glRotatef(45, 0, 1, 0);
            glPushMatrix();
                glCallList(cycles);
            glPopMatrix();
        }

        glTranslatef(-9.2, 0.0, 2.0);

        for(int i=0; i<8; i++)
        {
            glRotatef(45, 0, 1, 0);
            glPushMatrix();
                glCallList(cycles);
            glPopMatrix();
        }
    glPopMatrix();

    //DRAW HANDLE BARS
    glPushMatrix();
        glTranslatef(0.2, 2.0, 0.0);
        glRotatef(-45, 1, 0, 0);
        glScalef(0.7, 0.7, 0.7);
        glCallList(pipe);
        glRotatef(-90, 1, 0, 0);
        glCallList(pipe);
    glPopMatrix();

    //DRAW LIGHT
    glPushMatrix();
        glTranslatef(1.0, 1.0, 0.0);
        glColor3f(0.5, 0.5, 0.0);
        //glScalef(1.0, 0.5, 1.0);
        glutSolidSphere(0.5, 5, 5);
        glPushMatrix();
            //drawCoordinates();
        glPopMatrix();
    glPopMatrix();

    //DRAW BASE
    glPushMatrix();
        glRotatef(10.0, 0.0, 0.0, 1.0);
        glScalef(3.5, 1.5, 1.0);
        glTranslatef(-0.4, -1.0, 0.0);
        glColor3f(0.3, 0.3, 0.3);
        glutSolidCube(1);
    glPopMatrix();

    //GAS TANK
    glPushMatrix();
        glScalef(2.5, 1.0, 0.8);
        glTranslatef(-0.8, -1.7, -1.4);
        glCallList(pipe);
    glPopMatrix();
}

void drawTrailer()
{
    //DRAW BODY
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glScalef(2.0, 2.5, 1.5);
        glTranslatef(-4.5, -0.5, 0.0);
        glutSolidCube(1);
    glPopMatrix();

    //DRAW WHEELS
    glPushMatrix();
        glPushMatrix();
            glScalef(0.8, 0.8, 0.8);
            glTranslatef(-12.0, -1.5, 2.0);
            glCallList(wheel);
            glCallList(wheelCenter);

            glRotatef(90, 1, 0, 0);
            for(int i=0; i<8; i++)
            {
                glRotatef(45, 0, 1, 0);
                glPushMatrix();
                    glCallList(cycles);
                glPopMatrix();
            }
        glPopMatrix();

        glPushMatrix();
            glScalef(0.8, 0.8, 0.8);
            glTranslatef(-12.0, -1.5, -2.0);
            glCallList(wheel);
            glCallList(wheelCenter);

            glRotatef(90, 1, 0, 0);
            for(int i=0; i<8; i++)
            {
                glRotatef(45, 0, 1, 0);
                glPushMatrix();
                    glCallList(cycles);
                glPopMatrix();
            }
        glPopMatrix();
    glPopMatrix();

    //DRAW CONNECTION TO MOTORCYCLE
    glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glTranslatef(0.0, -1.0, -8.0);
        glCallList(pipe);
    glPopMatrix();
}

void drawHemisphere()
{
    for(int j = 0; j < q; j++)
    {
        // One latitudinal triangle strip.
        glBegin(GL_TRIANGLE_STRIP);
        for(int i = 0; i <= p; i++)
        {
            glVertex3f( R * cos( (float)(j+1)/q * PI/2.0 ) * cos( 2.0 * (float)i/p * PI ),
            R * sin( (float)(j+1)/q * PI/2.0 ),
            R * cos( (float)(j+1)/q * PI/2.0 ) * sin( 2.0 * (float)i/p * PI ) );
            glVertex3f( R * cos( (float)j/q * PI/2.0 ) * cos( 2.0 * (float)i/p * PI ),
            R * sin( (float)j/q * PI/2.0 ),
            R * cos( (float)j/q * PI/2.0 ) * sin( 2.0 * (float)i/p * PI ) );         
        }
        glEnd();
    }
}

void reshape (int w, int h)
{

    // (Window of width = zero is not possible).
    if(h == 0)
        h = 1;

    float ratio = 1.0* w / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(90,ratio,-1,1);
}


// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key) 
   {
    case 'c' : distance = 10; angleH=0; angleV=0.0; break;
    case 'C' : distance = 10; angleH=0; angleV=0.0; break;
    case 'f': distance = (distance == 4)? 4:distance-1; break;
    case 'F': distance = (distance == 4)? 4:distance-1; break;
    case 'b': distance = (distance == 20)? 20:distance+1; break;
    case 'B': distance = (distance == 20)? 20:distance+1; break;
    case 'w': if (isWire == 0) isWire = 1; else isWire = 0; break;
    case 'W': if (isWire == 0) isWire = 1; else isWire = 0; break;
    //case 27: exit(0); break;
    default: break;
   }
}

void specialKeyboard(int key, int x, int y) {
   switch (key)
   {
       case GLUT_KEY_RIGHT:
      angleH -= .2;
      break;
   case GLUT_KEY_LEFT:
      angleH += .2;
      break;
   case GLUT_KEY_UP:
       angleV += .2;
      break;

   case GLUT_KEY_DOWN:
       angleV -= .2;
      break;
   }

}

void update(void){
   glutPostRedisplay();
}

void setup()
{
    // PARTS
    pipe = glGenLists(1);
    seat = glGenLists(1);
    cover = glGenLists(1);
    wheel = glGenLists(1);
    wheelCenter = glGenLists(1);
    cycles = glGenLists(1);

    glNewList(pipe, GL_COMPILE);   // Any cylinder on the motorcycle
        GLUquadricObj *cylinder; 
        cylinder=gluNewQuadric();
        glPushMatrix();
            glColor3f(.5,.5,.5);
            gluCylinder(cylinder,0.2,0.2,3,5,5);
                glPushMatrix();
                    //drawCoordinates();
                glPopMatrix();
        glPopMatrix();
    glEndList();

    glNewList(seat, GL_COMPILE);
        glPushMatrix();
            glColor3f(0.5, 0.35, 0.05);
            glutSolidCube(1);
                glPushMatrix();
                    //drawCoordinates();
                glPopMatrix();
        glPopMatrix();
    glEndList();

    glNewList(cover, GL_COMPILE);
        glPushMatrix();
            glColor3f(0.5, 0.0, 0.0);
            drawHemisphere();
                glPushMatrix();
                    //drawCoordinates();
                glPopMatrix();
        glPopMatrix();
    glEndList();

    glNewList(wheel, GL_COMPILE);
        glPushMatrix();
            glColor3f(0.1, 0.1, 0.1);
            glutSolidTorus(0.2, 1.2, 20, 20);
                glPushMatrix();
                    //drawCoordinates();
                glPopMatrix();
        glPopMatrix();
    glEndList();

    glNewList(wheelCenter, GL_COMPILE);
        glPushMatrix();
            glColor3f(0.4, 0.5, 0.5);
            glScalef(1.0, 0.5, 1.0);
            glutSolidSphere(0.8, 5, 5);
                glPushMatrix();
                    //drawCoordinates();
                glPopMatrix();
        glPopMatrix();
    glEndList();

    glNewList(cycles, GL_COMPILE);
        glColor3f(0.5, 0.5, 0.5);
        glScalef(0.25, 0.25, 0.25);
        cylinder=gluNewQuadric();
        gluCylinder(cylinder,0.5,0.5,5,15,5);
    glEndList();
}


int main (int argc, char** argv) {

    /* initialize GLUT, using any commandline parameters passed to the 
       program */
    glutInit(&argc,argv);

    /* setup the size, position, and display mode for new windows */
    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH );

    /* create and set up a window */
    glutCreateWindow("Motorcycle");
    setup(); // Build all the display lists, ready to use

    glutIdleFunc(update);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyboard);

    /* set up depth-buffering */
    glEnable(GL_DEPTH_TEST);

    /* background color */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* tell GLUT to wait for events */
    glutMainLoop();
    return 0;
}