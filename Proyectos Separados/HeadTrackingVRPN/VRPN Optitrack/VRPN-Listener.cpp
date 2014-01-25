//== 
//== VRPN Sample Application ==--
//==
//== This application listens for a rigid body named 'Tracker' on the local machine
//== and displays it's position and orientation.
//==
//== You'll also need to download the VRPN distribution files.
//==

//== Includes ==--
#include "conio.h"           // for kbhit()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "vrpn_Connection.h" // Missing this file?  Get the latest VRPN distro at
#include "vrpn_Tracker.h"    //    ftp://ftp.cs.unc.edu/pub/packages/GRIP/vrpn
#include <windows.h>

// What VRPN-Listener
// These are all reported in three-element double arrays
// in the order (X=0, Y=1, Z=2).
// They are translated into this format from the native format for each device.
// Orientations from all trackers in VRPN are reported in quaternions
// (see Quatlib for more info) in four-element double arrays
// in the order (X=0, Y=1, Z=2, W=3).

#define PI 3.141592
#define NUM_WALLS 10
#define WALLS_SIZE 5
#define TOROIDE 0
#define CUBO 1
#define ESFERA 2
#define TEAPOT 3

typedef struct NodoStruct* Nodo;

struct NodoStruct {
	void* contenido;
	struct NodoStruct* next;
}NodoStruct;

/* data structures: is a list */
typedef struct ListaStruct* Lista;

struct ListaStruct {
	Nodo inicio;
	Nodo fin;
}ListaStruct;

/* data structures: represents a figure */
struct FiguraStruct {
	int name;
	int spin;
	int orbit;
	float x;
	float y;
	float z;
	float a_x;
	float a_y;
	float a_z;
	int tipo;
	int textura;
}FiguraStruct;

typedef struct FiguraStruct* Figura;

//**Initial values
//window dimensions 
int ancho, alto;
//camera rotation angles
float anglex = 9.45, angley = -11.40;
//camer point vector
float lx = 0.0f, lz = -1.0f;
//camera position
float x = 0.0f, z = 52.0f;
//shape indicator
int shape = 0;
//left clic indicator
int left_clic = 0;
//right clic indicator
int right_clic = 0;
//middle clic indicator
int mid_clic = 0;
//mouse position
int curx, cury;

int angulo = 325;
//wall array
float walls[NUM_WALLS][9]; // X,Y,Z,QX,QY,QZ,QW,sizeWalls_X,sizeWalls_Y
double disparidad=0;
GLfloat floorVertices[4][3] = {
	{ -15.0, 0.0, 15.0 },
	{ 15.0, 0.0, 15.0 },
	{ 15.0, 0.0, -15.0 },
	{ -15.0, 0.0, -15.0 },
};
//Material colors
GLfloat blueMaterial[] = { 0.0, 0.2, 1.0, 1.0 };
GLfloat	redMaterial[] = { 0.6, 0.1, 0.0, 1.0 };
GLfloat	purpleMaterial[] = { 0.3, 0.0, 0.3, 1.0 };
GLfloat	greenMaterial[] = { 1.0, 0.2, 0.0, 1.0 };

GLuint texture[6];
Lista Figuras;


#define QUAD_OBJ_INIT() { if(!quadObj) initQuadObj(); }
static GLUquadricObj *quadObj;


float rotationInSameAxis;
float rotationTranslation;
int floorAvailable=1;
int conditionFloor=0;
int conditionRotation=0;
int condicionPlanet=0;
int condicionFog=0;
int condicionLight =0;
int condicionSpotLight =0;
int condicionShadow =0;
int condicionObjectOrShadow=0;

//Variables for FOG
GLfloat angle = 0.0;
GLfloat density = 0.05;
GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};

float n[] = { 0.0,  -1.0, 0.0 }; // Normal vector for the plane
float e[] = { 0.0, -4.2, 0.0 }; // Point of the plane


float xPared = 0.0f, yPared = 0.0f, zPared = 0.0f;
float qx = .0f,qy = .0f,qz = .0f, qw = .0f, angleQ = .0f, scale =.0f;
int savedWalls;
int sizeWalls_X=10;
int sizeWalls_Y=5;
//== Functions ==--
float dot_product(const float* a, const float* b);
void cross_product(float* dst, const float* a, const float* b);
void normalize(float* v);
void VectorSet(float *v, float x, float y, float z);
void subtract(float* dst, const float* a, const float* b);
void projection(const float *pa,const float *pb,const float *pc,const float *pe, float n, float f);
void calibrar();
Figura crearFigura(int tipo);
Figura moverFiguraXYZ(Figura f, float x, float y, float z);
Nodo crearNodo(void* content);
void agregarNodo(Lista l, Nodo n);
Lista crearLista();
void doList(Lista l, void(*hacer)(void*));
void dibujarFigura(Figura f);
static void drawFloor(void);
static void drawBox(GLfloat xsize, GLfloat ysize, GLfloat zsize);
static void drawPillar(void);
static void customDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings);
void customSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
	GLint nsides, GLint rings);
void customSolidCube(GLdouble size);
void customSolidSphere(GLdouble radius, GLint slices, GLint stacks);
static void initQuadObj(void);
static void customDrawBox(GLfloat size, GLenum type);
Figura setTextura(Figura f,int i);
static void display(void);
static void redraw();
void mouseW(int button, int state, int x, int y);
//== global variables ==--
float *Pa,*Pb,*Pc,*PeI,*PeD,*offset0,*offIzq,*offDer;
//== Callback prototype ==--

void VRPN_CALLBACK handle_pos (void *, const vrpn_TRACKERCB t);

//== Main entry point ==--

//== Position/Orientation Callback ==--

void VRPN_CALLBACK handle_pos (void *, const vrpn_TRACKERCB t)
{

    //printf("Pared Position:(%.4f,%.4f,%.4f) Orientation:(%.2f,%.2f,%.2f,%.2f)\n",t.pos[0], t.pos[1], t.pos[2],t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
	xPared = t.pos[0]*100.0f;//ahora esta en cm
	yPared = t.pos[1]*100.0f;//ahora esta en cm
	zPared = t.pos[2]*100.0f;//ahora esta en cm

	VectorSet(PeI,xPared,yPared,zPared);
	subtract(PeI,PeI,offset0);
	subtract(PeI,PeI,offIzq);

	VectorSet(PeD,xPared,yPared,zPared);
	subtract(PeD,PeI,offset0);
	subtract(PeD,PeI,offDer);

	xPared = t.pos[0]*10.0f;//ahora esta en cm
	yPared = t.pos[1]*10.0f;//ahora esta en cm
	zPared = t.pos[2]*10.0f;//ahora esta en cm

	
	angleQ = 2.0 * acos(t.quat[3]) * (180.0f / PI);
	qx   =   t.quat[0];
    qy   =   t.quat[1];
    qz   =   t.quat[2];
	//printf("Pared Position:(%.4f,%.4f,%.4f)\n",qx,qy,qz);
	//printf("Pared angle:(%.4f)\n",t.quat[3]* (180.0f / PI));
}
// FUnctions that take the BMP file and load the Texture
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

}

//**FUNCTION THAT GENERATES THE SCENE
void renderScene(void) {
   
   //Clear the color an the depth in the buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   //Reset the transformations
   glLoadIdentity();
   
   //Camera
   gluLookAt(x, 1.0f, z, 
             x+lx, 1.0f, z+lz,
             0.0f, 1.0f, 0.0f);
   glRotatef(angley, 0.0f, 1.0f, 0.0f);
   glRotatef(anglex, 1.0f, 0.0f, 0.0f);
   //printf("x:%.4f  z:%.4f anglex:%.4f angley:%.4f",x,z,anglex,angley);
   //Shape
   if(shape==0){
       glShadeModel(GL_FLAT);
   }else if(shape==1){
	   glShadeModel(GL_SMOOTH);
   }else if(shape==2){
	   glShadeModel(GL_PHONG_WIN);
   }

   glPushMatrix();
		glColor3f(255.0f, 0.0f, 0.0f);
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 2 );
		glBegin(GL_LINES);
			glVertex3f(-1000, 0, 0);
			glVertex3f(1000, 0, 0);
		glEnd();
    glPopMatrix();
	glPushMatrix();
		glColor3f(1.0f, 255.0f, 0.0f);
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 2 );
		glBegin(GL_LINES);
			glVertex3f(0, -1000, 0);
			glVertex3f(0, 1000, 0);
		glEnd();
    glPopMatrix();
	glPushMatrix();
		glColor3f(1.0f, 0.0f, 255.0f);
		glEnable( GL_LINE_SMOOTH );
		glLineWidth( 2 );
		glBegin(GL_LINES);
			glVertex3f(0, 0, -1000);
			glVertex3f(0, 0, 1000);
		glEnd();
    glPopMatrix();

   // Dibujo Piso
   glEnable(GL_COLOR_MATERIAL);
				glColor3f(1.0f,1.0f,1.0f);
				glPushMatrix();
					GLUquadricObj * newCuadric = gluNewQuadric();
					gluQuadricDrawStyle(newCuadric, GLU_FILL);
					glEnable(GL_TEXTURE_2D);
			    
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					gluQuadricDrawStyle(newCuadric, GLU_FILL );
					gluQuadricNormals(newCuadric, GLU_SMOOTH );
					gluQuadricTexture(newCuadric,GL_TRUE);
					glRotatef(0.0f,0.0f,0.0f,0.0f);
					glTranslatef(0.0f,0.0f,0.0f);
					glBegin( GL_QUADS );
						glVertex3f(-20,0,-20);
						glVertex3f(20,0,-20);
						glVertex3f(20,0,20);
						glVertex3f(-20,0,20);
					glEnd();
				glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	// Dibujo Esfera Guia 
   glPushMatrix();
	    glTranslatef(xPared, yPared, zPared);
		glutSolidSphere(0.5f, 10,10);
	glPopMatrix();
	// Dibujo Ojo izquierdo 
	glPushMatrix();
	    glTranslatef(xPared, yPared, zPared);
		glutSolidSphere(0.5f, 10,10);
	glPopMatrix();
	//Pared
    glPushMatrix();
		glColor3f(255.0f,1.0f,1.0f);
	    glTranslatef(xPared, yPared, zPared);
		glRotatef(angleQ, qx, qy, qz);
		glBegin(GL_QUADS);
			glVertex3f(-1*sizeWalls_X,0,-1*sizeWalls_Y);
			glVertex3f(1*sizeWalls_X,0,-1*sizeWalls_Y);
			glVertex3f(1*sizeWalls_X,0,1*sizeWalls_Y);
			glVertex3f(-1*sizeWalls_X,0,1*sizeWalls_Y);
		glEnd();
	glPopMatrix();
	
	int o;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(o=0;o<savedWalls;o++){
		/*
		walls[savedWalls][0] = xPared;
		walls[savedWalls][1] = yPared;
		walls[savedWalls][2] = zPared;
		walls[savedWalls][3] = qx;
		walls[savedWalls][4] = qy;
		walls[savedWalls][5] = qz;
		walls[savedWalls][6] = angleQ;*/
		glPushMatrix();
		glColor4f(1.0f,1.0f,1.0f,0.5f);
	    glTranslatef(walls[o][0], walls[o][1], walls[o][2]);
		glRotatef(walls[o][6], walls[o][3], walls[o][4], walls[o][5]);
		glBegin(GL_QUADS);
			glVertex3f(-1*walls[o][7],0,-1*walls[o][8]);
			glVertex3f(1*walls[o][7],0,-1*walls[o][8]);
			glVertex3f(1*walls[o][7],0,1*walls[o][8]);
			glVertex3f(-1*walls[o][7],0,1*walls[o][8]);
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_BLEND);

	glutSwapBuffers();

}
void projection(const float *pa,const float *pb,const float *pc,const float *pe, float n, float f)
{
	float va[3], vb[3], vc[3];
	float vr[3], vu[3], vn[3];
	float l, r, b, t, d, M[16];
	// Compute an orthonormal basis for the screen.
	subtract(vr, pb, pa);
	subtract(vu, pc, pa);
	normalize(vr);
	normalize(vu);
	cross_product(vn, vr, vu);
	normalize(vn);
	// Compute the screen corner vectors.
	subtract(va, pa, pe);
	subtract(vb, pb, pe);
	subtract(vc, pc, pe);
	// Find the distance from the eye to screen plane.
	d = -dot_product(va, vn);
	// Find the extent of the perpendicular projection.
	l = dot_product(vr, va) * n / d;
	r = dot_product(vr, vb) * n / d;
	b = dot_product(vu, va) * n / d;
	t = dot_product(vu, vc) * n / d;
	// Load the perpendicular projection.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(l, r, b, t, n, f);
	// Rotate the projection to be non-perpendicular.
	memset(M, 0, 16 * sizeof (float));
	M[0] = vr[0]; M[4] = vr[1]; M[8] = vr[2];
	M[1] = vu[0]; M[5] = vu[1]; M[9] = vu[2];
	M[2] = vn[0]; M[6] = vn[1]; M[10] = vn[2];
	M[15] = 1.0f;
	glMultMatrixf(M);
	// Move the apex of the frustum to the origin.
	glTranslatef(-pe[0], -pe[1], -pe[2]);
	glMatrixMode(GL_MODELVIEW);

}
void subtract(float* dst, const float* a, const float* b){
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
}
void VectorSet(float *v, float x, float y, float z){
	v[0] = x;
	v[1] = y;
	v[2] = z;
}
void normalize(float* v){
	v[0] /= sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	v[1] /= sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	v[2] /= sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
}
void cross_product(float* dst, const float* a, const float* b){
	dst[0] = (a[1] * b[2]) - (a[2] * b[1]);
	dst[1] = -((a[0] * b[2]) - (a[2] * b[0]));
	dst[2] = (a[0] * b[1]) - (a[1] * b[0]);
}
float dot_product(const float* a, const float* b){
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}
//**FUNCTION FOR CHANGES IN THE WINDOW SIZE
void changeSize(int w, int h)
{
   //In order not to have zero division
   if(h==0){
       h=1;
   }
   float ratio =  w * 1.0 / h;
   
   //Use of Matrix projection
   glMatrixMode(GL_PROJECTION);
   
   //Reset Matrix
   glLoadIdentity();
   
   //Set the window
   glViewport(0, 0, w, h);
   
   //Perspective
   gluPerspective(45.0f, ratio, 0.1f, 100.0f);
   
   //Return to modelview
   glMatrixMode(GL_MODELVIEW);
   
}


//**FUNCTION FOR THE EVENTS WITH KEYBOARD
void processSpecialKeys(int key, int xx, int yy) {
   
   int mod;
   if(key==GLUT_KEY_LEFT){
        angley=angley+5;
   }
   if(key==GLUT_KEY_RIGHT){
        angley=angley-5;
   }
   if(key==GLUT_KEY_UP){
        mod = glutGetModifiers();
        if(mod == GLUT_ACTIVE_SHIFT){
            z--;
        }else{
            anglex=anglex+5;
        }
   }
   if(key==GLUT_KEY_DOWN){
        mod = glutGetModifiers();
        if(mod == GLUT_ACTIVE_SHIFT){
            z++;
        }else{
            anglex=anglex-5;
        }
   }
}


//Function of the keys that are pressed
void processNormalKeys(unsigned char key, int x, int y){
	int figura=-1;
	switch (key) {
			
			// We establish if the User want to see the Floor or Not
		case 'f':
			//walls[NUM_WALLS][7]; // X,Y,Z,QX,QY,QZ,QW
			if(savedWalls < NUM_WALLS){
				printf("printed\n");
				walls[savedWalls][0] = xPared;
				walls[savedWalls][1] = yPared;
				walls[savedWalls][2] = zPared;
				walls[savedWalls][3] = qx;
				walls[savedWalls][4] = qy;
				walls[savedWalls][5] = qz;
				walls[savedWalls][6] = angleQ;
				walls[savedWalls][7] = sizeWalls_X;
				walls[savedWalls][8] = sizeWalls_Y;
				printf("saved Wall %d\n \t\t%.4f \t%.4f \t%.4f\n\t\t%.4f \t%.4f \t%.4f\n", savedWalls, walls[savedWalls][0], walls[savedWalls][1], walls[savedWalls][2], 0,0,0);//walls[savedWalls][3], walls[savedWalls][4], walls[savedWalls][5]);
				savedWalls++;
			}
			break;
            //Change the Floor Texture
		case 'g':
			if(conditionFloor==1)
				conditionFloor=0;
			else
				conditionFloor++;
			break;

		case 'o':
			sizeWalls_X++;
			break;

		case 'l':
			sizeWalls_Y++;
			break;
	    case 'i': // x--
			if (sizeWalls_X>0)
				sizeWalls_X--;
			break;
		case 'k': // y--
			if (sizeWalls_Y>0)
				sizeWalls_Y--;
			break;

		case 'p':
				system("PAUSE");	
			break;
			
	
	}
}

void motion(int x, int y){
	GLfloat dx = (y - cury) * 360.0 / ancho;
	GLfloat dy = (x - curx) * 360.0 / alto;
	if(left_clic == 1){
        angley += dy;
        anglex += dx;
	}
	if(right_clic ==1){
		if(dx>0)
			z ++;
		if(dx<0)
			z --;
	}
	if(mid_clic ==1){
		if(dx>0){
			angulo++;
		}
		if(dx<0){
			angulo--;
		}
		if (angulo>=135&&angulo<=225){
		}else if(angulo>225)
			angulo=225;
		else if(angulo<135)
			angulo=135;
	}
    curx = x;
    cury = y;
	
  glutPostRedisplay();
}
void mouseW(int button, int state, int x, int y){
	
	if (state > 0)
    {
        disparidad+=0.3;
    }
    else
    {
        disparidad-=0.3;
    }
    return;
}
void mouse(int button, int state, int x, int y){
	/*
		With the right click, camera is moved, with left click we apply zoom
	*/
	if(state == GLUT_DOWN){
		switch(button){
		case GLUT_LEFT_BUTTON:
			//seleccionar(x,y);
			left_clic = 1;
			motion(curx = x, cury = y);
			break;
		case GLUT_RIGHT_BUTTON:
			right_clic =1;
			break;
		case GLUT_MIDDLE_BUTTON:
			mid_clic = 1;
			break;
		}
	}else if(state == GLUT_UP){
		switch(button){
		case GLUT_LEFT_BUTTON:
			left_clic = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			right_clic = 0;
			break;
		
		case GLUT_MIDDLE_BUTTON:
			mid_clic = 0;
			break;
		}
	}

}
static void redraw(){
	GLfloat lightColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glPushMatrix();

	//glColor3f(0.72f, 0.45f, 0.2f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	drawFloor();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	drawPillar();
	


	doList(Figuras, ((void(*)(void*))dibujarFigura));

	glDisable(GL_TEXTURE_2D);


	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT0);
	}
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT1);
	}
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT2);
	}
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT3);
	}
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT4);
		glLightfv(GL_LIGHT4, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT4);
	}
	if (1){
		glPushMatrix();
		glEnable(GL_LIGHT5);
		glLightfv(GL_LIGHT5, GL_DIFFUSE, lightColor);
		glPopMatrix();
	}
	else{
		glDisable(GL_LIGHT5);
	}
	glFlush();
	glutSwapBuffers();
	
}
static void display(void) {
	
	//double posx = -13.0, posy = 15.0, posz =-15.0, lookx = 80, looky = -20, lookz = -20, upx = 0, upy = 1, upz = 0;
	double posx = 5, posy = 2, posz = 10, lookx = 0, looky = 0, lookz = 0, upx = 0, upy = 1, upz = 0;
	glDrawBuffer(GL_BACK_LEFT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	
	projection(Pa, Pb, Pc, PeI, 0.4f, 1000.0);//el ultimo antes era 200
	glLoadIdentity();
	redraw();



	glTranslatef(disparidad, 0.0f, 0.0f);

	gluLookAt(posx - disparidad/2, posy, posz, lookx, looky, lookz, upx, upy, upz);
	glDrawBuffer(GL_BACK_RIGHT);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
   

	 projection(Pa, Pb, Pc, PeD, 0.4f, 1000.0);//el ultimo antes era 200
	glLoadIdentity();

	redraw();
	
	glTranslatef(-disparidad, 0.0f, 0.0f);
	gluLookAt(posx + disparidad/2, posy, posz, lookx, looky, lookz, upx, upy, upz);

	



	//gluLookAt(zoom, zoom, zoom, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	


	glShadeModel(GL_SMOOTH);
	
	
}
static void drawBox(GLfloat xsize, GLfloat ysize, GLfloat zsize) {
	static GLfloat n[6][3] = {
		{ -1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.0, -1.0 }
	};
	static GLint faces[6][4] = {
		{ 0, 1, 2, 3 },
		{ 3, 2, 6, 7 },
		{ 7, 6, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 5, 6, 2, 1 },
		{ 7, 4, 0, 3 }
	};
	GLfloat v[8][3];
	GLint i;
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -xsize / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = xsize / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -ysize / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = ysize / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -zsize / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = zsize / 2;
	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0.0, 0.0); glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0.0, 1.0); glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1.0, 1.0); glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1.0, 0.0); glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}
static void drawPillar(void) {
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenMaterial);
	glPushMatrix();
	glTranslatef(8.0, 10.0, 8.0);
	drawBox(1.0, 20.0, 1.0);
	glPopMatrix();
	glDisable(GL_NORMALIZE);
}
static void initQuadObj(void)
{
	quadObj = gluNewQuadric();
	if (!quadObj)
		printf("out of memory.");
}
DWORD WINAPI hacerTracking(LPVOID lpParam)
{
	 
		vrpn_Connection *connection;

		char connectionName[128];
		int  port = 3883;

		sprintf(connectionName,"localhost:%d", port);
        
		connection = vrpn_get_connection_by_name(connectionName);
    
		vrpn_Tracker_Remote *tracker = new vrpn_Tracker_Remote("Ojos", connection);

  		tracker->register_change_handler(NULL, handle_pos);	
		while(!kbhit())
		{
			tracker->mainloop();
			connection->mainloop();
			Sleep(5);
		}
     return 0;
}
static void drawFloor(void) {
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3fv(floorVertices[0]);
	glTexCoord2f(0.0, 1.0);
	glVertex3fv(floorVertices[1]);
	glTexCoord2f(1.0, 1.0);
	glVertex3fv(floorVertices[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3fv(floorVertices[3]);
	glEnd();
}
void dibujarFigura(Figura f){
	const GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat mat_diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat high_shininess[] = { 10.0f };
	//Asignamos estos valores a los componentes delmaterial
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glPushMatrix();
	glTranslatef(f->x, f->y, f->z);
	 
		 glBindTexture(GL_TEXTURE_2D, texture[f->textura]);
		 glColor3f(1.0f, 1.0f, 1.0f);
	 switch (f->tipo){
	 case TOROIDE:
		 customSolidTorus(0.5, 1, 10, 14);
		 break;
	 case CUBO:
		 customSolidCube(3.0f);
		 break;
	 case ESFERA:
		 customSolidSphere(1.5f, 8, 8);
		 break;
	 case TEAPOT:
		 glutSolidTeapot(1.5);
		 break;
	 }
	glPopMatrix();

}
void customSolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
	QUAD_OBJ_INIT();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluQuadricTexture(quadObj, GL_TRUE);
	gluSphere(quadObj, radius, slices, stacks);

}
void customSolidCube(GLdouble size)
{
	customDrawBox(size, GL_QUADS);
}
void customSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
	GLint nsides, GLint rings)
{
	customDoughnut(innerRadius, outerRadius, nsides, rings);
}

static void customDrawBox(GLfloat size, GLenum type)
{
	static GLfloat n[6][3] =
	{
		{ -1.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, 0.0, -1.0 }
	};
	static GLint faces[6][4] =
	{
		{ 0, 1, 2, 3 },
		{ 3, 2, 6, 7 },
		{ 7, 6, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 5, 6, 2, 1 },
		{ 7, 4, 0, 3 }
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);

		glTexCoord2f(0.0, 0.0); glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(0.0, 1.0); glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1.0, 1.0); glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(1.0, 0.0); glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}
static void customDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
	int i, j, n;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;

	ringDelta = 2.0 * PI / rings;
	sideDelta = 2.0 * PI / nsides;
	n = 0;
	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	for (i = rings - 1; i >= 0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		glBegin(GL_QUAD_STRIP);
		phi = 0.0;
		for (j = nsides; j >= 0; j--) {
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = R + r * cosPhi;
			glTexCoord2f(n, 0.0); glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
			glTexCoord2f(n, 1.0); glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist, r * sinPhi);
			n = !n;
		}
		glEnd();
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
	}
}
void doList(Lista l, void(*hacer)(void*)){
	Nodo ultimo = NULL;
	for (ultimo = l->inicio; ultimo != NULL; ultimo = ultimo->next){
		hacer((ultimo->contenido));
	}
}
Lista crearLista() {
	Lista l = (Lista) (malloc(sizeof(ListaStruct)));
	l->inicio = NULL;
	l->fin = NULL;
	return l;
}
void agregarNodo(Lista l, Nodo n) {
	Nodo ultimo = l->inicio;
	while (true){
		if (ultimo == NULL){
			l->inicio = n;
			l->fin = n;
			return;
		}
		if (ultimo->next == NULL){
			ultimo->next = n;
			l->fin = n;
			return;
		}
		else{
			ultimo = ultimo->next;
		}
	}
}
Nodo crearNodo(void* content) {
	Nodo n = (Nodo) (malloc(sizeof(NodoStruct)));
	n->contenido = content;
	n->next = NULL;
	return n;
}
Figura moverFiguraXYZ(Figura f, float x, float y, float z){
	f->x += x;
	f->y += y;
	f->z += z;
	return f;
}
Figura crearFigura(int tipo){
	Figura f = (Figura) (malloc(sizeof(FiguraStruct)));
	f->x = 0.00f;
	f->y = 0.00f;
	f->z = 0.00f;
	f->a_x = 0.00f;
	f->a_y = 0.00f;
	f->a_z = 0.00f;
	f->spin = 0;
	f->orbit = 0;
	f->tipo = tipo;
	f->textura = -1;
	return f;
}
void calibrar(){
	Pa= (float*) malloc(sizeof(float*) * 3);
	Pb= (float*) malloc(sizeof(float*) * 3);
	Pc= (float*) malloc(sizeof(float*) * 3);
	PeI= (float*) malloc(sizeof(float*) * 3);
	PeD= (float*) malloc(sizeof(float*) * 3);
	offset0=(float*)malloc(sizeof(float*)*3);
	offIzq=(float*)malloc(sizeof(float*)*3);
	offDer=(float*)malloc(sizeof(float*)*3);

	VectorSet(offIzq,-3.5,-3.15,0);
	VectorSet(offDer,3.5,-3.15,0);

	VectorSet(offset0,1,117.45,-252);
	VectorSet(Pa,-62.2,-36.55,0);
	VectorSet(Pb,62.2,-36.55,0);
	VectorSet(Pc,-62.2,36.55,0);
}
Figura setTextura(Figura f,int i){
	f->textura = i;
	return f;
}
//**PRINCIPAL FUNCTION
int main(int argc, char **argv){
	int i;
	DWORD dwThreadId, dwThrdParam = 1;
	savedWalls=0;
	HANDLE hiloParaTracking;
	

	
   // GLut Initialization
   glutInit(&argc,argv);
   calibrar();
   	Figuras = crearLista();
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA),2), 5.0f, 1.5f, 8.0f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 4), -10.0f, 4.5f, 5.0f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 5), -10.0f, 1.5f, -6.0f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 0), 1.0f, 2.5f, 10.0f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA), 3), -12.3f, 1.5f, -5.3f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 3), -10.0f, 7.5f, 2.2f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 4), 3.0f, 1.5f, 4.13f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 5), 3.1f, 2.5f, -11.1f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA), 0), -1.22f, 1.5f, -1.34f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 2), 7.0f, 1.5f, 12.2f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 3), 10.0f, 1.5f, -1.13f)));
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 2), 3.1f, 3.5f, -1.1f)));


   //Window Position
   glutInitWindowPosition(100, 100);
   //Shows the window: GLUT_SINGLE = single buffer de color; GLUT:RGB = modelo RGB de color
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA| GLUT_STEREO );
   
   //Window Size
   glutInitWindowSize(ancho = 800, alto = 600);
   
   
   //Title for the window
   glutCreateWindow("Proyecto-RealidadVirtual");    

   //Textures Loading
   initRendering();
   //Function that generates the objects
  //glutDisplayFunc(renderScene);
  glutDisplayFunc(display);

   //Changes in the size of the window by the user
   glutReshapeFunc(changeSize);

   glutIdleFunc(display);
    //glutIdleFunc(renderScene);
   
   //Function for the user entries
   glutSpecialFunc(processSpecialKeys);
   glutKeyboardFunc(processNormalKeys);
   glutMouseWheelFunc(mouseW);
    //Functions with the mouse
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

   //OpenGLinit
   glEnable(GL_DEPTH_TEST);
   //Window always visible
		hiloParaTracking = CreateThread(
		NULL, // default security attributes
		0, // use default stack size
		hacerTracking, // thread function
		&dwThrdParam, // argument to thread function
		0, // use default creation flags
		&dwThreadId); // returns the thread identifier
		if (hiloParaTracking == NULL)
			printf("CreateThread() failed, error: %d.\n", GetLastError());
		//else, gives some prompt...
		else
		{
			printf("It seems the CreateThread() is OK lol!\n");
			printf("The thread ID: %u.\n", dwThreadId);
		}
		if (CloseHandle(hiloParaTracking) != 0)
			printf("Handle to thread closed successfully.\n");

		glutMainLoop();
	
	return 0;
}
