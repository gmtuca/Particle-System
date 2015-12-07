////////////////////////////////////////////////////////////////
// School of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
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
#include "ex11-bitmap.c"

#define DEG_TO_RAD 0.017453293

#define ELASTICITY 0.9


#define POINT_SIZE 10.0f

#define ARROWS_VY_CHANGE_FACTOR 			1.2f
#define ARROWS_ORBIT_RADIUS_CHANGE_FACTOR 	1.2f

#define MAX_NUMBER_OF_PARTICLE_SYSTEMS 5

#define FLOOR_SIZE 30.0f
#define AXIS_SIZE  200

//////////////////////// GLOBALS //////////////////////////

GLuint axisList;

BITMAPINFO *TobyTexInfo; // Texture bitmap information
GLubyte    *TobyTexBits; // Texture bitmap pixel bits
int 		TobytexName;

BITMAPINFO *SteveTexInfo; // Texture bitmap information
GLubyte    *SteveTexBits; // Texture bitmap pixel bits
int 		StevetexName;

LinkedListParticleSystem** particle_systems;
Camera* camera;

int disco_floor = 0;
int selected_index = 0;

///////////////////////////////////////////////////////////

void display_info(){
  int n = 0;
  float f = 0;
  int i;
  for(i = 0; i < MAX_NUMBER_OF_PARTICLE_SYSTEMS; i++){
  	if(particle_systems[i]){
  		n += particle_systems[i]->n;
  		f += particle_systems[i]->particle_spawn_frequency;
  	}
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

void drawDiscoFloor(){
	if(disco_floor && particle_systems[selected_index]){
		rgb curr_rgb = hsv2rgb(*particle_systems[selected_index]->current_hsv);
		drawFloor(curr_rgb.r, curr_rgb.g, curr_rgb.b);
	}
	else{
		drawFloor(0.5, 0.5, 0.5);
	}
}

void maybeEnableTexture(LinkedListParticleSystem* ps){
  	if(ps->renderOption == TOBY){
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TobyTexInfo->bmiHeader.biWidth,
	                TobyTexInfo->bmiHeader.biHeight, 0, GL_BGR,
	                GL_UNSIGNED_BYTE, TobyTexBits);
	   glEnable(GL_TEXTURE_2D);
	}
	else if(ps->renderOption == STEVE){
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SteveTexInfo->bmiHeader.biWidth,
	                SteveTexInfo->bmiHeader.biHeight, 0, GL_BGR,
	                GL_UNSIGNED_BYTE, SteveTexBits);
	   glEnable(GL_TEXTURE_2D); 					
	}
}

void maybeDisableTexture(LinkedListParticleSystem* ps){
	if(ps->renderOption >= TOBY){
		glDisable(GL_TEXTURE_2D);
	}
}

void animate(){
	count_fps();
	view(camera);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	drawDiscoFloor();
   	glCallList(axisList);
 	
	int i;
	for(i = 0; i < MAX_NUMBER_OF_PARTICLE_SYSTEMS; i++){
		LinkedListParticleSystem* ps = particle_systems[i];
		if(!ps){
			continue;
		}

		update_particle_system(ps);
		create_particles(ps);
  	
	  	drawSpawnCircle(ps);

	  	maybeEnableTexture(ps);

	  	if(ps->renderOption == POINTS){
	  		glBegin(GL_POINTS);
	  	}

		Particle* p = ps->tail;

		//Loop through all particles of this system
		while(p){
			//Remove the particle if it is dead
			if(p->t > ps->particle_lifespan){
			  	ps->tail = p->prev;
			  	ps->n--;
			  	free(p);

			  	p = ps->tail;
			  	continue;
			 }

			//Update properties of the particle
			p->t++;

		    p->vy -= ps->gravity;
		        
		    if(p->y < 0 && p->vy < 0){
		        p->vy *= -ELASTICITY;
		    }

		    glColor3f(p->r, p->g, p->b);

		    if(ps->renderOption == LINES){
				glBegin(GL_LINES);
			  	glVertex3f(p->x, p->y, p->z);
			}
			else if(ps->renderOption == FIRE){
				glBegin(GL_LINES);
			  	glVertex3f(ps->x,0, ps->z);
			}
			else if(ps->renderOption == TOBY || ps->renderOption == STEVE){
			   glBegin(GL_QUAD_STRIP);
			}

		    p->y += p->vy;
		      
		    p->orbit += 360.0 / ps->particle_orbit_time;

		    double a = 1.005 - 0.0000035 * p->t;

		    if(a<1){
		    	a = 1;
		    }

		    p->orbital_radius *= a;

		    p->x = ps->x + cos(p->orbit * DEG_TO_RAD) * p->orbital_radius;
		    p->z = ps->z + sin(p->orbit * DEG_TO_RAD) * p->orbital_radius;      

		    if(ps->renderOption == TOBY || ps->renderOption == STEVE){
			   glTexCoord2f(1.0, 1.0); glVertex3f(p->x-1, p->y+1, p->z);
			   glTexCoord2f(1.0, 0.0); glVertex3f(p->x-1, p->y-1, p->z);
			   glTexCoord2f(0.0, 1.0); glVertex3f(p->x+1, p->y+1, p->z);
			   glTexCoord2f(0.0, 0.0); glVertex3f(p->x+1, p->y-1, p->z);
		    }
		    else{
		    	//render
				glVertex3f(p->x, p->y, p->z);
		    }

		    if(ps->renderOption > POINTS){
		    	glEnd();
		    }

			p = p->prev;
		}

		if(ps->renderOption == POINTS){
			glEnd();
		}

		glPopMatrix();

		maybeDisableTexture(ps);
		
	}

	display_info();

  glutSwapBuffers();
}

///////////////////////////////////////////////

void special_keypress(int key, int x, int y) {
	if(!particle_systems[selected_index]){
		return;	
	}

	Particle* p = particle_systems[selected_index]->tail;

	switch (key) {
		case GLUT_KEY_UP:
	  		particle_systems[selected_index]->initial_vy *= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_DOWN:
	  		particle_systems[selected_index]->initial_vy /= ARROWS_VY_CHANGE_FACTOR;
	  		break;
	 	case GLUT_KEY_RIGHT:
	 		particle_systems[selected_index]->initial_orbit_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

			while(p){
				p->orbital_radius *= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
	 	case GLUT_KEY_LEFT:
	 		particle_systems[selected_index]->initial_orbit_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;

	 		while(p){
				p->orbital_radius /= ARROWS_ORBIT_RADIUS_CHANGE_FACTOR;
			  	p = p->prev;
			}
	 		break;
 	}

}

//returns [x,y]
int* init_location(int ps_index){
	int* xy = (int*)malloc(sizeof(int)*2);

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
		default:xy[0]=0;
				xy[1]=0;
				break;
	}

	return xy;
}

void keyboard(unsigned char key, int x, int y)
{

  if(key >= '1' && key <= '9'){
  	int num_clicked = key - '0';

  	if(num_clicked > MAX_NUMBER_OF_PARTICLE_SYSTEMS){
  		return;
  	}

  	selected_index = num_clicked-1;

	if(!particle_systems[selected_index]){
	  int* xy = init_location(selected_index);
	  particle_systems[selected_index] = init_particle_system(xy[0], xy[1]);
	}

	lookat(camera, particle_systems[selected_index]);
	return;
  }

  switch(key){
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
  	case 'f':	disco_floor = !disco_floor;
  				return;
  	case 27:	exit(0); //esc
  				return;
  	default:	break;
  }

  if(key == ' '){
  	disco_floor = 0;
  	if(particle_systems[selected_index]){
      reset(particle_systems[selected_index], particle_systems[selected_index]->x, particle_systems[selected_index]->z);
  	}
  	else{
	  int* xy = init_location(selected_index);
	  particle_systems[selected_index] = init_particle_system(xy[0], xy[1]);
  	}
  	return;
  }

  if(!particle_systems[selected_index]){
  	return;
  }

  switch(key){
  	case '+':	particle_systems[selected_index]->particle_spawn_frequency *= 1.2;
  				break;
  	case '-':	particle_systems[selected_index]->particle_spawn_frequency /= 1.2;
  				break;
  	case 't':	if(particle_systems[selected_index]->gravity > 0.0003){
  					particle_systems[selected_index]->gravity /= 1.05;
  				}
  				return;
  	case 'g':	if(particle_systems[selected_index]->gravity < 0.03){
  					particle_systems[selected_index]->gravity *= 1.05;
  				}
  				return;
  	case 'c':	particle_systems[selected_index]->hsv_increment_amount *= 1.2;
  				break;
  	case 'f':	disco_floor = !disco_floor;
  				break;
  	case 'r':	particle_systems[selected_index]->renderOption = (particle_systems[selected_index]->renderOption+1) % 5;
  				break;
  	case 127:	killall(particle_systems[selected_index]);//del
  				free(particle_systems[selected_index]);
  				particle_systems[selected_index] = NULL;
  				break;
  	default:	break;
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
  glutCreateWindow("Particle DISCO");
  glutIdleFunc(animate);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special_keypress);
  glutReshapeFunc(reshape);

  glPointSize(POINT_SIZE);
  glLineWidth(3); 

  makeAxes();
}

void init_bitmaps(){
   TobyTexBits = LoadDIBitmap("bmp/toby.bmp", &TobyTexInfo);
   glGenTextures(1, &TobytexName);
   glBindTexture(GL_TEXTURE_2D, TobytexName);

   SteveTexBits = LoadDIBitmap("bmp/steve.bmp", &SteveTexInfo);
   glGenTextures(1, &StevetexName);
   glBindTexture(GL_TEXTURE_2D, StevetexName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  camera = init_camera();
  particle_systems = (LinkedListParticleSystem**) malloc(sizeof(LinkedListParticleSystem*) * MAX_NUMBER_OF_PARTICLE_SYSTEMS);
  
  int* xy = init_location(selected_index);
  particle_systems[selected_index] = init_particle_system(xy[0],xy[1]);

  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);

  init_bitmaps();

  glutMainLoop();
  return 0;
}
