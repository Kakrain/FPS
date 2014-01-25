/*
 * FILE:
 *      main.c
 * 		LAST MODIFIED: 2013/09/17
 * --------------------------------------------------------------------------
 * AUTHOR
 *		David Barrera
 *		Liliana Ramos
 *		Denny Schuldt
 * DESCRIPTION:
 * 		This file is the main program
 * DEPENDENCIES:
 *		../ProyectoFinal/proyectoFinal.h
 *		gl.h
 *		glut.h
 *		windows.h
 *		stdlib.h
 *		conio.h
 *		stdio.h
 */

/*Libraries*/

#include "../ProyectoFinal/proyectoFinal.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>	

/***
 *
 * @brief main : This function is the program to be executed 
 ***/

int main(int argc, char **argv) 
{	
	presentation();
	getch();
	instrucciones();
	Inicio(argc, argv);
	glutMainLoop();
	return 0;
}
 