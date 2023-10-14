#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <windows.h>
#include<math.h>
#include<bits/stdc++.h>
#include "BmpLoader.h"
#include <fstream>
#include <iostream>
#define GL_CLAMP_TO_EDGE 0x812F
using namespace std;


const int window_width = 1200;
const int window_height = 700;
GLfloat eyeX=0;
GLfloat eyeY=40;
GLfloat eyeZ=50;

GLfloat lookX=0;
GLfloat lookY=40;
GLfloat lookZ=-200;

bool light_switch_0=false;
bool light_switch_1=false;
bool spot_light_switch=false;
bool main_light_switch=false;

float carx=-300, carz=0;
float busx=0, busz=-300;
bool bus_switch=true;
bool car_switch=true;

float rot = 0;

unsigned int ID;

const double PI = 3.14159265389;


/* GLUT callback Handlers */


int anglex= 0, angley = 0, anglez = 0;          //rotation angles
int window;
int wired=0;
int shcpt=1;
int animat = 0;
const int L=20;
const int dgre=3;
int ncpt=L+1;
int clikd=0;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;


GLfloat ctrlpoints[L+1][3] =
{
{7.625,5.4,0},
{7.325,4.775,0},
{7.225,4.05,0},
{6.975,3.4,0},
{6.675,2.85,0},
{6.275,2.15,0},
{5.85,1.725,0},
{4.85,1.15,0},
{3.975,0.9,0},
{3.325,0.85,0},
{2.9,0.375,0},
{2.775,-0.175,0},
{2.725,-0.7,0},
{3.225,-0.925,0},
{3.875,-0.9,0},
{5.175,-1.375,0},
{5.825,-1.975,0},
{6.5,-2.725,0},
{6.925,-3.925,0},
{7.325,-5.05,0},
{7.6,0.175,0},
};




float wcsClkDn[3],wcsClkUp[3];
///////////////////////////////
class point1
{
public:
    point1()
    {
        x=0;
        y=0;
    }
    int x;
    int y;
} clkpt[2];
int curve_flag=0;
GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info

//////////////////////////
void scsToWcs(float sx,float sy, float wcsv[3] );
void processMouse(int button, int state, int x, int y);
void matColor(float kdr, float kdg, float kdb,  float shiny, int frnt_Back=0, float ambFactor=1.0, float specFactor=1.0);
///////////////////////////

void scsToWcs(float sx,float sy, float wcsv[3] )
{

    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    //glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

    winX = sx;
    winY = (float)viewport[3] - (float)sy;
    winZ = 0;

    //get the world coordinates from the screen coordinates
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    wcsv[0]=worldX;
    wcsv[1]=worldY;
    wcsv[2]=worldZ;


}
void processMouse(int button, int state, int x, int y)
{
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        if(curve_flag!=1)
        {
            curve_flag=1;
            clkpt[0].x=x;
            clkpt[0].y=y;
        }


        scsToWcs(clkpt[0].x,clkpt[0].y,wcsClkDn);
        //cout<<"\nD: "<<x<<" "<<y<<" wcs: "<<wcsClkDn[0]<<" "<<wcsClkDn[1];
        cout<<"{"<<wcsClkDn[0]<<","<<wcsClkDn[1]<<",0},"<<endl;
    }
    else if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
    {
        if (curve_flag==1)
        {
            clkpt[1].x=x;
            clkpt[1].y=y;
            curve_flag=0;
        }
        float wcs[3];
        scsToWcs(clkpt[1].x,clkpt[1].y,wcsClkUp);
        //cout<<"\nU: "<<x<<" "<<y<<" wcs: "<<wcsClkUp[0]<<" "<<wcsClkUp[1];

        clikd=!clikd;
    }
}

//control points
long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve ( double t,  float xy[2])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);

    //return y;
}


static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{


    {3,1,5,7},
    {2,0,1,3},
    {7,5,4,6},
    {2,3,7,6},
    {1,0,5,4},
    {6,4,0,2},


};
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void material_property(float R, float G, float B)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { R, G, B, 1.0 };
    GLfloat mat_diffuse[] = { R, G, B, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {5.0};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

}




void cube(float R=0.5, float G=0.5, float B=0.5 )
{
    material_property(R,G,B);
    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {

        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);


        glTexCoord2f(1,1);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(1,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(0,1);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);

    }
    glEnd();
}



void LoadTexture(const char*filename, int rep = 1)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(rep)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void buiding()
{
    // building r1
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);

    glPushMatrix();
    glTranslatef(-80,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);


    //roof r1
    glPushMatrix();
    glTranslatef(-80,60,0);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // building 2
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);

    glPushMatrix();
    glTranslatef(50,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //roof 2
    glPushMatrix();
    glTranslatef(50,60,0);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building r2
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,4);

    glPushMatrix();
    glTranslatef(-80,0,-30);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof r2
    glPushMatrix();
    glTranslatef(-80,30,-30);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // building 5
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,5);

    glPushMatrix();
    glTranslatef(50,30,-30);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


    //roof 5
    glPushMatrix();
    glTranslatef(50,60,-30);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 6
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, 6);

glPushMatrix();
glTranslatef(100, 30, -50); // Move 20 units behind (change -30 to -50)
glScalef(20, 60, 20);
glTranslatef(-0.5, -0.5, -0.5);
cube();
glPopMatrix();
glDisable(GL_TEXTURE_2D);

// roof 6
glPushMatrix();
glTranslatef(100, 60, -50); // Move 20 units behind (change -30 to -50)
glScalef(20, 1, 20);
glTranslatef(-0.5, -0.5, -0.5);
cube();
glPopMatrix();


    // building 7
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);

    glPushMatrix();
    glTranslatef(0,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 7
    glPushMatrix();
    glTranslatef(0,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 8
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);

    glPushMatrix();
    glTranslatef(50,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 8
    glPushMatrix();
    glTranslatef(50,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // Building r3
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, 4);

// Move the building to the left
glPushMatrix();
glTranslatef(-80, 30, -60); // Updated X-coordinate to move to the left
glScalef(20, 60, 20);
glTranslatef(-0.5, -0.5, -0.5);
cube();
glPopMatrix();
glDisable(GL_TEXTURE_2D);

// Roof r3
// Move the roof to the left
glPushMatrix();
glTranslatef(-80, 60, -60); // Updated X-coordinate to move to the left
glScalef(20, 1, 20);
glTranslatef(-0.5, -0.5, -0.5);
cube();
glPopMatrix();




}
void playground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,7);
    glPushMatrix();
    glTranslatef(95,2,-10);
    glScalef(50,1,40);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}
//2nd field
void ground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,22);
    glPushMatrix();
    glTranslatef(-120,2,-10);
    glScalef(50,1,40);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}


void road()
{


    // left side road
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,10);

    glPushMatrix();
    glTranslatef(-50,0.8,0);
    glRotatef(90,0,1,0);
    glScalef(300,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //right side road
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,22);

    glPushMatrix();
    glTranslatef(135,0.8,0);
    glRotatef(90,0,1,0);
    glScalef(1000, 3, 48);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void sun_moon()
{
   // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set light position
    GLfloat light_position[] = { 0.0, 1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Set material properties for the moon
    GLfloat moon_ambient[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_specular[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_shininess = 100.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, moon_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, moon_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, moon_shininess);

    // Render the moon
    glPushMatrix();
    glTranslatef(0, 150, 0);
    glScalef(3, 3, 3);
    glutSolidSphere(2, 16, 16);
    glPopMatrix();

    // Disable lighting
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}
void main_light()
{
    //GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 150.0, 0, 1.0 };

    glEnable( GL_LIGHT7);

    glLightfv( GL_LIGHT7, GL_AMBIENT, light_ambient);
    glLightfv( GL_LIGHT7, GL_DIFFUSE, light_diffuse);
    glLightfv( GL_LIGHT7, GL_SPECULAR, light_specular);

    glEnable( GL_LIGHT7);
    if (main_light_switch)
    {
        glLightfv( GL_LIGHT7, GL_AMBIENT, light_ambient);
        glLightfv( GL_LIGHT7, GL_DIFFUSE, light_diffuse);
        glLightfv( GL_LIGHT7, GL_SPECULAR, light_specular);

    }



    glLightfv( GL_LIGHT7, GL_POSITION, light_position);

}


void axes()
{
    float length = 10;
    float width = 0.3;

    // X-axis
    glPushMatrix();
    glTranslatef(length/2,0,0);
    glScalef(length,width,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();

    // Y-axis
    glPushMatrix();
    glTranslatef(0,length/2,0);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,1,0);
    glPopMatrix();

    // Z-axis
    glPushMatrix();
    glTranslatef(0,0,length/2);
    glScalef(width,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,0,1);
    glPopMatrix();
}

void bus_front()
{

    //base
    glPushMatrix();
    glTranslatef(0,10,0);
    glScalef(20,20,2);
    cube();
    glPopMatrix();

    // glass
    glPushMatrix();
    glTranslatef(2,18,2);
    glScalef(15,10,1);
    cube(0.0,0.0,1.0);
    glPopMatrix();

    // Lights 1
    glPushMatrix();
    glTranslatef(5,15,2);
    glutSolidTorus(0.5,1,16,16);
    glPopMatrix();

    // Lights 2
    glPushMatrix();
    glTranslatef(15,15,2);
    glutSolidTorus(0.5,1,16,16);
    glPopMatrix();

}
void bus_left_side()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,26);
    glPushMatrix();
    glTranslatef(-1,10,-40);
    glScalef(1,15,40);
    cube(0.0,0.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

void bus_right_side()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,26);
    glPushMatrix();
    glTranslatef(20,10,-40);
    glScalef(1,15,40);
    cube(0.0,0.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}
void bus_back_side()
{
    //base
    glPushMatrix();
    glTranslatef(0,10,-41);
    glScalef(20,20,1);
    cube(0.0,0.0,1.0);
    glPopMatrix();

}
void bus_wheels()
{
    // Save the current color
    GLfloat prevColor[4];
    glGetFloatv(GL_CURRENT_COLOR, prevColor);

    // Set wheel color to black
    glColor3f(1.0, 0.41, 0.71);

    // wheel 1
    glPushMatrix();
    glTranslatef(2, 5, -10);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(2, 4, 16, 16);
    glPopMatrix();

    // wheel 2
    glPushMatrix();
    glTranslatef(20, 5, -10);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(2, 4, 16, 16);
    glPopMatrix();

    // wheel 3
    glPushMatrix();
    glTranslatef(2, 5, -35);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(2, 4, 16, 16);
    glPopMatrix();

    // wheel 4
    glPushMatrix();
    glTranslatef(20, 5, -35);
    glRotatef(90, 0, 1, 0);
    glutSolidTorus(2, 4, 16, 16);
    glPopMatrix();

    // Restore the previous color
    glColor4fv(prevColor);
}

void bus()
{
    glPushMatrix();
    // Body of Bus
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,16);
    glPushMatrix();
    glTranslatef(0,10,-40);
    glScalef(20,20,40);
    cube(0.0,0.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //FRONT

    bus_front();

    // Bus left side

    bus_left_side();


    // Bus right side

    bus_right_side();


    // bus back side

    bus_back_side();


    // bus wheels

    bus_wheels();


    glPopMatrix();


}
void bus_animation()
{
    if(busz<=300)
    {

        busz +=0.08;
    }
    if(busz>=300)
    {
        busz = -300;

    }

    glutPostRedisplay();

}
void bus_move()
{
    for(int i=1; i<=150; i+=70)
    {
        glPushMatrix();
        glTranslatef(0,0,busz);
        bus();
        if (bus_switch)
        {

            bus_animation();
        }

        glPopMatrix();
    }


}

void bus_control()
{


        //bus_move();
        glPushMatrix();
        glTranslatef(-55,0,0);
        glScalef(0.5,0.5,0.5);
        bus_move();
        glPopMatrix();



}



void base_floor()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,20);

    glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(600,1,500);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.2,0.2,0.2);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}



void tableBezier()
{
    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*PI / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                getNormal3p(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i

}

static void idle(void)
{
    glutPostRedisplay();
}


void showControlPoints()
{
    glPointSize(5.0);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i <=L; i++)
        glVertex3fv(&ctrlpoints[i][0]);
    glEnd();
}
void chair()
{

    float length=20;
    float width=1;

    //base seat
    glPushMatrix();
    glTranslatef(0,length/2,0);
    glScalef(length,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // leg base 1
    glPushMatrix();
    glTranslatef(length/2 -width/2,0,length/2-width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();
    // leg base 2
    glPushMatrix();
    glTranslatef(length/2 -width/2,0,- length/2 +width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();
    // leg base 3
    glPushMatrix();
    glTranslatef(-length/2 +width/2,0,+ length/2 -width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();
    // leg  base 4
    glPushMatrix();
    glTranslatef(-length/2 +width/2,0,- length/2 +width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();

    // upper  1
    glPushMatrix();
    glTranslatef(length/2 -width/2,length,length/2-width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,1,0);
    glPopMatrix();
    // upper 2
    glPushMatrix();
    glTranslatef(-length/2 -width/2,length,length/2+width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,1,0);
    glPopMatrix();

    // upper close 1
    glPushMatrix();
    glTranslatef(0,length,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,0,1);
    glPopMatrix();
    // upper close 2
    glPushMatrix();
    glTranslatef(0,length+5,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,1);
    glPopMatrix();

    // upper close 3
    glPushMatrix();
    glTranslatef(0,length-5,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,1);
    glPopMatrix();


}


void chair_table()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set light position
    GLfloat light_position[] = { 0.0, 1.0, 0.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Set material properties for the moon
    GLfloat moon_ambient[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_specular[] = { 1.0, 1.0, 0.0, 1.0 };
    GLfloat moon_shininess = 100.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, moon_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, moon_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, moon_shininess);

    // CURVED CHAIR TABLE PART
    glPushMatrix();
    material_property(1,0,0);
    glTranslatef(-220, -25, 0); // Add this translation to move the entire chair and table to the left
    glRotatef( 90, 0.0, 0.0, 1.0);
    //glRotatef( 180, 1.0, 0.0, 1.0);
    glScalef(3,3,3);
    tableBezier();
    glPopMatrix();


    //3rd chair
    glPushMatrix();
    glTranslatef(-22 -224,-5,0); // Add -X to move the chair to the left
    glRotatef(-90,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();
    //4th chair
    glPushMatrix();
    glTranslatef(22 -218,-5,0); // Add -X to move the chair to the left
    glRotatef(90,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();
}

void curved_animation()
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 5000.0;
    const double a = t*90.0;
    if(wired)
    {
        glPolygonMode( GL_FRONT, GL_LINE ) ;
        glPolygonMode( GL_BACK, GL_LINE ) ;

    }
    else
    {
        glPolygonMode( GL_FRONT,GL_FILL ) ;
        glPolygonMode( GL_BACK, GL_FILL ) ;
    }

    glPushMatrix();

    if(animat)
        glRotated(a,0,0,1);

    glRotatef( anglex, 1.0, 0.0, 0.0);
    glRotatef( angley, 0.0, 1.0, 0.0);         	//rotate about y-axis
    glRotatef( anglez, 0.0, 0.0, 1.0);

    glRotatef( 90, 0.0, 0.0, 1.0);
    glTranslated(-3.5,0,0);
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    //matColor(0,0,0,20);   // front face color
    //matColor(0.0,0,0,20,1);  // back face color
    material_property(0,1,0);


    if(shcpt)
    {
        //matColor(0.0,0.0,0.9,20);
        material_property(0,1,0);

        //showControlPoints();
    }

    glPopMatrix();

}
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'u': // up


        eyeY++;
        lookY++;

        break;
    case 'd': // down

        eyeY--;
        lookY--;
        break;
    case 'a': // look right
        lookX++;

        break;
    case 'b':
        lookX--;
        // look left
        break;

    case 'p': // rotate left
        rot--;
        break;
    case 'q': // rotate right
        rot++;
        break;
    case 'l': // left

        eyeX--;
        lookX--;


        break;
    case 'r': // right

        eyeX++;
        lookX++;

        break;
    case '+': // zoom in
        eyeZ--;
        break;

    case '-':
        //zoom out
        eyeZ++;
        break;
    case 's':
    case 'S':
        shcpt=!shcpt;
        break;

    case 'w':
    case 'W':
        wired=!wired;
        break;

    case 'x':
        anglex = ( anglex + 3 ) % 360;
        break;
    case 'X':
        anglex = ( anglex - 3 ) % 360;
        break;

    case 'y':
        angley = ( angley + 3 ) % 360;
        break;
    case 'Y':
        angley = ( angley - 3 ) % 360;
        break;

    case 'z':
        anglez = ( anglez + 3 ) % 360;
        break;
    case 'Z':
        anglez = ( anglez - 3 ) % 360;
        break;

    case '1':
        light_switch_0 =! light_switch_0;
        break;

    case '2':
        light_switch_1 =! light_switch_1;
        break;

    case '3':
        spot_light_switch =! spot_light_switch;
        break;
    case '4':
        main_light_switch=!main_light_switch;
        break;
    case '8':
        car_switch=!car_switch;
        break;
    case '9':
        bus_switch=!bus_switch;
        break;
    }

    glutPostRedisplay();
}


void display_settings()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int frustum_window = 8;
    glFrustum(-frustum_window, frustum_window, -frustum_window, frustum_window, 4, 300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;

    gluLookAt(eyeX,eyeY,eyeZ, lookX,lookY,lookZ, 0,1,0);


    glRotatef(rot, 0,1,0);

}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.529, 0.808, 0.922, 1.0); // Set clear color to light blue (sky color)


    display_settings();
    base_floor();
    sun_moon();
    main_light();
    //axes();
    buiding();
    road();
    playground();
    ground();


    // Wall Watch with base tower
    glPushMatrix();
    glTranslatef(300,0,0);
    glScalef(0.5,0.5,0.5);

    glPushMatrix();
    glTranslatef(5,75,0);

    //wall_watch();
    glPopMatrix();
    // watch function end

    glPopMatrix();

    // more floor
    glPushMatrix();
    glTranslatef(200,0,0);
    base_floor();
    glPopMatrix();
    // Hotel 1
    glPushMatrix();
    glTranslatef(400,10,-20);

    glPopMatrix();


    // Sign board 1
    glPushMatrix();
    glTranslatef(-25,-10,-20);

    glPopMatrix();

    // Sign board 2
    glPushMatrix();
    glTranslatef(200,-10,-20);

    glPopMatrix();



     bus_control();

    glPushMatrix();
    glTranslatef(-205,45,-40);
    glScalef(5,5,5);
    glTranslatef(-0.5,-0.5,-0.5);
    //cube(1.0,0,0);
    glPopMatrix();
    glPushMatrix();

    glPopMatrix();

    glPushMatrix();
    glTranslatef(-100,0,-40);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(-450,0,-20);

    glPopMatrix();




    // Spot light for shop
    glPushMatrix();

    glTranslatef(5,0,0);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10,-20,10);

    glPopMatrix();

    // Traffic Signal 2
    glPushMatrix();
    glTranslatef(140,-20,10);

    glPopMatrix();

    for (int i=10; i<=100; i=i+20)
    {
        glPushMatrix();

        glTranslatef(40,-20,-i);

        glPopMatrix();
    }

    for (int i=10; i<=100; i=i+20)
    {
        glPushMatrix();

        glTranslatef(-150,-20,-i);

        glPopMatrix();
    }


    // chair table
    glPushMatrix();
    glTranslatef(200,10,-10);
    glScalef(0.5,0.5,0.5);
    chair_table();
    glPopMatrix();

    // Curved Setting for animation
    curved_animation();



    glFlush();
    glutSwapBuffers();
}
void texture_function()
{
      printf("f");
    // Building texture
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building6.bmp",1);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building4.bmp",2);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building5.bmp",3);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building4.bmp",4);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building5.bmp",5);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\building5.bmp",6);

    // Playground grass
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\leaf4.bmp",7);

    //  Traffic Light
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\traffic2.bmp",8);

    // Signboard
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\signboard7.bmp",9);

    // roads
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\road.bmp",10);

    // shop
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\shop1.bmp",11);

    // Trees
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\leaf2.bmp",12);

    // wood 1
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\wood3.bmp",13);
    //LoadTexture("C:\\Users\\Shimul\\Documents\\CSE 4208 Computer Graphics\\City 3D\\images\\wood2.bmp",15);
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\water2.bmp",14);

    // road light bulb texture
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\bulb4.bmp",15);
    // Cars
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\car1.bmp",16);

    // park lead
    LoadTexture("F:\\ComputerG\\3d\\3d\\leaf6.bmp",17);

    // shop 3
    LoadTexture("F:\\ComputerG\\3d\\3d\\shop4.bmp",18);
    // Chocolate color
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\chocolate.bmp",19);
    // base floor
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\car1.bmp",20);
    // sun moon
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\sun1.bmp",21);
    //swimming pool water
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\water3.bmp",22);
    // watch
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\watch1.bmp",23);
    // Walls
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\floor3.bmp",24);
    // Football playground
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\football1.bmp",25);
    // BUS Texture
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\bus10.bmp",26);

    // Hotel Bricks
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\brick1.bmp",27);
    //stone
    LoadTexture("F:\\ComputerG\\3d\\3d\\stone1.bmp",28);
    // KUET BUS
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\kuet3.bmp",29);

     // hotel Door
    LoadTexture("F:\\ComputerG\\3d\\3d\\images\\door3.bmp",30);

}
int main(int argc, char *argv[])
{

    glutInit(&argc, argv);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(300,10);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("City 19204004 19204005 19204040");

    texture_function();


    cout<<"1. Buildings \t\t 2. Roads \t\t "<<endl;
    cout<<" \t\t 8. Playgrounds \t\t"<<endl;
    cout<<" \t\t 12. Sun"<<endl;
    cout<<"\t\t 13. Bus \t\t 14. Hotel \t\t "<<endl;
    cout<<"-------------------------------------------------------------------------------------------"<<endl;



    cout<<"--------------------------------------------Instruction------------------------------------"<<endl;
    cout<<"\t Press : u -> Move Up"<<endl;
    cout<<"\t Press : d -> Move Down"<<endl;
    cout<<"\t Press : l -> Move Left"<<endl;
    cout<<"\t Press : r -> Move Right"<<endl;

    cout<<"\t Press : p -> Rotate Right"<<endl;
    cout<<"\t Press : p -> Rotate Right"<<endl;

    cout<<"\t Press : a -> Look Right"<<endl;
    cout<<"\t Press : b -> Look Left"<<endl;

    cout<<"\t Press : + -> Zoom In"<<endl;
    cout<<"\t Press : - -> Zoom Out"<<endl;

    cout<<"---------------------------------------------------------------"<<endl;

    cout<<"\t Press : w -> Curve Wired Line ON/OFF"<<endl;

    cout<<"\t Press : 1 -> ON/OFF Light 1"<<endl;
    cout<<"\t Press : 2 -> ON/OFF Light 2"<<endl;
    cout<<"\t Press : 3 -> ON/OFF Spot Light"<<endl;
    cout<<"\t Press : 4 -> ON/OFF Sun/Moon Light"<<endl;

    cout<<"\t Press : 8 -> START/STOP Car Moving"<<endl;
    cout<<"\t Press : 9 -> START/STOP Bus Moving"<<endl;




    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(processMouse);
    //glutIdleFunc(idle);




    glShadeModel( GL_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glutMainLoop();

}
