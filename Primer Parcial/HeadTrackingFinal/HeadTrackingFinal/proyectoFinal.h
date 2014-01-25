/*
 * FILE:
 *      proyectoFinal.h
 * 		LAST MODIFIED: 2013/09/17
 * --------------------------------------------------------------------------
 * AUTHOR
 *		David Barrera
 *		Liliana Ramos
 *		Denny Schuldt
 * DESCRIPTION:
 * 		This file contains functions to be used in proyectoFinal.c
 *		It describes the building of a scene to be illuminated and objects to be moved
 * DEPENDENCIES:
 *		gl.h
 *		glut.h
 *		windows.h
 */

/*Definition*/
#ifndef PROYECTOFINAL_H
#define	PROYECTOFINAL_H

#ifdef	__cplusplus
extern "C" {
#endif


/*Libraries*/
#include <windows.h>
#include <GL/gl.h>
#include "../lib/glut.h"



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
	float x,y,z;
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

typedef struct Image Image;

/* definitions*/

typedef struct FiguraStruct* Figura;

typedef struct FiguraStruct* Figura;

typedef enum {LIGHT_CENTER_LEFT, LIGHT_CENTER_RIGHT, LIGHT_NEAR, LIGHT_FAR, LIGHT_LEFT, LIGHT_RIGHT} light_position;

enum { M_TEXTURE, M_TOOGLE_SELF_SPIN, M_TEXTURE_01, M_TEXTURE_02, M_TEXTURE_03, M_TEXTURE_04, M_TEXTURE_05, M_ORBIT};

enum { X, Y, Z, W };

enum { A, B, C, D };


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

void dibujarArco(Arco a, int esFig);

/***
 *
 * @brief dibujarArcoFigura This function draws an arc that joins two figures
 *
 * @Details Use:
 * Arco a;
 * dibujarArcoFigura( a);
 * @param a is data structure arc
 */

void dibujarArcoFigura(Arco a);

/***
 *
 * @brief listaSize This function gets a lists size
 *
 * @Details Use:
 * Lista l;
 * listaSize(l);
 * @param l is a data structure of list type
 */
int listaSize(Lista l);

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
float getDistanciaAlCuadradoCF(Cascara c,Figura f);

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
void calcularCascara(Cascara c,int isFig);

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
int estaDentroLista(Lista list,void* c);
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
int estaDentroListaArcos(Lista list,void* c);
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
Lista Agrupar(Lista arcos, int Figs);

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
Lista IniciarClustering(Lista Figs,int isFigs);
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


float getDistanciaAlCuadrado(void* v1,void* v2,int isFig);

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
Arco unirNodos(void* inicio, void* fin,float peso);

/***
 *
 * @brief clusterNew This function creates a cluster
 *
 * @Details Use: clusterNew();
 */
Cluster clusterNew();

/***
 *
 * @brief cascaraNew This function creates a shell
 *
 * @Details Use: cascaraNew();
 */
Cascara cascaraNew();

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
void cascaraAdd(Cascara c,Figura f);

/***
 *
 * @brief cascaraDraw This function draws a shell
 *
 * @Details Use:
 * Cascara c;
 * cascaraDraw(c);
 * @param c is a data structure of shell type
 */

void cascaraDraw(Cascara c);

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

void doList(Lista l,void (*hacer)( void*));
/***
 *
 * @brief crearFigura This function creates a new figure
 *
 * @Details Use:
 * int tipo;
 * crearFigura(tipo)
 * @param tipo is the type of a figure 
 */


Figura crearFigura(int tipo);

/***
 *
 * @brief crearNodo This function create a new node
 *
 * @Details Use:
 * void* content;
 * crearNodo( content);
 * @param content is a void pointer
 */

Nodo crearNodo(void* content);
/***
 *
 * @brief agregarNodo This function add a node to a list
 *
 * @Details Use:
 * Lista l;
 * Nodo n
 * agregarNodo(l, n)
 * @param l is a list type
 * @param n is a node of a list
 */


void agregarNodo(Lista l,Nodo n);


/***
 *
 * @brief crearFigura This function create a new figure
 *
 * @Details Use:
 * int tipo;
 * crearFigura(tipo);
 * @param tipo is the type of a figure
 */

Figura crearFigura(int tipo);

/***
 *
 * @brief dibujarFigura This function draws a figure
 *
 * @Details Use:
 * Figura f;
 * dibujarFigura(f);
 * @param f is a data structure of list
 */

void dibujarFigura(Figura f);


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

Figura moverFiguraXYZ(Figura f,float x,float y,float z);


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
Figura moverFiguratoXYZ(Figura f,float x,float y,float z);

/***
 *
 * @brief figuraToogleSpin This function turns off and on the spinning process
 *
 * @Details Use:
 * Figura f;
 * figuraToogleSpin(f);
 * @param f is a data structure of list
 */

void figuraToogleSpin(Figura f);

/***
 *
 * @brief figuraOrbit This function turns off and on the orbiting process
 *
 * @Details Use:
 * Figura f;
 * figuraOrbit(f);
 * @param f is a data structure of list
 */
void figuraOrbit(Figura f);

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


void moveToSpace(Figura f,int x, int y);

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

void startPicking(int cursorX, int cursorY);
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
static void drawBox(GLfloat xsize, GLfloat ysize, GLfloat zsize);

/***
 *
 * @brief drawPillar This function draws a box
 *
 * @Details Use:drawPillar();
 */

static void drawPillar(void);

/***
 *
 * @brief drawFloor This function draws a pillar
 *
 * @Details Use:drawPillar();
 */
static void drawFloor(void);

/***
 *
 * @brief redraw This function draws the scene
 *
 * @Details Use: redraw();
 */
static void redraw(void);
/*
 * @brief retrieves the first figure that overlaps figure f, return null if no figure overlaps it
 * @Details Use:
 * Figura f;
 * getFiguraQueColisona(Figura f);
 * @param f is a figure 
 */
Figura getFiguraQueColisona(Figura f);

int stopPicking();
int processHits (GLint hits, GLuint buffer[]);
void processPick (GLint cursorX,GLint cursorY);
void startPicking(int cursorX, int cursorY);
Lista crearLista();
Nodo crearNodo(void* content);
//void doList(Lista l,void (*hacer)(Figura));
Figura getFigura(Lista l,int name);
static void mouse(int button, int state, int x, int y);
void makeCheckImage(void);

/***
 *
 * @brief drawLightArrow(move_x,move_y,move_z,position) : Generic fuction for drawing a light source indicator.
 *
 * @Details Use:
 * drawLightArrow(move_x,move_y,move_z,position)
 *
 ***/
void drawLightArrow(GLfloat move_x, GLfloat move_y, GLfloat move_z, light_position position);

/***
 *
 * @brief presentation() : Shows the names of the authors
 *
 * @Details Use:
 * presentation()
 *
 ***/
void presentation();

/***
 *
 * @brief ImageLoad(filename,image) : Saves a file specified with the "filename" into image.
 *
 * @Details Use:
 * ImageLoad(filename,image)
 *
 ***/
int ImageLoad(char *filename, Image *image);

/***
 *
 * @brief loadTexture() : Returns the loaded texture
 *
 * @Details Use:
 * loadTexture()
 *
 ***/
Image * loadTexture();

/***
 *
 * @brief inicializar() : Initial texture
 *
 * @Details Use:
 * inicializar(void)
 *
 ***/
void inicializar(void);

/***
 *
 * @brief Inicio(argc,argv); : The initial function.
 *
 * @Details Use:
 * Inicio(int argc, char **argv);
 *
 ***/
void Inicio(int argc, char **argv);

/***
 *
 * @brief redrawObjects(void) : This function redraws all the objects that will be picked.
 *
 * @Details Use:
 * redrawObjects();
 *
 ***/
void redrawObjects(void);

/***
 *
 * @brief instrucciones(void) : This function shows the instructions to follow, in order to manipulate the scene.
 *
 * @Details Use:
 * instrucciones();
 *
 ***/
void instrucciones(void);

#ifdef	__cplusplus
}
#endif

#endif	