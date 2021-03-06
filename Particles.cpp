// Particles.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <iostream>		// I/O
#include <time.h>

#ifdef _WIN32
#include <glut.h> // GLUT
#endif

#ifdef __gnu_linux__ // or __linux__ as well
#include <GL/glut.h> // GLUT
#endif


#include <vector>

#ifndef PI
#define PI 3.14159265
#endif

#include "Vector3f.h"

#include "ParticleSystem.h"



#include "OGL2Utils.cpp" //it is generally bad to include a cpp, I know..



/**********LIGHTS*********/
GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0}; //lol
GLfloat left_light_position[] = {-70,10,-70, 1.0};
GLfloat right_light_position[] = {70,10,70, 1.0};
/**********END LIGHTS*********/

//camera views
float zoom = 30.0f;
float rotx = 30.0;
float roty = -220.0f;

//mouse control
float tx = 0;
float ty = 0;
int lastx= 0;
int lasty= 0;
unsigned char Buttons[3] = {0};

//time indicator
int frames=0;

bool		wireframe=true;
int         windowId;

float cameraRotation;
float cameraTarget;



//std::vector<Particle> parts;
std::vector<Vector3f> forces;


ParticleSystem field;

//---auxiliary methods (setup scene, keyboard handler, mouse handler)---------

void setupScene(){

    std::cout<<"Initializing scene..."<<std::endl;

    std::cout.precision(5);


    glClearColor(0.5,0.5,0.5,1.0);


    //Set up Lighting Stuff
    glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
    glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_COLOR_MATERIAL); glEnable(GL_COLOR_MATERIAL); //in this application we want user-defined colors. no materials


    //forces = std::vector<Vector>(2);
    forces.push_back(Vector3f(-0.01,0.0,0.0));//wind
    forces.push_back(Vector3f(0.0,0.0,0.0));//not
    forces.push_back(Vector3f(0.0,-0.09,0.0));//ok 0.15; gravity force


    //TENSION
    //field = ParticleSystem(100); //100 particles
    //field(100); //100 particles (wouldn't work anyway)
    field.SystemForces().push_back(Vector3f(0.0,-0.09,0.0)); //populate with forces (nb: i shouldn't push back; i should substitute the only null force that is there already)
    field.SystemForces().push_back(forces.at(0));

    for(unsigned int i=0; i<field.getSystemForces().size(); i++){
        std::cout << i << " " << field.getSystemForces().at(i) << std::endl;
    }

}

void exitScene()
{

    std::cout<<"Exiting scene..."<<std::endl;

    // Close window
    glutDestroyWindow(windowId);

    // Free any allocated memory
    //delete field;

    // Exit program
    exit(0);
}

void setViewport(int width, int height) {

    // Work out window ratio, avoid divide-by-zero
    if(height==0) height=1;
    float ratio = float(width)/float(height);

    // Reset projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Fill screen with viewport
    glViewport(0, 0, width, height);

    // Set a 45 degree perspective
    gluPerspective(45, ratio, .1, 1000);

}

void cameraTransform()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,-zoom);
    glTranslatef(tx,ty,0);
    glRotatef(rotx,1,0,0);
    glRotatef(roty-170,0,1,0);

}


void keypress(unsigned char key, int x, int y){

    // Test if user pressed ESCAPE (ascii 27)
    // If so, exit the program
    if(key==27){
        exitScene();
    }

    // 'W' key toggles wireframe mode on & off
    /*if(key == 'w' || key == 'W'){
        wireframe=!wireframe;
        if(wireframe){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }*/

    // Other possible keypresses go here
    if(key == 'a'){   field.addBlackHoleMagnitude( -0.1 ) ;    }
    if(key == 'd'){   field.addBlackHoleMagnitude(  0.1 ) ;    }

    if(key == 's'){   field.addBlackHoleCentreY (-0.1 ) ;    }
    if(key == 'w'){   field.addBlackHoleCentreY ( 0.1 ) ;    }

}

void keyreleased(unsigned char key, int x, int y){
    if(key == 'w'){ //
    }
    if(key == 's'){ //
    }

    if(key == 'a'){ //
    }

    if(key == 'd'){ //
    }
}

void Motion(int x,int y)
{
    int diffx=x-lastx;
    int diffy=y-lasty;
    lastx=x;
    lasty=y;

    if( Buttons[2] )
    {
        zoom -= (float) 0.15f * diffy;
    }
    else if( Buttons[0] )
    {
        rotx += (float) 0.5f * diffy;
        roty += (float) 0.5f * diffx;
    }
    else if( Buttons[1] )
    {
        tx += (float) 0.05f * diffx;
        ty -= (float) 0.05f * diffy;
    }

}


void Mouse(int b,int s,int x,int y)
{
    lastx=x;
    lasty=y;
    switch(b)
    {
    case GLUT_LEFT_BUTTON:
        Buttons[0] = ((GLUT_DOWN==s)?1:0);
        break;
    case GLUT_MIDDLE_BUTTON:
        Buttons[1] = ((GLUT_DOWN==s)?1:0);
        break;
    case GLUT_RIGHT_BUTTON:
        Buttons[2] = ((GLUT_DOWN==s)?1:0);
        break;
    default:
        break;
    }

}


//----Methods  (draw & update)-------------

void renderScene(){
        
    // Clear framebuffer & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //where should the camera look at?
    cameraTransform();

    glDisable(GL_LIGHTING);//disable lighting not to affect the drawings of meta-objects

    drawAxes();

    drawGrid();

    glEnable(GL_LIGHTING);//resume lighting

    //WITH = All the light will NOT move with the camera (light fixed even if i rotate)
    //Because i tell at every frame to fix the light at those 2 points
    glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);

    glColor3f(0.5,0.5,0.5);

    field.getCollisionPlane().draw(Vector3f(0.93, 0.82, 0)); //WTF only certain colors work. JK it's because number must be float.. then why tf there are some 255 every now and then (e.g. axes)


    glPushMatrix();
    glTranslatef(field.getBlackHoleCentre().x,field.getBlackHoleCentre().y,field.getBlackHoleCentre().z);
    glColor3f(0.7,0.5,0);
    glutSolidSphere(0.1f,4,4);
    glPopMatrix();

    field.draw();


    //TODO pause button
    field.update();
	
	glutSwapBuffers();
        
}


void updateScene(int millisec)
{
		frames ++ ;

        /*
        THIS IS A SHITE WINDOWS METHOD
		// Wait until at least 16ms passed since start of last frame
		// Effectively caps framerate at ~60fps
		while(timeGetTime()-lastTickCount<16);
        lastTickCount=timeGetTime();
        */
    
		/*  Debug outputs
		 */
		//if(frames%60==1)
		//{
		
		//}
	

		// ** Do any other updates here ** //

		//	//test on randomness
		//	//float casual = rand();
		//	//bool uno = casual/RAND_MAX > 0.5;
		//	//bool due = (int)casual%2 == 0; // this one test is better
		
		// Draw the next frame
		glutPostRedisplay();

        glutTimerFunc(millisec, updateScene, millisec);
}


//------Entry point----------------------------------------------------------
//

int main(int argc, char *argv[]){
        

	srand ( time(NULL) );


    // Initialise OpenGL
    glutInit(&argc, argv); 

    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(550,50);
    glutInitWindowSize(640,480);
	windowId = glutCreateWindow("IET Physics Lab 1: Particles");
    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
    glutDisplayFunc(renderScene);
    //glutIdleFunc(updateScene); //substituted by below: every Timer expiration, updateScene is called
    glutTimerFunc(3, updateScene, 3);
    glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyreleased);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);


    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();

    // Show window & start update loop
    glutMainLoop();    

    return 0;
    
}
