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
#include "textures.h"

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

#define MAX_NUMBER_OF_PARTICLE_SYSTEMS 5

#define FLOOR_SIZE 20.0f

///////////////////////////////////////////////

LinkedListParticleSystem** particle_systems;

int selected_index = 0;
LinkedListParticleSystem* selected_particle_system;

Camera* camera;
int disco_floor = 0;

void display_info(){
  int n = 0;
  float f = 0;
  int i;
  for(i = 0; i < MAX_NUMBER_OF_PARTICLE_SYSTEMS; i++){
  	n += particle_systems[i]->n;
  	f += particle_systems[i]->particle_spawn_frequency;
  }

  print_n("FPS", 					(int)fps, 0.005, 0.97);  
  print_n("Number of particles", 	n, 0.005, 0.95);
  print_n("Spawn frequency", 		f, 0.005, 0.93);
}

void drawFloor(double r, double g, double b){
	float w = FLOOR_SIZE;
	glPushMatrix();
		glColor3f(r, g, b);   
		glBegin(GL_QUADS);                      	// Draw A Quad
		    glVertex3f(-w, 0.0f, w);                // Top Left
		    glVertex3f( w, 0.0f, w);                // Top Right
		    glVertex3f( w, 0.0f, -w);               // Bottom Right
			glVertex3f(-w, 0.0f, -w);               // Bottom Left
		glEnd(); 
	glPopMatrix();
}

void animate(){ 

	count_fps();

	view(camera);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	if(disco_floor){
		rgb curr_rgb = hsv2rgb(*selected_particle_system->current_hsv);
		drawFloor(curr_rgb.r, curr_rgb.g, curr_rgb.b);
	}
	else{
		drawFloor(0.5, 0.5, 0.5);
	}
	
	int i;
	for(i = 0; i < MAX_NUMBER_OF_PARTICLE_SYSTEMS; i++){
		LinkedListParticleSystem* particle_system = particle_systems[i];
		if(!particle_system){
			continue;
		}

		update_particle_system(particle_system);
		create_particles(particle_system);
  	
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

		      p->x = particle_system->x + cos(p->orbit * DEG_TO_RAD) * p->orbital_radius;
		      p->z = particle_system->z + sin(p->orbit * DEG_TO_RAD) * p->orbital_radius;      

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
		}

	display_info();

    if(axisEnabled){
    	glCallList(axisList);
    }

  glutSwapBuffers();
}



///////////////////////////////////////////////

void special_keypress(int key, int x, int y) {
	Particle* p = selected_particle_system->tail;

	switch (key) {
		case GLUT_KEY_UP:
	  		selected_particle_system->initial_vy *= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_DOWN:
	  		selected_particle_system->initial_vy /= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_RIGHT:
	 		selected_particle_system->initial_orbit_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

			while(p){
				p->orbital_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
	 	case GLUT_KEY_LEFT:
	 		selected_particle_system->initial_orbit_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

	 		while(p){
				p->orbital_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
 	}

}

//returns [x,y]
int* init_location(int ps_index){
	int xy[2];

	switch(ps_index){
		case 0: xy[0]=0;
				xy[1]=0;
				break;
		case 1: xy[0]=-FLOOR_SIZE/2;
				xy[1]=0;
				break;
		case 2: xy[0]=FLOOR_SIZE/2;
				xy[1]=0;
				break;
		case 3: xy[0]=0;
				xy[1]=-FLOOR_SIZE/2;
				break;
		case 4: xy[0]=0;
				xy[1]=FLOOR_SIZE/2;
				break;
		default:break;
	}

	return xy;
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key){
  	case '1':	selected_index = 0;
  				break;
   	case '2':	selected_index = 1;
  				break;
  	case '3':	selected_index = 2;
  				break; 
  	case '4':	selected_index = 3;
  				break; 			
   	case '5':	selected_index = 4;
  				break; 		
  	case '+':	selected_particle_system->particle_spawn_frequency *= 1.2;
  				return;
  	case '-':	selected_particle_system->particle_spawn_frequency /= 1.2;
  				return;
  	case 'w':	camera->eyeY += 1;
  				return;
  	case 's':	camera->eyeY -= 1;
  				return;
  	case 'd':	camera->orbit -= 5; 
  				update_camera(camera);
  				return;
  	case 'a':	camera->orbit += 5;
  	  			update_camera(camera);
  				return;
  	case 'c':	selected_particle_system->hsv_increment_amount *= 1.2;
  				return;
  	case ' ': 	if(selected_particle_system){
  					reset(selected_particle_system, selected_particle_system->x, selected_particle_system->z);
  				}
  				else{
  					int* xy = init_location(selected_index);
  					particle_systems[selected_index] = init_particle_system(xy[0], xy[1]);
  					selected_particle_system = particle_systems[selected_index];
  				}
  				//reset_camera(camera);
  				disco_floor = 0;
  				return;
  	case 'f':	disco_floor = !disco_floor;
  				return;
  	case 'r':	selected_particle_system->renderOption = (selected_particle_system->renderOption+1) % 3;
  				return;
  	case 27:	exit(0); //esc
  				return;
  	case 127:	killall(selected_particle_system);//del
  				free(selected_particle_system);
  				return;
  	default:	return;
  }


  selected_particle_system = particle_systems[selected_index];
  lookat(camera, selected_particle_system);
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
  glutCreateWindow("Particle DISCO");
  //glutDisplayFunc(display);
  glutIdleFunc(animate);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special_keypress);
  //glutMouseFunc(mouse);
  //glutPassiveMotionFunc (mouse_motion);
  glutReshapeFunc(reshape);

  glPointSize(POINT_SIZE);
  glLineWidth(3); 

  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  camera = init_camera();

  particle_systems = (LinkedListParticleSystem**) malloc(sizeof(LinkedListParticleSystem*) * MAX_NUMBER_OF_PARTICLE_SYSTEMS);

  int i;
  for(i = 0; i < MAX_NUMBER_OF_PARTICLE_SYSTEMS; i++){
  	int* xy = init_location(i);
  	particle_systems[i] = init_particle_system(xy[0],xy[1]);
  }

  selected_particle_system = particle_systems[0];

  //basketball_texture = LoadTexture( "basketball.bmp" );

  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
  return 0;
}
