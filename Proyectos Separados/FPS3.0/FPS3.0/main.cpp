#include <stdlib.h>
#include <math.h>
#include <gl/glew.h> // Include the GLEW header file  
#include <gl/glut.h> // Include the GLUT header file  

//angle of rotation
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle = 0.0;

float lastx, lasty;

bool disparo = false;
bool movingUp = false; // Whether or not we are moving up or down
float yLocation = 0.0f; // Keep track of our position on the y axis.
float yRotationAngle = 0.0f; // The angle of rotation for our object


//positions of the cubes
float positionz[5];
float positionx[5];

//Material
//GLfloat diffuseMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };

void cubepositions(void) { //set the positions of the cubes

	for (int i = 0; i<10; i++)
	{
		positionz[i] = rand() % 5 + 5;
		positionx[i] = rand() % 5 + 5;
	}
}

//draw the cube
void cube(void) {
	glColor3f(0.1f, 0.1f, 1.1f);
	for (int i = 0; i < 20; i++)
	{
		glPushMatrix();
		glTranslated(-positionx[i + 1] * 10, 0, -positionz[i + 1] * 10); //translate the cube
		glutSolidCube(2); //draw the cube
		glPopMatrix();
	}
}

//Dibujar el piso
void piso(void){
	glColor3f(0.1f, 1.1f, 0.1f);
	glBegin(GL_LINES);
	for (GLfloat i = -100; i <= 100; i += 0.25) {
		glVertex3f(i, -1, 100); glVertex3f(i, -1, -100);
		glVertex3f(100, -1, i); glVertex3f(-100, -1, i);
	}
}

void disparar() {
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -5.0f); // Push eveything 5 units back into the scene, otherwise we won't see the primitive
	glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis
	glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis
	glutWireCube(2.0f); // Render the primitive
	glFlush(); // Flush the OpenGL buffers to the window
	if (movingUp) // If we are moving up
		yLocation -= 0.005f; // Move up along our yLocation
	else// Otherwise
		yLocation += 0.005f; // Move down along our yLocation

	if (yLocation < -3.0f) // If we have gone up too far
		movingUp = false; // Reverse our direction so we are moving down
	else if (yLocation > 3.0f) // Else if we have gone down too far
		movingUp = true; // Reverse our direction so we are moving up

	yRotationAngle += 0.005f; // Increment our rotation value

	if (yRotationAngle > 360.0f) // If we have rotated beyond 360 degrees (a full rotation)
		yRotationAngle -= 360.0f; // Subtract 360 degrees off of our rotation
	glPopMatrix();
}

void init(void) {
	cubepositions();
}

void enable(void) {
	/*
	glEnable(GL_DEPTH_TEST); //enable the depth testing
	glEnable(GL_LIGHTING); //enable the lighting
	glEnable(GL_LIGHT0); //enable LIGHT0, our Diffuse Light
	glShadeModel(GL_SMOOTH); //set the shader to smooth shader
	*/
	///*
	static float lmodel_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	static float lmodel_twoside[] = { GL_FALSE };
	static float lmodel_local[] = { GL_FALSE };
	static float light0_ambient[] = { 0, 0, 0, 1.0 };
	static float light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	static float light0_position[] = { 1.0, 0.5, 1, 0 };
	static float light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	glClearColor(0.5, 0.5, 0.5, 0.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);



	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	//*/

}

void camera(void) {
	glRotatef(xrot, 1.0, 0.0, 0.0);  //rotate our camera on teh x - axis(left and right)
	glRotatef(yrot, 0.0, 1.0, 0.0);  //rotate our camera on the y - axis(up and down)
	glTranslated(-xpos, -ypos, -zpos); //translate the screen to the position of our camera
}

void display(void) {
	glutSetCursor(GLUT_CURSOR_NONE);

	glClearColor(0.0, 0.0, 0.0, 1.0); //clear the screen to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color buffer and the depth buffer
	glLoadIdentity();
	camera();
	enable();
	if (disparo == true){
		disparar();
	}
	cube(); //Dibujar los cubos
	piso(); //Dibujar el piso

	glEnd();
	glutSwapBuffers(); //swap the buffers
	angle++; //increase the angle
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
	glMatrixMode(GL_PROJECTION); //set the matrix to projection

	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0
		); //set the perspective (angle of sight, width, height, depth)
	glMatrixMode(GL_MODELVIEW); //set the matrix back to model

}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'q')
	{
		xrot += 1;
		if (xrot >360) xrot -= 360;
	}

	if (key == 'z')
	{
		xrot -= 1;
		if (xrot < -360) xrot += 360;
	}

	if (key == 'w')
	{
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * 3.141592654f);
		xrotrad = (xrot / 180 * 3.141592654f);
		xpos += float(sin(yrotrad));
		zpos -= float(cos(yrotrad));
		//ypos -= float(sin(xrotrad));
	}

	if (key == 's')
	{
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * 3.141592654f);
		xrotrad = (xrot / 180 * 3.141592654f);
		xpos -= float(sin(yrotrad));
		zpos += float(cos(yrotrad));
		//ypos += float(sin(xrotrad));
	}

	if (key == 'd')
	{
		float yrotrad;
		yrotrad = (yrot / 180 * 3.141592654f);
		xpos += float(cos(yrotrad)) * 0.8;
		zpos += float(sin(yrotrad)) * 0.8;
	}

	if (key == 'a')
	{
		float yrotrad;
		yrotrad = (yrot / 180 * 3.141592654f);
		xpos -= float(cos(yrotrad)) * 0.8;
		zpos -= float(sin(yrotrad)) * 0.8;
	}

	if (key == 27)
	{
		exit(0);
	}
	if (key == 32)
	{
		disparo = !disparo;
	}

}

void mouseMovement(int x, int y) {
	int diffx = x - lastx; //check the difference between the  current x and the last x position
	int diffy = y - lasty; //check the difference between the  current y and the last y position
	lastx = x; //set lastx to the current x position
	lasty = y; //set lasty to the current y position
	xrot += (float)diffy; //set the xrot to xrot with the addition of the difference in the y position
	yrot += (float)diffx;    //set the xrot to yrot with the addition of the difference in the x position
	if (xrot >= 90){
		xrot = 89;
	}
	if (xrot <= -90){
		xrot = -89;
	}


}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("A basic OpenGL Window");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(mouseMovement); //check for mouse movement
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}