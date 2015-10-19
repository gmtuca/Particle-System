////////////////////////////////////////////////////////////////
// School of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
//
// Skeleton code for COMP37111 coursework, 2013-14
//
// Authors: Arturs Bekasovs and Toby Howard
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include "rgb_hsv.h"
#include "print.h"
#include "particle.h"
#include "fps.h"

// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 1;

#define DEG_TO_RAD 0.017453293

///////////////////////////////////////////////

LinkedListParticleSystem* particle_system;

typedef struct Camera{
  GLfloat eyeX, eyeY, eyeZ;
} Camera;

Camera* camera;

int current_view = 0;
Particle* current_view_p;

void create_particles(){
  if(particle_system->particle_spawn_frequency < 1){
  	if(particle_system->t % (int)(1/particle_system->particle_spawn_frequency) == 0){
  		create_new_particle(particle_system);
  	}
  }
  else{
  	int i;
  	for(i = 0; i < particle_system->particle_spawn_frequency; i++){
  		create_new_particle(particle_system);
  	}
  }
}

void update_particle_system(){
  particle_system->t++;
  
  particle_system->current_hsv->h += particle_system->hsv_increment_amount;
  if(particle_system->current_hsv->h > 360){particle_system->current_hsv->h = 0;}
}

void display_info(){
  print_n("FPS", 					(int)fps, 0.005, 0.97);
  print_n("Time", 					particle_system->t, 0.005, 0.94);
  print_n("Number of particles", 	particle_system->n, 0.005, 0.91);
  print_n("Spawn frequency", 		particle_system->particle_spawn_frequency, 0.005, 0.88);
}

void animate(){ 

  count_fps();

  update_particle_system();
  create_particles();


  glLoadIdentity();



     if(current_view == 1){

	     gluLookAt(current_view_p->x, current_view_p->y, current_view_p->z,
	          0.0, 0.0, 0.0,
	          0.0, 1.0, 0.0);
     }
     else{
     	 gluLookAt(camera->eyeX, camera->eyeY, camera->eyeZ,
          0.0, 0.0, 0.0,
          0.0, 1.0, 0.0);

     }
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);
  // If enabled, draw coordinate axis
  
  //  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    if(particle_system->renderOption == POINT){
    	glBegin(GL_POINTS);
    }

 //glBegin(GL_LINE_STRIP);
 


	Particle* p = particle_system->tail;

	while(p){

		  if(p->time_lived > particle_system->particle_lifespan){
		  	particle_system->tail = p->prev;
		  	free(p);
		  	p = particle_system->tail;
		  	particle_system->n--;
		  	continue;
		 }

		  p->time_lived++;
	      p->t+=0.0002;
	      p->vy -= 0.1* p->t;
	      

	      if(p->y < 0){
	        if(p->vy < 0){
	          p->vy = -p->vy * 0.7;
	        }
	 
	        p->t = 0.1;
	      }

	      p->y += p->vy * p->t + 0.5 * p->t * p->t;

	      
	      p->orbit += 360.0 / p->total_orbit_time;

	      
	      double a = 1.005 - 0.0000035 * p->time_lived;

	      if(a<1){
	      	a = 1;
	      }

	      p->orbital_radius *= a;

	      p->x = 0.8* (cos(p->orbit * DEG_TO_RAD) * p->orbital_radius);
	      p->z = 0.8* (sin(p->orbit * DEG_TO_RAD) * p->orbital_radius);

	      

	    glPushMatrix();
	      glColor3f(p->r, p->g, p->b);

	      if(particle_system->renderOption == POINT){
	      	glVertex3f(p->x, p->y, p->z);
	      }
	      else{
	      	glTranslated(p->x, p->y, p->z);

	      	if(particle_system->renderOption == SPHERE){
	      		glutSolidSphere(0.1, 10, 10);
	      	}
	      	else if(particle_system->renderOption == TEAPOT){
	      		glutSolidTeapot(0.1);
	      	}
	      }

	    glPopMatrix();

			p = p->prev;
		}

		glEnd();



	display_info();
    if(axisEnabled) glCallList(axisList);

  glutSwapBuffers();
}

///////////////////////////////////////////////

void special_keypress(int key, int x, int y) {
 switch (key) {
	 case GLUT_KEY_UP:
	  particle_system->initial_vy *= 1.2;
	  break;
	 case GLUT_KEY_DOWN:
	  particle_system->initial_vy /= 1.2;
	  break;
 }
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key){
  	case '+':	particle_system->particle_spawn_frequency *= 1.2; printf("+\n");
  				break;
  	case '-':	particle_system->particle_spawn_frequency /= 1.2; printf("-\n");
  				break;
  	case 'v':	current_view_p = particle_system->head;
  				current_view = !current_view;
  				break;
  	case 'c':	particle_system->hsv_increment_amount *= 1.2;
  				break;
  	case ' ': 	reset(particle_system);
  				break;
  	case 'r': 	particle_system->renderOption++;
  				particle_system->renderOption %= 3;
  				break;
  	case 27:	exit(0); //esc
  				break;
  }
}

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////

void makeAxes() {
  // Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}

///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("Particle System");
  //glutDisplayFunc(display);
  glutIdleFunc(animate);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special_keypress);
  //glutMouseFunc(mouse);
  //glutPassiveMotionFunc (mouse_motion);
  glutReshapeFunc(reshape);

  //POINT STUFF TODO
  glEnable( GL_POINT_SPRITE );
  glEnable( GL_POINT_SMOOTH );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glPointSize( 5.0 );
  //POINT STUFF

  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  camera = (Camera*) malloc(sizeof(Camera));
  camera->eyeX = 0.0;
  camera->eyeY = 7.0;
  camera->eyeZ = 10.0;

  particle_system = init_particle_system();

  //double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
  return 0;
}
