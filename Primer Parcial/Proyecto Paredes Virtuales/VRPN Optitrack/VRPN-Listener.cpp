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

//== Callback prototype ==--

void VRPN_CALLBACK handle_pos (void *, const vrpn_TRACKERCB t);

//== Main entry point ==--

//== Position/Orientation Callback ==--

void	VRPN_CALLBACK handle_pos (void *, const vrpn_TRACKERCB t)
{
    //printf("Pared Position:(%.4f,%.4f,%.4f) Orientation:(%.2f,%.2f,%.2f,%.2f)\n",t.pos[0], t.pos[1], t.pos[2],t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
	xPared = t.pos[0]*10.0f;
	yPared = t.pos[1]*10.0f;
	zPared = t.pos[2]*10.0f;
	angleQ = 2.0 * acos(t.quat[3]) * (180.0f / PI);
	qx   =   t.quat[0];
    qy   =   t.quat[1];
    qz   =   t.quat[2];
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
				printf("saved Wall %d\n \t\t%.4f \t%.4f \t%.4f\n\t\t%.4f \t%.4f \t%.4f\n", savedWalls, walls[savedWalls][0], walls[savedWalls][1], walls[savedWalls][2], walls[savedWalls][3], walls[savedWalls][4], walls[savedWalls][5]);
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

DWORD WINAPI hacerTracking(LPVOID lpParam)
{
	 
		vrpn_Connection *connection;

		char connectionName[128];
		int  port = 3883;

		sprintf(connectionName,"localhost:%d", port);
        
		connection = vrpn_get_connection_by_name(connectionName);
    
		vrpn_Tracker_Remote *tracker = new vrpn_Tracker_Remote("Pared", connection);

  		tracker->register_change_handler(NULL, handle_pos);	
		while(!kbhit())
		{
			tracker->mainloop();
			connection->mainloop();
			Sleep(5);
		}
     return 0;
}


//**PRINCIPAL FUNCTION
int main(int argc, char **argv){
	int i;
	DWORD dwThreadId, dwThrdParam = 1;
	savedWalls=0;
	HANDLE hiloParaTracking;
   // GLut Initialization
   glutInit(&argc,argv);
   
   //Window Position
   glutInitWindowPosition(100, 100);
   
   //Window Size
   glutInitWindowSize(ancho = 800, alto = 600);
   
   //Shows the window: GLUT_SINGLE = single buffer de color; GLUT:RGB = modelo RGB de color
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
   
   //Title for the window
   glutCreateWindow("Proyecto-RealidadVirtual");    

   //Textures Loading
   initRendering();
   //Function that generates the objects
   glutDisplayFunc(renderScene);
  
   //Changes in the size of the window by the user
   glutReshapeFunc(changeSize);
   
   //
   glutIdleFunc(renderScene);
   
   //Function for the user entries
   glutSpecialFunc(processSpecialKeys);
   glutKeyboardFunc(processNormalKeys);

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