/*
 * FILE:
 *      proyectoFinal.c
 * 		LAST MODIFIED: 2013/09/17
 * --------------------------------------------------------------------------
 * AUTHOR
 *		David Barrera
 *		Liliana Ramos
 *		Denny Schuldt
 * DESCRIPTION:
 * 		This file implements functions to be used to the final project.
 * DEPENDENCIES:
 *		string.h
 *		gl.h
 *		glut.h
 *		windows.h
 *		math.h
 *		conio.h
 *		stdio.h
 */


/*Libraries*/
#include "../ProyectoFinal/proyectoFinal.h"
#include <string.h>
#include <windows.h>
#include <GL/gl.h>
#include "../lib/glut.h"
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>

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
int nombreCascaraActual;
Cluster cluster;
float *a,*b,*c,*e;
/* Variable controlling various rendering modes. */
int stencilReflection = 1, stencilShadow = 1, offsetShadow = 1;
int renderShadow = 0, renderReflection = 0;
int linearFiltering = 0, useMipmaps = 0, useTexture = 0;
int reportSpeed = 0;
int animation = 0;
int directionalLight = 1;
int forceExtension = 0;
int haloMagic = 0, blendedHalo = 0, subMenu,Menu;
GLfloat haloScale = 1.0, haloTime = 0.0;
GLboolean lightSwitch = GL_TRUE;

/* Time varying or user-controled variables. */
int moving, startx, starty;
int lightMoving = 0, lightStartX, lightStartY;
float lightAngle = 0.0, lightHeight = 20, zoom = 40.0;

/*necesarios para implementar los customs solid shapes*/
#define QUAD_OBJ_INIT() { if(!quadObj) initQuadObj(); }
static GLUquadricObj *quadObj;

static void customDrawBox(GLfloat size, GLenum type);
void customSolidCube(GLdouble size);
void customSolidSphere(GLdouble radius, GLint slices, GLint stacks);
static void customDoughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings);
void customSolidTorus(GLdouble innerRadius, GLdouble outerRadius,GLint nsides, GLint rings);

//Position of the lights
GLfloat lightPositionCenterLeft[]  = {0.0, LIGHT_HEIGHT, 5.0, 0.0};
GLfloat lightPositionCenterRight[] = {0.0, LIGHT_HEIGHT, -5.0, 0.0};
GLfloat lightPositionNear[]        = {15.0, LIGHT_HEIGHT, 0.0, 0.0};
GLfloat lightPositionFar[]         = {-15.0, LIGHT_HEIGHT, 0.0, 0.0};
GLfloat lightPositionLeft[]        = {0.0, LIGHT_HEIGHT, 15.0, 0.0};
GLfloat lightPositionRight[]       = {0.0, LIGHT_HEIGHT, -15.0, 0.0};

//Light colors
GLfloat lightColor[] = {0.8, 1.0, 0.8, 1.0};
GLfloat skinColor[]  = {0.1, 1.0, 0.1, 1.0};
GLfloat eyeColor[]   = {1.0, 0.2, 0.2, 1.0};
//GLfloat AmbientLight[] = {1.0,1.0,1.0};

//light controllers
int light0 = 1;
int light1 = 1;
int light2 = 1;
int light3 = 1;
int light4 = 1;
int light5 = 1;

//Material colors
GLfloat blueMaterial[] = {0.0, 0.2, 1.0, 1.0};
GLfloat	redMaterial[] = {0.6, 0.1, 0.0, 1.0};
GLfloat	purpleMaterial[] = {0.3, 0.0, 0.3, 1.0};
GLfloat	greenMaterial[] = {1.0, 0.2, 0.0, 1.0};

GLuint texture[6];
GLubyte checkImage[CHECK_IMAGE_WIDTH][CHECK_IMAGE_HEIGHT][3];
Image *image1,*image2, *texture_01, *texture_02, *texture_03, *texture_04, *texture_05;

//Modify plane size
GLfloat floorVertices[4][3] = {
	{ -15.0, 0.0,  15.0 },
	{  15.0, 0.0,  15.0 },
	{  15.0, 0.0, -15.0 },
	{ -15.0, 0.0, -15.0 },
};
GLfloat floorPlane[4];
GLfloat floorShadow[4][4];
GLfloat angle = -150;
GLfloat angle2 = 30;

int centinel = 0;
float* VectorNew();
void calibrar();
void VectorSet(float *v,float x,float y,float z);
void projection(const float *pa,
const float *pb,
const float *pc,
const float *pe, float n, float f);
void subtract(float* dst,const float* a,const float* b);
void normalize(float* v);
void cross_product(float* dst,const float* a,const float* b);
float dot_product(const float* a,const float* b);
/***
 *
 * @brief dibujarArco draws an arc
 *
 * @Details Use:
 * Arco a;
 * int esFig;
 * dibujarArco( a, esFig);
 * @param a is data structure arc
 * @param esFig is a flag
 */


void dibujarArco(Arco a, int esFig){
	glPushMatrix();
		glLineWidth(2.5); 
		glColor3f(1.0f, 1.0f, 0.2f);
		glBegin(GL_LINES);
			if(esFig){
				glVertex3f(((Figura)(a->inicio))->x,((Figura)(a->inicio))->y,((Figura)(a->inicio))->z);
				glVertex3f(((Figura)(a->fin))->x,((Figura)(a->fin))->y,((Figura)(a->fin))->z);
			}else{
				glVertex3f(((Cascara)(a->inicio))->x,((Cascara)(a->inicio))->y,((Cascara)(a->inicio))->z);
				glVertex3f(((Cascara)(a->fin))->x,((Cascara)(a->fin))->y,((Cascara)(a->fin))->z);
			}
		glEnd();
	glPopMatrix();
}


/***
 *
 * @brief dibujarArcoFigura This function draws an arc that joins two figures
 *
 * @Details Use:
 * Arco a;
 * dibujarArcoFigura( a);
 * @param a is data structure arc
 */

void dibujarArcoFigura(Arco a){
	glPushMatrix();
		glLineWidth(2.5); 
		glColor3f(1.0f, 1.0f, 0.2f);
		glBegin(GL_LINES);
			glVertex3f(((Figura)(a->inicio))->x,((Figura)(a->inicio))->y,((Figura)(a->inicio))->z);
			glVertex3f(((Figura)(a->fin))->x,((Figura)(a->fin))->y,((Figura)(a->fin))->z);
		glEnd();
	glPopMatrix();
}

/***
 *
 * @brief listaSize This function gets a lists size
 *
 * @Details Use:
 * Lista l;
 * listaSize(l);
 * @param l is a data structure of list type
 */
int listaSize(Lista l){
	Nodo n;
	int i=0;
	for(n=l->inicio;n!=NULL;n=n->next){
		i++;
	}
	return i;
}

/***
 *
 * @brief getDistanciaAlCuadradoCF This function gets the square distance between a shell and a figure
 *
 * @Details Use:
 * Cascara c;
 * Figura f;
 * getDistanciaAlCuadradoCF(c,f);
 * @param c is a data structure of shell type
 * @param f is a data structure of figure type
 */
float getDistanciaAlCuadradoCF(Cascara c,Figura f){
	float dx=((f->x)-(c->x));
	float dy=((f->y)-(c->y));
	float dz=((f->z)-(c->z));
	return dx*dx + dy*dy + dz*dz;
}

/***
 *
 * @brief calcularCascara This function calculates the positcion and radius the shell has
 *
 * @Details Use:
 * Cascara c;
 * int isFig;
 * calcularCascara(c,isFig);
 * @param c is a data structure of shell type
 * @param isFig is a flag
 */
void calcularCascara(Cascara c,int isFig){
	Lista l=c->Figuras;
	Nodo n;
	Figura f;
	Cascara cas,c2;
	int i=0;
	float x=0;
	float y=0;
	float z=0;
	float d;
	float r=0;
	for(n=l->inicio;n!=NULL;n=n->next){
		if(isFig){
			f=((Figura)(n->contenido));
			x+=f->x;
			y+=f->y;
			z+=f->z;
		}else{
			cas=((Cascara)(n->contenido));
			x+=cas->x;
			y+=cas->y;
			z+=cas->z;
		}
		i++;
	}
	c->x=x/i;
	c->y=y/i;
	c->z=z/i;
	for(n=l->inicio;n!=NULL;n=n->next){
		if(isFig){
			f = ((Figura)(n->contenido));
			d = getDistanciaAlCuadradoCF(c,f);
		}else{
			cas = ((Cascara)(n->contenido));
			d = getDistanciaAlCuadrado(c,cas,0);
		}
		if(d>r){
			r=d;
			if(!isFig){c2=cas;}
		}
	}
	if(isFig){
		c->r=sqrt(r)+1.5f;
	}else{
		c->r=sqrt(r)+c2->r;
	}
	
}
/***
 *
 * @brief estaDentroLista This function determines if an element is inside of a list
 *
 * @Details Use:
 * Lista list;
 * void *c;
 * estaDentroLista(list,c);
 * @param list is a data structure of list type
 * @param c is a void pointer
 */
int estaDentroLista(Lista list,void* c){
	Nodo n1;
	for(n1=list->inicio;n1!=NULL;n1=n1->next){
		if(n1->contenido==c){
			return 1;//ya esta alli
		}
	}
	return 0;
}
/***
 *
 * @brief estaDentroListaArcos This function determines if an element is inside of a list of arcs
 *
 * @Details Use:
 * Lista list;
 * void *c;
 * estaDentroListaArcos(Lista list,void* c);
 * @param list is a data structure of list type
 * @param c is a void pointer
 */
int estaDentroListaArcos(Lista list,void* c){
	Nodo n1;
	for(n1=list->inicio;n1!=NULL;n1=n1->next){
		if(((Arco)(n1->contenido))->fin==c||((Arco)(n1->contenido))->inicio==c){
			return 1;//ya esta alli
		}
	}
	return 0;
}
/***
 *
 * @brief Agrupar This function joins elements whithin shells
 *
 * @Details Use:
 * Lista arcos;
 * int Figs;
 * Agrupar(arcos, Figs);
 * @param list is a data structure of list type
 * @param Figs is a flag
 */
Lista Agrupar(Lista arcos, int Figs){//si no es de figuras es de cascaras
	Lista l=crearLista();
	Lista sub;
	Nodo n1,casc;
	Arco arc;
	void* v1;
	void *v2;
	int i1,i2;
	Cascara c;
	for(n1=arcos->inicio;n1!=NULL;n1=n1->next){
		i1=0;
		i2=0;
		v1=((Arco)(n1->contenido))->inicio;
		v2=((Arco)(n1->contenido))->fin;
		for(casc=l->inicio;!i1&&!i2&&casc!=NULL;casc=casc->next){
			i1=estaDentroListaArcos(((Cascara)(casc->contenido))->Figuras,v1);
			i2=estaDentroListaArcos(((Cascara)(casc->contenido))->Figuras,v2);
			if((i1&&!i2)||(!i1&&i2)){//si por si acaso alguno se encuentra dentro
				agregarNodo(((Cascara)(casc->contenido))->Figuras,crearNodo((Arco)(n1->contenido)));
				v1=NULL;
				v2=NULL;
			}
		}
		if(v2!=NULL&&!(i1&&i2)){//si ambos estan dentro no se hace nada
			c=cascaraNew();
			agregarNodo(c->Figuras,crearNodo((n1->contenido)));
			agregarNodo(l,crearNodo(c));
		}
	}
	for(casc=l->inicio;casc!=NULL;casc=casc->next){
		sub=((Cascara)(casc->contenido))->Figuras;
		((Cascara)(casc->contenido))->Figuras=crearLista();
		for(n1=sub->inicio;n1!=NULL;n1=n1->next){
			v1=((Arco)(n1->contenido))->inicio;
			v2=((Arco)(n1->contenido))->fin;
			if(!estaDentroLista(((Cascara)(casc->contenido))->Figuras,v1)){
				agregarNodo(((Cascara)(casc->contenido))->Figuras,crearNodo(v1));
			}
			if(!estaDentroLista(((Cascara)(casc->contenido))->Figuras,v2)){
				agregarNodo(((Cascara)(casc->contenido))->Figuras,crearNodo(v2));
			}
		}
	}
	for(casc=l->inicio;casc!=NULL;casc=casc->next){
		calcularCascara((Cascara)(casc->contenido),Figs);
	}
	return l;
}
/***
 *
 * @brief IniciarClustering This function starts the process of clustering
 *
 * @Details Use:
 * Lista Figs;
 * int isFigs;
 * IniciarClustering(Figs,isFigs);
 * @param Fisgs is a data structure of list type
 * @param isFigs is a flag
 */
Lista IniciarClustering(Lista Figs,int isFigs){
	Lista arc = crearLista();
	Nodo n1, n2, nCercano;
	float min_dist;
	float d;
	for(n1=Figs->inicio; n1!=NULL; n1=n1->next){
		min_dist=999999;
		for(n2=Figs->inicio;n2!=NULL;n2=n2->next){
			if(n1!=n2){
				d=getDistanciaAlCuadrado((n1->contenido),(n2->contenido),isFigs);
				if(d<min_dist){
					nCercano=n2;
					min_dist=d;
				}
			}
		}
		if(Figs->fin->next != NULL)
		agregarNodo(arc,crearNodo(unirNodos((n1->contenido),(nCercano->contenido),min_dist)));
	}
	return arc;
}
/***
 *
 * @brief getDistanciaAlCuadrado This function gets the square distance between two objects
 *
 * @Details Use:
 * void* v1;
 * void* v2;
 * int isFigs;
 * getDistanciaAlCuadrado(v1,v2,isFig);
 * @param v1 is a void pointer
 * @param v2 is a void pointer
 * @param isFigs is a flag
 */

float getDistanciaAlCuadrado(void* v1,void* v2,int isFig){
	float dx,dy,dz;	
	if(isFig){
		dx=(((Figura)v2)->x-((Figura)v1)->x);
		dy=(((Figura)v2)->y-((Figura)v1)->y);
		dz=(((Figura)v2)->z-((Figura)v1)->z);
	}else{
		dx=(((Cascara)v2)->x-((Cascara)v1)->x);
		dy=(((Cascara)v2)->y-((Cascara)v1)->y);
		dz=(((Cascara)v2)->z-((Cascara)v1)->z);
	}	
	return dx*dx + dy*dy + dz*dz;
}
/***
 *
 * @brief unirNodos This function gets an arc that joins two elements
 *
 * @Details Use:
 * void* inicio;
 * void* fin;
 * float peso;
 * unirNodos(inicio, fin,peso);
 * @param inico is a void pointer
 * @param fin is a void pointer
 * @param peso is the weight of an arc
 */

Arco unirNodos(void* inicio, void* fin,float peso){
	Arco a = (Arco)(malloc(sizeof(ArcoStruct)));
	a->inicio = inicio;
	a->fin = fin;
	a->peso = peso;
	return a;
}
/***
 *
 * @brief clusterNew This function creates a cluster
 *
 * @Details Use: clusterNew();
 */

Cluster clusterNew(){
	Cluster c=(Cluster)(malloc(sizeof(ClusterStruct)));
	c->ArcosFiguras=NULL;//en iniciar clustering se crea la lista
	c->niveles=crearLista();
	return c;
}
/***
 *
 * @brief cascaraNew This function creates a shell
 *
 * @Details Use: cascaraNew();
 */

Cascara cascaraNew(){
	Cascara c=(Cascara)(malloc(sizeof(CascaraStruct)));
	c->Figuras=crearLista();
	c->name=nombreCascaraActual;
	nombreCascaraActual--;
	return c;
}
/***
 *
 * @brief cascaraAdd This function joins a figure to a shell
 *
 * @Details Use:
 * Cascara c;
 * Figura f;
 * cascaraAdd(c,f);
 * @param c is a data structure of shell type
 * @param f is a data structure of figure type
 */
void cascaraAdd(Cascara c,Figura f){ agregarNodo(c->Figuras,crearNodo(f));}

/***
 *
 * @brief cascaraDraw This function draws a shell
 *
 * @Details Use:
 * Cascara c;
 * cascaraDraw(c);
 * @param c is a data structure of shell type
 */
void cascaraDraw(Cascara c){
	if(selectionMode){
		glPushName(c->name);
	}
	glPushMatrix();
		glTranslatef(c->x,c->y,c->z);	
		glColor3f (1.0f,1.0f,1.0f);
		glutWireSphere(c->r, 8, 8);
	glPopMatrix();
	if(selectionMode){
		glPopName();
	}
}

/***
 *
 * @brief doList This function executes a function to each element of a list
 *
 * @Details Use:
 * Lista l;
 * void (*hacer)( void*);
 * doList(l, (*hacer)( void*))
 * @param l is a data structure of list type
 * @param (*hacer)( void*) is a callback to a function 
 */

void doList(Lista l,void (*hacer)( void*)){
	Nodo ultimo = NULL;
	for(ultimo=l->inicio; ultimo != NULL; ultimo=ultimo->next){
		hacer((ultimo->contenido));
	}
}
/***
 *
 * @brief crearFigura This function creates a new figure
 *
 * @Details Use:
 * int tipo;
 * crearFigura(tipo)
 * @param tipo is the type of a figure 
 */
Lista crearLista() {
	Lista l = (Lista)(malloc(sizeof(ListaStruct)));
	l->inicio=NULL;
	l->fin=NULL;
    return l;
}


/***
 *
 * @brief crearNodo This function create a new node
 *
 * @Details Use:
 * void* content;
 * crearNodo( content);
 * @param content is a void pointer
 */

Nodo crearNodo(void* content) {
	Nodo n = (Nodo)(malloc(sizeof(NodoStruct)));
	n->contenido=content;
	n->next=NULL;
    return n;
}

/***
 *
 * @brief agregarNodo This function add a node to a list
 *
 * @Details Use:
 * Lista l;
 * Nodo n
 * agregarNodo(l, n);
 * @param l is a list type
 * @param n is a node of a list
 */


void agregarNodo(Lista l,Nodo n) {
	Nodo ultimo =l->inicio;
	while(true){
		if(ultimo==NULL){
			l->inicio=n;
			l->fin=n;
			return;
		}
		if(ultimo->next==NULL){
			ultimo->next=n;
			l->fin=n;
			return;
		}else{
			ultimo=ultimo->next;
		}
	}
}

/***
 *
 * @brief crearFigura This function create a new figure
 *
 * @Details Use:
 * int tipo;
 * crearFigura(tipo);
 * @param tipo is the type of a figure
 */

Figura crearFigura(int tipo){
	Figura f = (Figura)(malloc(sizeof(FiguraStruct)));
    f->x = 0.00f;
    f->y = 0.00f;
	f->z = 0.00f;
	f->a_x = 0.00f;
    f->a_y = 0.00f;
	f->a_z = 0.00f;
	f->spin=0;
	f->orbit=0;
	f->tipo=tipo;
	f->name=nombreActual;
	f->textura=-1;
	nombreActual++;
    return f;
}
/***
 *
 * @brief dibujarFigura This function draws a figure
 *
 * @Details Use:
 * Figura f;
 * dibujarFigura(f);
 * @param f is a data structure of list
 */
void dibujarFigura(Figura f){
	const GLfloat mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f};
	const GLfloat mat_diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f};
	const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	const GLfloat high_shininess[] = { 10.0f};
//Asignamos estos valores a los componentes delmaterial
glMaterialfv(GL_FRONT,GL_AMBIENT, mat_ambient);
glMaterialfv(GL_FRONT,GL_DIFFUSE, mat_diffuse);
glMaterialfv(GL_FRONT,GL_SPECULAR, mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS, high_shininess);
	if(f->textura!=-1){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[f->textura]);
	}
	if(selectionMode){
		glPushName(f->name);
	}
	if(f->orbit||f->spin){
		f->a_y+=4.0f;
	}
	glPushMatrix();
		glTranslatef(f->x,f->y,f->z);	
		glRotatef(f->a_x,1,0,0);
		glRotatef(f->a_y,0,1,0);
		glRotatef(f->a_z,0,0,1);
		if(f->orbit){
			glTranslatef(4.0f,0,0);
		}
		if(f->spin){
		glRotatef(f->a_y,0,1,0);
		}
		if(f == FiguraSelected){
				glColor3f (0.0f,1.0f,0.0f);
			}else{
				if(f->textura!=-1){
				glColor3f (1.0f,1.0f,1.0f);
				}else{
				glColor3f (0.3f,0.6f,1.0f);
				}
			}
		
		
		switch(f->tipo){
			case TOROIDE:
				customSolidTorus(0.5,1,10,14);
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
	if(selectionMode){
		glPopName();
	}
	if(f->textura!=-1){
		glDisable(GL_TEXTURE_2D);
	}
}


/***
 *
 * @brief moverFiguraXYZ This function moves a figure
 *
 * @Details Use:
 * Figura f;
 * float x;
 * float y;
 * float z;
 * moverFiguraXYZ(f,x,y,z);
 * @param f is a data structure of list 
 * @param x is the position in x 
 * @param y is the position in y
 * @param z is the position in z
 */

Figura moverFiguraXYZ(Figura f,float x,float y,float z){
	f->x+=x;
	f->y+=y;
	f->z+=z;
	return f;
}

/***
 *
 * @brief moverFiguratoXYZ This function moves a figure
 *
 * @Details Use:
 * Figura f;
 * float x;
 * float y;
 * float z;
 * moverFiguratoXYZ(f,x,y,z);
 * @param f is a data structure of list 
 * @param x is the position in x 
 * @param y is the position in y
 * @param z is the position in z
 */
Figura moverFiguratoXYZ(Figura f,float x,float y,float z){
	f->x=x;
	f->y=y;
	f->z=z;
	return f;
}


/***
 *
 * @brief figuraToogleSpin This function turns off and on the spinning process
 *
 * @Details Use:
 * Figura f;
 * figuraToogleSpin(f);
 * @param f is a data structure of list
 */

void figuraToogleSpin(Figura f){ f->spin=!(f->spin); }

/***
 *
 * @brief figuraOrbit This function turns off and on the orbiting process
 *
 * @Details Use:
 * Figura f;
 * figuraOrbit(f);
 * @param f is a data structure of list
 */

void figuraOrbit(Figura f){ f->orbit=!(f->orbit); }

/***
 *
 * @brief moveToSpace This function turns off and on the orbiting process
 *
 * @Details Use:
 * Figura f;
 * int x;
 * int y;
 * void moveToSpace(Figura f,int x, int y);
 * @param f is a data structure of list
 * @param x is the position in x on screen
 * @param x is the position in y on screen
 */

void moveToSpace(Figura f,int x, int y){
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    glReadPixels(winX,winY,1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	moverFiguratoXYZ(f,(float)posX, ((float) posY)+1.5f, ((float) posZ));
}

/***
 *
 * @brief startPicking This function starts the picking process
 *
 * @Details Use:
 * int cursorX;
 * int cursorY;
 * startPicking(int cursorX, int cursorY);
 * @param cursorX is the position in x of the mouse
 * @param cursorY is the position in y of the mouse
 */
void startPicking(int cursorX, int cursorY) {
	GLint viewport[4];
	glSelectBuffer(size,buffer);
	glRenderMode(GL_SELECT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glGetIntegerv(GL_VIEWPORT,viewport);
		gluPickMatrix(cursorX,viewport[3]-cursorY,10,10,viewport);
		gluPerspective(40.0,1.0,20.0,100.0);
		glMatrixMode(GL_MODELVIEW);
		glInitNames();
}

/***
 *
 * @brief drawBox This function draws a box
 *
 * @Details Use:
 * GLfloat xsize;
 * GLfloat ysize;
 * GLfloat zsize;
 * drawBox(xsize, ysize, zsize);
 * @param xsize is the size in x of the box
 * @param ysize is the size in y of the box
 * @param zsize is the size in z of the box
 */

static void drawBox(GLfloat xsize, GLfloat ysize, GLfloat zsize) {
	static GLfloat n[6][3] = {
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] = {
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
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
			glVertex3fv(&v[faces[i][0]][0]);
			glVertex3fv(&v[faces[i][1]][0]);
			glVertex3fv(&v[faces[i][2]][0]);
			glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}
/***
 *
 * @brief drawPillar This function draws a box
 *
 * @Details Use:drawPillar();
 */
static void drawPillar(void) {
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, greenMaterial);
	glPushMatrix();
		glTranslatef(8.0, 10.0, 8.0);
		drawBox(1.0, 20.0, 1.0);
	glPopMatrix();
	glDisable(GL_NORMALIZE);
}

/***
 *
 * @brief drawFloor This function draws a box
 *
 * @Details Use:drawPillar();
 */
static void drawFloor(void) {	
	//glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor3f (1.0f,1.0f,1.0f);
	glTexCoord2f(0.0, 0.0);
		glVertex3fv(floorVertices[0]);
		glTexCoord2f(0.0, 1.0); 
		//glTexCoord2f(0.0, 16.0);
		glVertex3fv(floorVertices[1]);
		 glTexCoord2f(1.0, 1.0); 
		//glTexCoord2f(16.0, 16.0);
		glVertex3fv(floorVertices[2]);
		//glTexCoord2f(16.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3fv(floorVertices[3]);		
		glEnd();	
	//glEnable(GL_LIGHTING);
}

/***
 *
 * @brief redraw This function draws the scene
 *
 * @Details Use: redraw();
 */

static void redraw(void) {
	GLfloat lightColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	double posx=0.0,posy=10.0,posz=35.0,lookx=0,looky=1,lookz=0,upx=0,upy=0,upz=-1;


	projection(a,b,c,e,0.4f,100.0);
	glLoadIdentity();
	 gluLookAt(posx,posy,posz,lookx,looky,lookz,upx,upy,upz);
	 //gluLookAt(zoom, zoom, zoom, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);
	glDisable(GL_CULL_FACE);   
	glEnable(GL_COLOR_MATERIAL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	glShadeModel(GL_SMOOTH);

	glPushMatrix();
		glRotatef(angle2, 1.0, 0.0, 0.0);
		glRotatef(angle, 0.0, 1.0, 0.0);

	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[texturaPiso]);	
			drawFloor();
		glDisable(GL_TEXTURE_2D);
		glColor3f(0.72f,0.45f,0.2f);
		drawPillar();


		

		if(selectionMode){
			glInitNames();
		}
		if(!specialMode&&FiguraSelected!=NULL){
			moveToSpace(FiguraSelected,cursorX,cursorY);
		}

			doList(Figuras,((void(*)( void*))dibujarFigura));
		/*doList((Lista)((cluster->niveles->inicio)->contenido),(void(*)(void*))cascaraDraw);
		doList((Lista)((cluster->niveles->inicio->next)->contenido),(void(*)(void*))cascaraDraw);
		doList((Lista)((cluster->niveles->inicio->next->next)->contenido),(void(*)(void*))cascaraDraw);
		doList(cluster->ArcosFiguras,(void(*)(void*))dibujarArcoFigura);*/
	glPopMatrix();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	if(light0 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT0);
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT0,GL_POSITION,lightPositionNear);		
			drawLightArrow(15.0, 20.0, 0.0, LIGHT_NEAR);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT0);
	}
	if(light1 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT1);
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT1, GL_POSITION, lightPositionFar);
			drawLightArrow(-15.0, 20.0, 0.0, LIGHT_FAR);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT1);
	}
	if(light2 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT2);
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT2, GL_POSITION, lightPositionLeft);
			drawLightArrow(0.0, 20.0, 15.0, LIGHT_LEFT);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT2);
	}
	if(light3 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT3);
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT3, GL_POSITION, lightPositionRight);
			drawLightArrow(0.0, 20.0, -15.0, LIGHT_RIGHT);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT3);
	}
	if(light4 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT4);
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT4, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT4, GL_POSITION, lightPositionCenterLeft);
			drawLightArrow(0.0, 20.0, 5.0, LIGHT_CENTER_LEFT);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT4);
	}
	if(light5 > 0){
		glPushMatrix();
			glEnable(GL_LIGHT5);	
			glRotatef(angle2, 1.0, 0.0, 0.0);
			glRotatef(angle, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT5, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT5, GL_POSITION, lightPositionCenterRight);
			drawLightArrow(0.0, 20.0, -5.0, LIGHT_CENTER_RIGHT);
		glPopMatrix();
	}else{
		glDisable(GL_LIGHT5);
	}

	glutSwapBuffers();
}
/*
 * @brief retrieves the first figure that overlaps figure f, return null if no figure overlaps it
 * @Details Use:
 * Figura f;
 * getFiguraQueColisona(Figura f);
 * @param f is a figure 
 */

Figura getFiguraQueColisona(Figura f){
	Nodo n;
	for(n=Figuras->inicio;n!=NULL;n=n->next){
		if(f!=n->contenido&&Colisiona(f,(Figura)(n->contenido))){
			return (Figura)n->contenido;
		}
	}
	return NULL;
}
double getTColision3D(double x1,double y1,double z1,double x2,double y2, double z2){
	double dx,dy,dz,r,a,b,c,discriminante,t1,t2,xi,yi,zi;
		
	xi=x1-x2;
	yi=y1-y2;
	zi=z1-z2;

	dx=-xi;//Xm-xi
	dy=-yi;//Xm-xi
	dz=-zi;//Xm-xi
	r=3.0f;
	a=(dx*dx)+(dy*dy)+(dz*dz);
	b=2*(xi*dx)+(yi*dy)+(zi*dz);
	c=(xi*xi)+(yi*yi)+(zi*zi)-(r*r);
	discriminante=(b*b)- (4*a*c);
	if(discriminante<0){
	return -1;
	}
	t1=(-b-sqrt(discriminante))/(2*a);
	t2=(-b+sqrt(discriminante))/(2*a);
	if(abs(t1)>abs(t2)){
		return t1;
	}
	return t2;
}
float getTColision2D(double x1,double z1, double r1,double x2,double z2, double r2){
	double dx,dz,r,a,b,c,discriminante,t1,t2,xi,zi;
		
	xi=x1-x2;
	zi=z1-z2;

	dx=-xi;
	dz=-zi;
	
	r=r1+r2;
	a=dx*dx+dz*dz;
	b=2*(xi*dx+zi*dz);
	c=(xi*xi)+(zi*zi)-(r*r);
	discriminante=b*b - 4*a*c;
	if(discriminante<0){
	return -1;
	}
	t1=(-b-sqrt(discriminante))/(2*a);
	t2=(-b+sqrt(discriminante))/(2*a);
	if(abs(t1)>abs(t2)){
		return t1;
	}
	return t2;
}
int estaCercaNoY(float x1,float z1,float x2,float z2){
	float min_dist=(1.0f+3.0f)/2;
	float dx,dz;
	dx=x1-x2;
	dz=z1-z2;
	return sqrt((dx*dx)+(dz*dz))<min_dist;
}
int Colisiona(Figura f1,Figura f2){
	float min_dist=3.0f;
	float dx,dy,dz;
	dx=f1->x-f2->x;
	dy=f1->y-f2->y;
	dz=f1->z-f2->z;
	return sqrt((dx*dx)+(dy*dy)+(dz*dz))<min_dist;
}
void redrawObjects(void){
	glLoadIdentity();
	gluLookAt(zoom, zoom, zoom, 0.0, 8.0, 0.0, 0.0, 1.0, 0.0);
	glDisable(GL_CULL_FACE);   
	glEnable(GL_COLOR_MATERIAL);
	if ((stencilReflection && renderReflection) || (stencilShadow && renderShadow) || (haloScale > 1.0)) {
		glStencilMask(0xffffffff);
		glClearStencil(0x4);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	} else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	glPushMatrix();
		glRotatef(angle2, 1.0, 0.0, 0.0);
		glRotatef(angle, 0.0, 1.0, 0.0);
		if(selectionMode){
			glInitNames();
		}
		if(!specialMode&&FiguraSelected!=NULL){
			moveToSpace(FiguraSelected,cursorX,cursorY);
		}
		doList(Figuras,((void(*)( void*))dibujarFigura));
	glPopMatrix();	
	glEnable(GL_CULL_FACE);
	glutSwapBuffers();
}


void drawLightArrow(GLfloat move_x, GLfloat move_y, GLfloat move_z, light_position position){
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		glTranslatef(move_x, move_y, move_z);
		switch(position){
			case LIGHT_CENTER_LEFT:
				glRotatef(90, 0, 0, 1);
			break;
			case LIGHT_CENTER_RIGHT:
				glRotatef(90, 0, 0, 1);
			break;
			case LIGHT_NEAR:
				glRotatef(45, 0, 0, 1);
			break;
			case LIGHT_FAR:
				glRotatef(135, 0, 0, 1);
			break;
			case LIGHT_LEFT:
				glRotatef(-90, 0, 1, 0);
				glRotatef(45, 0, 0, 1);
			break;
			case LIGHT_RIGHT:
				glRotatef(90, 0, 1, 0);
				glRotatef(55, 0, 0, 1);
			break;
		}
		glBegin(GL_TRIANGLE_FAN);
			glVertex3f(0, 0, 0);
			glVertex3f(2, 1, 1);
			glVertex3f(2, -1, 1);
			glVertex3f(2, -1, -1);
			glVertex3f(2, 1, -1);
			glVertex3f(2, 1, 1);
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
			glVertex3f(0.1, 0, 0);
			glVertex3f(5, 0, 0);
		glEnd();
		glEnable(GL_LIGHTING);
	glPopMatrix();
}

static void keyboard (unsigned char key,int x,int y) {
	if(key==32){
		FiguraSelected = NULL;
		specialMode =! specialMode;
		if(specialMode){
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
		}else{
			glutSetCursor(GLUT_CURSOR_INHERIT);
			glutDetachMenu(GLUT_RIGHT_BUTTON);
		}
	}
	if(key==49) light0 = -light0;
	if(key==50) light1 = -light1;
	if(key==51) light2 = -light2;
	if(key==52) light3 = -light3;
	if(key==53) light4 = -light4;
	if(key==54) light5 = -light5;
	if(key==105) zoom -= 1.0;
	if(key==111) zoom += 1.0;
	if(key=='w') e[2]-=0.5;
	if(key=='s') e[2]+=0.5;
	if(key=='a') e[0]-=0.5;
	if(key=='d') e[0]+=0.5;
	if(key=='q') e[1]-=0.5;
	if(key=='e') e[1]+=0.5;
}

static void mouse(int button, int state, int x, int y) {
	float pos,t;
	Figura f;
	if(specialMode){
		if (button == GLUT_LEFT_BUTTON) {
			if (state == GLUT_DOWN) {
				selectionMode = 1;
				startPicking(x,y);
				redrawObjects();
				FiguraSelected = getFigura(Figuras,stopPicking());
			}
		}	
	}else{
		if (button == GLUT_LEFT_BUTTON) {
			if (state == GLUT_DOWN) {
				moving = 1;
				startx = x;
				starty = y;
			}
			if (state == GLUT_UP) {
				moving = 0;
			};
		}
		if (button == GLUT_MIDDLE_BUTTON) {
			if (state == GLUT_DOWN) {
				lightMoving = 1;
				lightStartX = x;
				lightStartY = y;
			}
			if (state == GLUT_UP) lightMoving = 0;
		}
		if (button == GLUT_RIGHT_BUTTON) {
			if (state == GLUT_DOWN) {
				cursorX = x;
				cursorY = y;
				selectionMode = 1;
				startPicking(x,y);
				redrawObjects();
				FiguraSelected = getFigura(Figuras,stopPicking());
			}
			if (state == GLUT_UP) {
				if(FiguraSelected!=NULL){
					f=getFiguraQueColisona(FiguraSelected);
					//printf("name selected %d\n",FiguraSelected->name);
				
				if(f!=NULL){
					//t=getTColision3D(f->x,f->y,f->z,FiguraSelected->x,FiguraSelected->y,FiguraSelected->z);
					//moverFiguratoXYZ(FiguraSelected,f->x+t*(FiguraSelected->x-f->x),f->y,f->z+t*(FiguraSelected->z-f->z));
					t=getTColision2D(FiguraSelected->x,FiguraSelected->z,1.5f,f->x,f->z,1.5f);
					moverFiguratoXYZ(FiguraSelected,f->x+t*(FiguraSelected->x-f->x),f->y,f->z+t*(FiguraSelected->z-f->z));
				}
					if(estaCercaNoY(FiguraSelected->x,FiguraSelected->z,8.0,8.0)){
						t=getTColision2D(FiguraSelected->x,FiguraSelected->z,1.5f,8.0,8.0,0.5f);
						moverFiguratoXYZ(FiguraSelected,FiguraSelected->x+t*(8.0-FiguraSelected->x),FiguraSelected->y,FiguraSelected->z+t*(8.0-FiguraSelected->z));	
					}
					FiguraSelected = NULL;
				}
			}
		}
	}
}

Figura getFigura(Lista l,int name){
	Nodo ultimo;
	Figura f;
	if(name==-1) return NULL;
	for(ultimo=l->inicio; ultimo != NULL; ultimo=ultimo->next){
		f = (Figura)(ultimo->contenido);
		if((f)->name==name) return f;
	}	
}

int stopPicking() {
	int hits = 0;	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	selectionMode = 0;
	if (hits != 0)
		return processHits(hits,buffer);
	else{
		return -1;
	}	
}

int processHits (GLint hits, GLuint buffer[]){
	unsigned int i, j;
	GLuint names, *ptr, minZ, *ptrNames, numberOfNames;
	ptr = (GLuint *) buffer;
	minZ = 0xffffffff;
	for(i=0; i<hits; i++){
		names = *ptr;
		ptr++;
		if (*ptr < minZ) {
			numberOfNames = names;
			minZ = *ptr;
			ptrNames = ptr+2;
		}	  
		ptr += names+2;
	}
	ptr = ptrNames;
	for (j = 0; j < numberOfNames; j++,ptr++) {
		return (int)(*ptr);
	}
	return -1;
}


static void motion(int x, int y) {
	if(FiguraSelected != NULL){
		cursorX = x;
		cursorY = y;
	}
	if (moving) {
		angle = angle + (x - startx);
		angle2 = angle2 + (y - starty);
		startx = x;
		starty = y;
	}
	if (lightMoving) {
		lightAngle += (x - lightStartX)/40.0;
		lightHeight += (lightStartY - y)/20.0;
		lightStartX = x;
		lightStartY = y;
	}
}

static void menuClick(int value) {
	
	switch (value) {
		case -1:
			if(FiguraSelected==NULL){return;}
			FiguraSelected->textura=-1;
		break;
		case 0:
			if(FiguraSelected==NULL){texturaPiso=0;return;}
			FiguraSelected->textura=0;
		break;
		case M_TEXTURE_01:
			if(FiguraSelected==NULL){texturaPiso=1;return;}
			FiguraSelected->textura=1;
		break;
		case M_TEXTURE_02:
			if(FiguraSelected==NULL){texturaPiso=2;return;}
			FiguraSelected->textura=2;
		break;
		case M_TEXTURE_03:
			if(FiguraSelected==NULL){texturaPiso=3;return;}
			FiguraSelected->textura=3;
		break;
		case M_TEXTURE_04:
			if(FiguraSelected==NULL){texturaPiso=4;return;}
			FiguraSelected->textura=4;
		break;
		case M_TEXTURE_05:
			if(FiguraSelected==NULL){texturaPiso=5;return;}
			FiguraSelected->textura=5;
		break;
	}
	glutSetMenu(Menu);
	glutPostRedisplay();
}
static void menuClick0(int value) {
	switch (value) {
		case M_TOOGLE_SELF_SPIN:
			if(FiguraSelected != NULL){
				figuraToogleSpin(FiguraSelected);
				FiguraSelected = NULL;
			}
		break;
		case M_ORBIT:
			if(FiguraSelected != NULL){
				figuraOrbit(FiguraSelected);
				FiguraSelected = NULL;
			}
		break;
	}
	glutPostRedisplay();
}


void Inicio(int argc, char **argv){
	Lista cascaras,arcos;
	Figura cubo;
	int i;
	cursorX = 0.0f;
	cursorY = 0.0f;
	FiguraSelected = NULL;
	size = 512;
	buffer = (GLuint *)(malloc(size));
	nombreActual = 1;
	selectionMode = 0;
	specialMode = 0;
	actualTexture=-1;
	Figuras = crearLista();
		glLoadName( 0 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(ESFERA),5.0f,1.5f,8.0f)));
		glLoadName( 1 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(CUBO),-10.0f,4.5f,5.0f)));
		glLoadName( 2 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TEAPOT),-10.0f,1.5f,-6.0f)));
		glLoadName( 3 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TOROIDE),1.0f,2.5f,10.0f)));
		glLoadName( 4 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(ESFERA),-12.3f,1.5f,-5.3f)));
		glLoadName( 5 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(CUBO),-10.0f,7.5f,2.2f)));
		glLoadName( 6 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TEAPOT),3.0f,1.5f,4.13f)));
		glLoadName( 7 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TOROIDE),3.1f,2.5f,-11.1f)));
		glLoadName( 8 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(ESFERA),-1.22f,1.5f,-1.34f)));
		glLoadName( 9 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(CUBO),7.0f,1.5f,12.2f)));
		glLoadName( 10 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TEAPOT),10.0f,1.5f,-1.13f)));
		glLoadName( 11 );
		agregarNodo(Figuras,crearNodo(moverFiguraXYZ(crearFigura(TOROIDE),3.1f,3.5f,-1.1f)));
	
	/*
	cluster= clusterNew();
	cluster->ArcosFiguras=IniciarClustering(Figuras,1);
	cascaras=Agrupar(cluster->ArcosFiguras,1);
	agregarNodo(cluster->niveles,crearNodo(cascaras));
	arcos=IniciarClustering(cascaras,0);
	cascaras=Agrupar(arcos,0);
	agregarNodo(cluster->niveles,crearNodo(cascaras));
	arcos=IniciarClustering(cascaras,0);
	cascaras=Agrupar(arcos,0);
	agregarNodo(cluster->niveles,crearNodo(cascaras));
	*/
	
	glutInit(&argc, argv);
	glutInitWindowSize(700, 500);
	for (i=1; i<argc; i++) {
		if (!strcmp("-linear", argv[i])) {
			linearFiltering = 1;
		} else if (!strcmp("-mipmap", argv[i])) {
			useMipmaps = 1;
		} else if (!strcmp("-ext", argv[i])) {
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
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);	
	subMenu = glutCreateMenu(menuClick);
	glutAddMenuEntry("Sin textura",-1);
	glutAddMenuEntry("ChessBoard",0);
	glutAddMenuEntry("Textura 1", M_TEXTURE_01);
	glutAddMenuEntry("Textura 2", M_TEXTURE_02);
	glutAddMenuEntry("Textura 3", M_TEXTURE_03);
	glutAddMenuEntry("Textura 4", M_TEXTURE_04);
	glutAddMenuEntry("Textura 5", M_TEXTURE_05);
	Menu=glutCreateMenu(menuClick0);
	glutAddSubMenu("Aplicar textura", subMenu);
	glutAddMenuEntry("Encender/Apagar auto-spin", M_TOOGLE_SELF_SPIN);
	glutAddMenuEntry("Iniciar/Detener Orbita", M_ORBIT);

	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glMatrixMode(GL_PROJECTION);
	//gluPerspective(40.0, 1.0, 20.0, 100.0);
	//glMatrixMode(GL_MODELVIEW);
	//gluLookAt(0.0, 8.0, 70.0, 0.0, 8.0, 0.0, 0.0, 1.0, 0.);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	a=VectorNew();
	b=VectorNew();
	c=VectorNew();
	e=VectorNew();
	calibrar();
	inicializar();

	


}
float* VectorNew(){
	return (float*)malloc(sizeof(float)*3);
}
void VectorSet(float *v,float x,float y,float z){
	v[0]=x;
	v[1]=y;
	v[2]=z;
}
void calibrar(){
	VectorSet(a,0,0,0);
	VectorSet(b,30,0,0);
	VectorSet(c,0,17.5,0);
	VectorSet(e,15,8.75,50);
}
void presentation() {
	int cont = 0;
	printf("\n");
	for(cont=0;cont<80;cont++) printf("*");
	printf("\t\t\t\t   Proyecto Final\n");
	for(cont=0;cont<80;cont++) printf("*");
	printf("\n\n\n\t\tIntegrantes:\n\n");
	printf("\t\t     * David Barrera\n");
	printf("\t\t     * Liliana Ramos\n");
	printf("\t\t     * Denny Schuldt\n");
}

void instrucciones(){
	int cont = 0;
	printf("\n\n\n\n");
	for(cont=0;cont<80;cont++) printf("*");
	printf("\t\t\t\t   Proyecto Final\n");
	for(cont=0;cont<80;cont++) printf("*");
	printf("\n\n 1) Puede mover la escena desde cualquier lugar tan solo arrastrandola con el mouse");
	printf("\n 2) Las luces se encienden o se apagan con las teclas [1,2,3,4,5,6]");
	printf("\n 3) Presione la tecla space, para luego poder seleccionar un objeto");
	printf("\n 4) Para mover un objeto a traves del tablero, hagalo presionando el clic derecho");
	printf("\n 5) Zoom in, con la tecla 'i'");
	printf("\n 6) Zoom out, con la tecla 'o'");
	printf("\n 7) Para aplicar texturas, spining o poner el objeto en orbita alrededor del asta, presione el click derecho. (Requiere haber seleccionado el objeto antes)");
	printf("\n 8) Para salir del modo seleccionar, presione space nuevamente\n\n\n\n");
}

void inicializar(void){
    glClearColor (0.5, 0.5, 0.5, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	image2 = loadTexture("texture_02.bmp");
	if(image2 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	texture_01 = loadTexture("texture_01.bmp");
	if(texture_01 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	texture_02 = loadTexture("texture_02.bmp");
	if(texture_02 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	texture_03 = loadTexture("texture_03.bmp");
	if(texture_03 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	texture_04 = loadTexture("texture_04.bmp");
	if(texture_04 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	texture_05 = loadTexture("texture_05.bmp");
	if(texture_05 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }


	makeCheckImage();
	image1 = loadTexture("chessboard.bmp");
	if(image1 == NULL){
		printf("Image was not returned from loadTexture\n");
		getch();
		exit(0);
    }
	makeCheckImage();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(6, texture);


	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,&checkImage[0][0][0]);	
	//glShadeModel(GL_FLAT);
	*/

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

}

void makeCheckImage(void){
    int i, j, c;
	for (i = 0; i < CHECK_IMAGE_WIDTH; i++) {
		for (j = 0; j < CHECK_IMAGE_HEIGHT; j++) {
			c = ((((i&0x8)==0)^((j&0x8)==0)))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
        }
	}
}
void projection(const float *pa,
const float *pb,
const float *pc,
const float *pe, float n, float f)
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
M[0] = vr[0]; M[4] = vr[1]; M[ 8] = vr[2];
M[1] = vu[0]; M[5] = vu[1]; M[ 9] = vu[2];
M[2] = vn[0]; M[6] = vn[1]; M[10] = vn[2];
M[15] = 1.0f;
glMultMatrixf(M);
// Move the apex of the frustum to the origin.
glTranslatef(-pe[0], -pe[1], -pe[2]);
glMatrixMode(GL_MODELVIEW);

}
void subtract(float* dst,const float* a,const float* b){
	dst[0]=a[0]-b[0];
	dst[1]=a[1]-b[1];
	dst[2]=a[2]-b[2];
}
void normalize(float* v){
	v[0]/=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[1]/=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[2]/=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}
void cross_product(float* dst,const float* a,const float* b){
	dst[0]=a[1]*b[2]-a[2]*b[1];
	dst[1]=-(a[0]*b[2]-a[2]*b[0]);
	dst[2]=a[0]*b[1]-a[1]*b[0];
}
float dot_product(const float* a,const float* b){
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];	
}
int ImageLoad(char *filename, Image *image) {
    FILE *file;
	unsigned long size;
	unsigned long i;
	unsigned short int planes;
	unsigned short int bpp;
	char temp;
    if ((file = fopen(filename, "rb"))==NULL){
		printf("File Not Found : %s\n",filename);
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
    for (i=0;i<size;i+=3) {
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
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
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] =
  {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
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
  gluQuadricTexture(quadObj,GL_TRUE);
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
  int i, j,n;
  GLfloat theta, phi, theta1;
  GLfloat cosTheta, sinTheta;
  GLfloat cosTheta1, sinTheta1;
  GLfloat ringDelta, sideDelta;

  ringDelta = 2.0 * M_PI / rings;
  sideDelta = 2.0 * M_PI / nsides;
  n=0;
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
									glVertex3f(cosTheta * dist, -sinTheta * dist,  r * sinPhi);
	  n=!n;
    }
    glEnd();
    theta = theta1;
    cosTheta = cosTheta1;
    sinTheta = sinTheta1;
  }
}


