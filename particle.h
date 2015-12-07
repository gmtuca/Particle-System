#define DEFAULT_PARTICLE_LIFESPAN         		  1700
#define DEFAULT_PARTICLE_SPAWN_FREQUENCY  		  0.1
#define DEFAULT_PARTICLE_ORBIT_TIME       		  200
#define DEFAULT_PARTICLE_ORBIT_RADIUS     		  0.2
#define DEFAULT_PARTICLE_HSV_INCREMENT_AMOUNT 	0.25
#define DEFAULT_PARTICLE_INITIAL_VY				      0.25
#define DEFAULT_PARTICLE_GRAVITY                0.005

#define PI 3.1415

double myRandom(){ //Return random double within range [0,1]
  return (rand()/(double)RAND_MAX);
}

typedef struct Particle Particle;

struct Particle {
  GLfloat  x,  y,  z;
  GLfloat  vy;
  GLfloat orbit, orbital_radius;
  GLfloat t;
  GLfloat r, g, b;

  Particle* next;
  Particle* prev;
};

typedef enum {
	POINTS = 0,
	LINES,
	FIRE,
	TOBY,
	STEVE
} RenderOption;

typedef struct LinkedListParticleSystem{
	int n;
	int t;

	GLfloat x, z;

	double particle_spawn_frequency;
	hsv* current_hsv;
	double hsv_increment_amount;
	GLfloat initial_vy;
	GLfloat initial_orbit_radius;
	GLfloat particle_orbit_time;
  GLfloat gravity;

	int particle_lifespan;
	RenderOption renderOption;


	Particle* head;
  	Particle* tail;
} LinkedListParticleSystem;

void reset_attributes(LinkedListParticleSystem* ps, GLfloat x, GLfloat z){
  ps->n = 0;
  ps->t = 0;
  ps->x = x;
  ps->z = z;
  ps->head = NULL;
  ps->tail = NULL;
  ps->particle_lifespan           = DEFAULT_PARTICLE_LIFESPAN;
  ps->particle_spawn_frequency    = DEFAULT_PARTICLE_SPAWN_FREQUENCY;

  ps->hsv_increment_amount 		    = DEFAULT_PARTICLE_HSV_INCREMENT_AMOUNT;
  
  ps->current_hsv->h = 0;
  ps->current_hsv->s = 1;
  ps->current_hsv->v = 1;

  ps->initial_vy                  = DEFAULT_PARTICLE_INITIAL_VY;
  ps->initial_orbit_radius        = DEFAULT_PARTICLE_ORBIT_RADIUS;
  ps->particle_orbit_time         = DEFAULT_PARTICLE_ORBIT_TIME;

  ps->gravity                     = DEFAULT_PARTICLE_GRAVITY;

  ps->renderOption = POINTS;
}

void drawSpawnCircle(LinkedListParticleSystem* ps){
	glColor3f(1, 0, 0.5); 
	
	GLfloat radius = ps->initial_orbit_radius;

	int n_lines = 100;
	
	glBegin(GL_LINE_LOOP);
		int i;
		for(i = 0; i <= n_lines;i++) { 
			glVertex3f(ps->x + radius * cos(i *  2.0f * PI / n_lines), 0, ps->z + radius* sin(i * 2.0f * PI / n_lines));
		}
	glEnd();
}

LinkedListParticleSystem* init_particle_system(GLfloat x, GLfloat z){

  LinkedListParticleSystem* ps 	= (LinkedListParticleSystem*) malloc(sizeof(LinkedListParticleSystem));
  ps->current_hsv				= (hsv*) malloc(sizeof(hsv));
  reset_attributes(ps, x, z);

  return ps;
}

void killall(LinkedListParticleSystem* ps){
  Particle* p = ps->tail;

  while(p){
	ps->tail = p->prev;
	free(p);
	p = ps->tail;
  }

  ps->n = 0;
}

void reset(LinkedListParticleSystem* ps, GLfloat x, GLfloat z){
  killall(ps);
  reset_attributes(ps, ps->x, ps->z);
}

void update_particle_system(LinkedListParticleSystem* ps){
  //Update particle System
  ps->t++;
  
  ps->current_hsv->h += ps->hsv_increment_amount;
  if(ps->current_hsv->h > 360){ps->current_hsv->h = 0;}
}

///////////////////////////////////////////////////////////////
Particle* init_particle(){
  Particle* p = (Particle*) malloc(sizeof(Particle));
    p->x = 0;
    p->y = 0;
    p->z = 0;
    p->t = 0;
    p->orbit = myRandom() * 360;

    p->prev = NULL;
    p->next = NULL;

    return p;
}

void add_particle(LinkedListParticleSystem* ps, Particle* p){

    Particle* tmp = ps->head;
    ps->head = p;
    p->next = tmp;

    if(tmp){
      tmp->prev = p;
    }
    
    ps->n++;

    if(ps->n == 1){
      ps->tail = p;
    }

    rgb p_rgb = hsv2rgb(*ps->current_hsv);

    p->r = p_rgb.r;
    p->g = p_rgb.g;
    p->b = p_rgb.b;
    p->vy = ps->initial_vy;
    p->orbital_radius = ps->initial_orbit_radius;
}

void create_new_particle(LinkedListParticleSystem* ps){
	add_particle(ps, init_particle());
}

void create_particles(LinkedListParticleSystem* ps){
  if(ps->particle_spawn_frequency < 1){
  	if(ps->t % (int)(1/ps->particle_spawn_frequency) == 0){
  		create_new_particle(ps);
  	}
  }
  else{
  	int i;
  	for(i = 0; i < ps->particle_spawn_frequency; i++){
  		create_new_particle(ps);
  	}
  }
}
