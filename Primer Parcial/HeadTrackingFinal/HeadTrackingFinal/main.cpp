#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <GL/gl.h>
#include "glut.h"
#include "include\cameralibrary.h"
using namespace CameraLibrary;

/* data structures: is a node of a list */

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

/* data structures: represents an arc*/
struct ArcoStruct {
	void* inicio;
	void* fin;
	float peso;
}ArcoStruct;

typedef struct ArcoStruct* Arco;

/* data structures: represents a shell*/
struct CascaraStruct {
	Lista Figuras;
	float x, y, z;
	float r;
	int name;
}CascaraStruct;

typedef struct CascaraStruct* Cascara;

/* data structures: represents a cluster */
struct ClusterStruct {
	Lista ArcosFiguras;
	Lista niveles;
}ClusterStruct;

typedef struct ClusterStruct* Cluster;

/* data structures: represents an image*/
struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};





/* definitions*/
typedef struct Image Image;

typedef struct FiguraStruct* Figura;

typedef struct FiguraStruct* Figura;

#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64

typedef enum { LIGHT_CENTER_LEFT, LIGHT_CENTER_RIGHT, LIGHT_NEAR, LIGHT_FAR, LIGHT_LEFT, LIGHT_RIGHT } light_position;

enum { M_TEXTURE, M_TOOGLE_SELF_SPIN, M_TEXTURE_01, M_TEXTURE_02, M_TEXTURE_03, M_TEXTURE_04, M_TEXTURE_05, M_ORBIT };

enum { X, Y, Z, W };

enum { A, B, C, D };



/*Global variables*/
int texturaPiso;
int nombreActual;
int selectionMode;
int specialMode;
int actualTexture;
Lista Figuras;
GLuint *buffer;
GLsizei size;
Figura FiguraSelected;
float cursorX, cursorY;
/* Variable controlling various rendering modes. */
int stencilReflection = 1, stencilShadow = 1, offsetShadow = 1;
int renderShadow = 0, renderReflection = 0;
int linearFiltering = 0, useMipmaps = 0, useTexture = 0;
int reportSpeed = 0;
int animation = 0;
int directionalLight = 1;
int forceExtension = 0;
int haloMagic = 0, blendedHalo = 0, subMenu, Menu;
GLfloat haloScale = 1.0, haloTime = 0.0;
GLboolean lightSwitch = GL_TRUE;
float **calibPoints;
int puntosCalib = 10;
float* Pe, *Pa, *Pb, *Pc, *Xp, *Yp;
float* plano = NULL;
float* offsetPePa,*offsetPa,*offsetOjo;
Camera *camera;
cModuleVector *vec;
Core::DistortionModel lensDistortion;
cModuleVectorProcessing *vecprocessor;

/* Time varying or user-controled variables. */
int moving, startx, starty;
int lightMoving = 0, lightStartX, lightStartY;
float lightAngle = 0.0, lightHeight = 20, zoom = 40.0;

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
GLubyte checkImage[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][3];
Image *image1, *texture_01, *texture_02, *texture_03, *texture_04, *texture_05;
/*necesarios para implementar los customs solid shapes*/
#define QUAD_OBJ_INIT() { if(!quadObj) initQuadObj(); }
static GLUquadricObj *quadObj;



/* definitions to be used */
#ifndef M_PI
#define M_PI 3.14159265
#define true  1
#define false 0
#define TOROIDE 0
#define CUBO 1
#define ESFERA 2
#define TEAPOT 3
#define LIGHT_HEIGHT 20
#define SIZE 256
#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64
#define MENU 101
#define SALIR 3
#endif
Figura crearFigura(int tipo);
Figura moverFiguraXYZ(Figura f, float x, float y, float z);
Nodo crearNodo(void* content);
void agregarNodo(Lista l, Nodo n);
Lista crearLista();
void doList(Lista l, void(*hacer)(void*));
void dibujarFigura(Figura f);
static void drawFloor(void);
float dot_product(const float* a, const float* b);
void cross_product(float* dst, const float* a, const float* b);
void normalize(float* v);
void subtract(float* dst, const float* a, const float* b);
void VectorSet(float *v, float x, float y, float z);
void projection(const float *pa, const float *pb, const float *pc, const float *pe, float n, float f);
static void drawBox(GLfloat xsize, GLfloat ysize, GLfloat zsize);
static void drawPillar(void);
static void redraw(void);
void calibrar();
static void keyboard(unsigned char key, int x, int y);
float *getPeReal();
void trackingPuntos(Camera *c, Core::DistortionModel lensDist, cModuleVector *vec, cModuleVectorProcessing *vecprocessor);
float getMagnitud(float*v);
void ordenarPuntos(float ** p, int tam);
void makeCheckImage(void);
int ImageLoad(char *filename, Image *image);
Image * loadTexture(char *nameimg);
static void customDrawBox(GLfloat size, GLenum type);
static void initQuadObj(void);
void customSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void customSolidCube(GLdouble size);
void customSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
	GLint nsides, GLint rings);
static void customDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings);
Figura setTextura(Figura f, int i);


static void redraw(void) {
	GLfloat lightColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	//double posx = -13.0, posy = 15.0, posz =-15.0, lookx = 80, looky = -20, lookz = -20, upx = 0, upy = 1, upz = 0;
	double posx = 5, posy = 2, posz = 10, lookx = 0, looky = 0, lookz = 0, upx = 0, upy = 1, upz = 0;
	trackingPuntos(camera, lensDistortion, vec, vecprocessor);
	subtract(Pe, Pe, offsetPa);
	subtract(Pe, Pe, offsetOjo);
	projection(Pa, Pb, Pc, Pe, 0.4f, 200.0);
	glLoadIdentity();
	gluLookAt(posx, posy, posz, lookx, looky, lookz, upx, upy, upz);
	//gluLookAt(zoom, zoom, zoom, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	glShadeModel(GL_SMOOTH);

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

	glutSwapBuffers();
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
void calibrar(){

	/*
	VectorSet(Pa, -49.91 / 2, -38.51-3, 14);
	VectorSet(Pb, 49.91 / 2, -38.51 - 3, 14);
	VectorSet(Pc, -49.91 / 2, -3, 14);

	VectorSet(offsetPa, Pa[0], Pa[1], Pa[2]);
	VectorSet(Pa, 0, 0, 0);
	subtract(Pb, Pb, offsetPa);
	subtract(Pc, Pc, offsetPa);
	*/
	VectorSet(Pa, -49.91 / 2, -38.51 - 3, 14);
	VectorSet(Pb, 49.91 / 2, -38.51 - 3, 14);
	VectorSet(Pc, -49.91 / 2, -3, 14);

	VectorSet(offsetPa, 0, -38.51 / 2 - 3, 14);
	subtract(Pa, Pa, offsetPa);
	subtract(Pb, Pb, offsetPa);
	subtract(Pc, Pc, offsetPa);

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
	f->name = nombreActual;
	f->textura = -1;
	nombreActual++;
	return f;
}
static void keyboard(unsigned char key, int x, int y) {
	if (key == 32){
		FiguraSelected = NULL;
		specialMode = !specialMode;
		if (specialMode){
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
		}
		else{
			glutSetCursor(GLUT_CURSOR_INHERIT);
			glutDetachMenu(GLUT_RIGHT_BUTTON);
		}
	}
	if (key == 105) zoom -= 1.0;
	if (key == 111) zoom += 1.0;
	if (key == 'w') Pe[2] -= 0.5;
	if (key == 's') Pe[2] += 0.5;
	if (key == 'a') Pe[0] -= 0.5;
	if (key == 'd') Pe[0] += 0.5;
	if (key == 'q') Pe[1] -= 0.5;
	if (key == 'e') Pe[1] += 0.5;

	
}
void Inicio(){
	CameraLibrary_EnableDevelopment();
	CameraManager::X().WaitForInitialization();
	camera = CameraManager::X().GetCamera();
	if (camera == 0)
	{
		printf("por favor conecte una camara, no hay camaras conectadas");
		return;
	}

	int cameraWidth = camera->Width();
	int cameraHeight = camera->Height();

	/*Arreglos de punteros a float*(vectores)*/
	calibPoints = (float**) malloc(sizeof(float*) *puntosCalib);
	for (int i = 0; i < puntosCalib; i++){
		calibPoints[i] = (float*) malloc(sizeof(float*) * 3);
		VectorSet(calibPoints[i], 0, 0, 0);
	}
	Pa = (float*) malloc(sizeof(float*) * 3);
	Pb = (float*) malloc(sizeof(float*) * 3);
	Pc = (float*) malloc(sizeof(float*) * 3);
	Xp = (float*) malloc(sizeof(float*) * 3);
	Yp = (float*) malloc(sizeof(float*) * 3);
	
	offsetPePa = (float*) malloc(sizeof(float*) * 3);
	offsetPa = (float*) malloc(sizeof(float*) * 3);
	VectorSet(offsetPePa, 49.91 / 2, 38.51 / 2, 28);

	offsetOjo = (float*) malloc(sizeof(float*) * 3);
	VectorSet(offsetOjo, 0,11,0);

	camera->SetVideoType(SegmentMode);
	//camera->SetShutterDelay(0);
	
	camera->SetEnableBlockingMask(false);
	//== Start camera output ==--
	
	camera->Start();

	camera->SetTextOverlay(false);
	vec = cModuleVector::Create(); //new cModuleVector();
	vecprocessor = new cModuleVectorProcessing();

	

	camera->GetDistortionModel(lensDistortion);
	
	//== Plug distortion into vector module ==--
	
	cVectorSettings vectorSettings;
	vectorSettings = *vec->Settings();
	//vectorSettings.TrueView = true;
	
	vectorSettings.Arrangement = cVectorSettings::VectorClip;
	vectorSettings.Enabled = false;//era true

	cVectorProcessingSettings vectorProcessorSettings;

	vectorProcessorSettings = *vecprocessor->Settings();

	vectorProcessorSettings.Arrangement = cVectorSettings::VectorClip;
	vectorProcessorSettings.ShowPivotPoint = false;
	vectorProcessorSettings.ShowProcessed = false;

	vecprocessor->SetSettings(vectorProcessorSettings);

	//== Plug in focal length in (mm) by converting it from pixels -> mm

	vectorSettings.ImagerFocalLength = (lensDistortion.HorizontalFocalLength / ((float) camera->PhysicalPixelWidth()))*camera->ImagerWidth();

	vectorSettings.ImagerHeight = camera->ImagerHeight();
	vectorSettings.ImagerWidth = camera->ImagerWidth();

	vectorSettings.PrincipalX = camera->PhysicalPixelWidth() / 2;
	vectorSettings.PrincipalY = camera->PhysicalPixelHeight() / 2;

	vectorSettings.PixelWidth = camera->PhysicalPixelWidth();
	vectorSettings.PixelHeight = camera->PhysicalPixelHeight();

	vec->SetSettings(vectorSettings);
	
	
Pe = (float*) malloc(sizeof(float) * 3);

}
void main(int argc, char **argv){
	int i;
	size = 512;
	buffer = (GLuint *) (malloc(size));
	nombreActual = 1;
	selectionMode = 0;
	specialMode = 0;
	Inicio();

	Figuras = crearLista();
	calibrar();
	glLoadName(0);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA),2), 5.0f, 1.5f, 8.0f)));
	glLoadName(1);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 4), -10.0f, 4.5f, 5.0f)));
	glLoadName(2);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 5), -10.0f, 1.5f, -6.0f)));
	glLoadName(3);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 0), 1.0f, 2.5f, 10.0f)));
	glLoadName(4);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA), 3), -12.3f, 1.5f, -5.3f)));
	glLoadName(5);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 3), -10.0f, 7.5f, 2.2f)));
	glLoadName(6);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 4), 3.0f, 1.5f, 4.13f)));
	glLoadName(7);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 5), 3.1f, 2.5f, -11.1f)));
	glLoadName(8);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(ESFERA), 0), -1.22f, 1.5f, -1.34f)));
	glLoadName(9);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(CUBO), 2), 7.0f, 1.5f, 12.2f)));
	glLoadName(10);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TEAPOT), 3), 10.0f, 1.5f, -1.13f)));
	glLoadName(11);
	agregarNodo(Figuras, crearNodo(moverFiguraXYZ(setTextura(crearFigura(TOROIDE), 2), 3.1f, 3.5f, -1.1f)));



	glutInit(&argc, argv);
	glutInitWindowSize(1900,1000);
	for (i = 1; i < argc; i++) {
		if (!strcmp("-linear", argv[i])) {
			linearFiltering = 1;
		}
		else if (!strcmp("-mipmap", argv[i])) {
			useMipmaps = 1;
		}
		else if (!strcmp("-ext", argv[i])) {
			forceExtension = 1;
		}
	}
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
#if 1  
	glutInitDisplayString("samples stencil>=3 rgb double depth");
#endif
	glutCreateWindow("Proyecto final");
	if (glutGet(GLUT_WINDOW_STENCIL_SIZE) < 3) {
		printf("halomagic: Sorry, I need at least 3 bits of stencil.\n");
		exit(1);
	}
	glutDisplayFunc(redraw);
	glutIdleFunc(redraw);


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glutKeyboardFunc(keyboard);


	//glClearColor(0.5, 0.5, 0.5, 0.0);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	
	texture_01 = loadTexture("texture_01.bmp");
	if (texture_01 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}
	texture_02 = loadTexture("texture_02.bmp");
	if (texture_02 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}
	texture_03 = loadTexture("texture_03.bmp");
	if (texture_03 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}
	texture_04 = loadTexture("texture_04.bmp");
	if (texture_04 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}
	texture_05 = loadTexture("texture_05.bmp");
	if (texture_05 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}


	makeCheckImage();
	image1 = loadTexture("chessboard.bmp");
	if (image1 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
	}
	makeCheckImage();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(6, texture);


	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_01->sizeX, texture_01->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_01->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_02->sizeX, texture_02->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_02->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_03->sizeX, texture_03->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_03->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_04->sizeX, texture_04->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_04->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_05->sizeX, texture_05->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_05->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);




	glutMainLoop();	

	//== Release camera ==--

	camera->Release();

	CameraManager::X().Shutdown();
}
void trackingPuntos(Camera *c, Core::DistortionModel lensDist, cModuleVector *vec, cModuleVectorProcessing *vecprocessor){

	//== Fetch a new frame from the camera ===---

	Frame *frame = c->GetFrame();

	if (frame)
	{
		vec->BeginFrame();
		for (int i = 0; i<frame->ObjectCount(); i++)
		{
			cObject *obj = frame->Object(i);
			float x = obj->X();
			float y = obj->Y();
			Core::Undistort2DPoint(lensDist, x, y);
			vec->PushMarkerData(x, y, obj->Area(), obj->Width(), obj->Height());
		}
		vec->Calculate();
		vecprocessor->PushData(vec);
		
		if (vecprocessor->MarkerCount()>0)
		{
			float x, y, z, xt = 0, yt = 0, zt = 0, tam = vecprocessor->MarkerCount();
			for (int i = tam; i>0; i--){
				vecprocessor->GetResult(i, x, y, z);
				xt += x;
				yt += y;
				zt += z;
			}
			if (tam > 0){
				VectorSet(Pe, xt / (tam * 10), yt / (tam * 10), zt / (tam * 10));
			}
		}
		frame->Release();
	}

}
void ordenarPuntos(float ** p, int tam){
	float *temp;
	for (int i = 0; i < tam; i++){
		for (int j = 0; j < tam; j++){
			if (getMagnitud(p[i])>getMagnitud(p[j])){
				temp = p[i];
				p[j] = p[i];
				p[i] = temp;
			}
		}
	}
}
float *getPeReal(){
	ordenarPuntos(calibPoints, puntosCalib);
	float mag;
	float Q1 = getMagnitud(calibPoints[(int) (puntosCalib*0.25)]);
	float Q3 = getMagnitud(calibPoints[(int) (puntosCalib*0.75)]);
	float IQR = Q3 - Q1;
	float Rl = Q1 - IQR;
	float Rr = Q3 + IQR;
	float* Pe = (float*) malloc(sizeof(float) * 3);
	VectorSet(Pe, 0, 0, 0);
	int m = 0;
	for (int i = 0; i < puntosCalib; i++){
		mag = getMagnitud(calibPoints[i]);
		if (mag >= Rl&&mag <= Rl){
			m++;
			Pe[0] += calibPoints[i][0];
			Pe[1] += calibPoints[i][1];
			Pe[2] += calibPoints[i][2];
		}
	}
	Pe[0] = Pe[0] / m;
	Pe[1] = Pe[1] / m;
	Pe[2] = Pe[2] / m;
	return Pe;
}
float getMagnitud(float*v){
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}



/*12/16/2013*/
Figura setTextura(Figura f,int i){
	f->textura = i;
	return f;
}
void makeCheckImage(void){
	int i, j, c;
	for (i = 0; i < CHECK_IMAGE_WIDTH; i++) {
		for (j = 0; j < CHECK_IMAGE_HEIGHT; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
	}
}
int ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size;
	unsigned long i;
	unsigned short int planes;
	unsigned short int bpp;
	char temp;
	if ((file = fopen(filename, "rb")) == NULL){
		printf("File Not Found : %s\n", filename);
		getch();
		return 0;
	}
	fseek(file, 18, SEEK_CUR);
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		getch();
		return 0;
	}
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		getch();
		return 0;
	}
	size = image->sizeX * image->sizeY * 3;
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		getch();
		return 0;
	}
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		getch();
		return 0;
	}
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		getch();
		return 0;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		getch();
		return 0;
	}
	fseek(file, 24, SEEK_CUR);
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		getch();
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		getch();
		return 0;
	}
	for (i = 0; i<size; i += 3) {
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	fclose(file);
	return 1;
}

Image * loadTexture(char *nameimg){
	Image* image;
	image = (Image *) malloc(sizeof(Image));
	if (image == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	if (!ImageLoad(nameimg, image)) {
		exit(1);
	}
	return image;
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




static void initQuadObj(void)
{
	quadObj = gluNewQuadric();
	if (!quadObj)
		printf("out of memory.");
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


static void customDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
	int i, j, n;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;

	ringDelta = 2.0 * M_PI / rings;
	sideDelta = 2.0 * M_PI / nsides;
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








