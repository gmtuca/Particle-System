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
#include "camera.h"
#include "fps.h"

// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 1;

#define DEG_TO_RAD 0.017453293

#define GRAVITY 0.005
#define ELASTICITY 0.9

#define POINT_SIZE 10.0f

#define ARROWS_VY_CHANGE_FACTOR 			1.2f
#define ARROWS_ORBIT_RADIUS_CHANGE_FACTOR 	1.2f

///////////////////////////////////////////////

LinkedListParticleSystem* particle_system;

Camera* camera;

Particle* current_view_p;

void display_info(){
  print_n("FPS", 					(int)fps, 0.005, 0.97);
  print_n("Time", 					particle_system->t, 0.005, 0.94);
  print_n("Number of particles", 	particle_system->n, 0.005, 0.91);
  print_n("Spawn frequency", 		particle_system->particle_spawn_frequency, 0.005, 0.88);
}

void drawFloor(float w){
	glPushMatrix();
		glColor3f(0.5, 0.5, 0.5);   
		glBegin(GL_QUADS);                      	// Draw A Quad
		    glVertex3f(-w, 0.0f, w);                // Top Left
		    glVertex3f( w, 0.0f, w);                // Top Right
		    glVertex3f( w, 0.0f, -w);               // Bottom Right
			glVertex3f(-w, 0.0f, -w);               // Bottom Left
		glEnd(); 
	glPopMatrix();
}

/*void drawCross(Particle* p){
  glPushMatrix();
	  glColor3f(p->r, p->g, p->b);
	  glBegin(GL_LINES);
	    glVertex3f(p->cross_x, 0, -20);
	    glVertex3f(p->cross_x, 0, 20);
	  glEnd();
	  glBegin(GL_LINES);
	    glVertex3f(-20, 0, p->cross_z);
	    glVertex3f(20, 0, p->cross_z);
	  glEnd();
  glPopMatrix();
}
*/
void animate(){ 

	count_fps();

	update_particle_system(particle_system);
	create_particles(particle_system);

	glLoadIdentity();

	view(camera);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

  	drawFloor(20.0f);
  	drawSpawnCircle(particle_system);

  	if(particle_system->renderOption == POINTS){
  		glBegin(GL_POINTS);
  	}

	Particle* p = particle_system->tail;

	while(p){
		//Remove the particle if it is dead
		if(p->t > particle_system->particle_lifespan){
		  	particle_system->tail = p->prev;
		  	particle_system->n--;
		  	free(p);

		  	p = particle_system->tail;
		  	continue;
		 }

		  p->t++;
	      //p->t+=0.0002;
	      //p->vy -= 0.1* p->t;
	      p->vy -= GRAVITY;
	      
	      if(p->y < 0 && p->vy < 0){
	          p->vy *= -ELASTICITY;
	          //drawCircle(p->x, p->z);

	          //add_splash(splashes, init_splash(p->x, p->z, p->r, p->g, p->b));
	        //p->t = 0.1;
	      }

	      glColor3f(p->r, p->g, p->b);

	      glLineWidth(3); 
	      if(particle_system->renderOption == LINES){
		  	glBegin(GL_LINES);
		  	glVertex3f(p->x, p->y, p->z);
		  }
		  else if(particle_system->renderOption == FIRE){
		  	glBegin(GL_LINES);
		  	glVertex3f(0,0, 0);
		  }

	      //p->y += p->vy * p->t + 0.5 * p->t * p->t;
	      p->y += p->vy;
	      
	      p->orbit += 360.0 / particle_system->particle_orbit_time;

	      double a = 1.005 - 0.0000035 * p->t;

	      if(a<1){
	      	a = 1;
	      }

	      p->orbital_radius *= a;

	      p->x = cos(p->orbit * DEG_TO_RAD) * p->orbital_radius;
	      p->z = sin(p->orbit * DEG_TO_RAD) * p->orbital_radius;      

		    //render
		    
	      	glVertex3f(p->x, p->y, p->z);

	      	if(particle_system->renderOption == LINES || particle_system->renderOption == FIRE){
	      		glEnd();
	      	}

			p = p->prev;
		}

		if(particle_system->renderOption == POINTS){
			glEnd();
		}

	display_info();
    if(axisEnabled) glCallList(axisList);

  glutSwapBuffers();
}



///////////////////////////////////////////////

void special_keypress(int key, int x, int y) {
	Particle* p = particle_system->tail;

	switch (key) {
		case GLUT_KEY_UP:
	  		particle_system->initial_vy *= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_DOWN:
	  		particle_system->initial_vy /= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_RIGHT:
	 		particle_system->initial_orbit_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

			while(p){
				p->orbital_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
	 	case GLUT_KEY_LEFT:
	 		particle_system->initial_orbit_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

	 		while(p){
				p->orbital_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
 	}

}

void keyboard(unsigned char key, int x, int y)
{
  switch(key){
  	case '+':	particle_system->particle_spawn_frequency *= 1.2;
  				break;
  	case '-':	particle_system->particle_spawn_frequency /= 1.2;
  				break;
  	case 'w':	camera->eyeY += 1;
  				break;
  	case 's':	camera->eyeY -= 1;
  				break;
  	case 'd':	camera->orbit -= 5; 
  				update_camera(camera);
  				break;
  	case 'a':	camera->orbit += 5;
  	  			update_camera(camera);
  				break;
  	case 'c':	particle_system->hsv_increment_amount *= 1.2;
  				break;
  	case ' ': 	reset(particle_system);
  				reset_camera(camera);
  				break;
  	case 'r':	particle_system->renderOption = (particle_system->renderOption+1) % 3;
  				break;
  	case 27:	exit(0); //esc
  				break;
  }
}

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

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

  glPointSize(POINT_SIZE);

  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  camera = init_camera();
  particle_system = init_particle_system();

  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
  return 0;
}
