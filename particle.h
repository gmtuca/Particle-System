#define DEFAULT_PARTICLE_LIFESPAN         		1700
#define DEFAULT_PARTICLE_SPAWN_FREQUENCY  		0.2
#define DEFAULT_PARTICLE_ORBIT_TIME       		200
#define DEFAULT_PARTICLE_ORBIT_RADIUS     		0.2
#define DEFAULT_PARTICLE_HSV_INCREMENT_AMOUNT 	0.25
#define DEFAULT_PARTICLE_INITIAL_VY				1

double myRandom(){ //Return random double within range [0,1]
  return (rand()/(double)RAND_MAX);
}

typedef struct Particle Particle;

struct Particle {
  GLfloat  x,  y,  z;
  GLfloat vx, vy, vz;
  GLfloat t;
  GLfloat orbit, orbital_radius, total_orbit_time, orbital_radius_resistance;
  GLfloat time_lived;
  GLfloat r, g, b;

  Particle* next;
  Particle* prev;
};

typedef enum {
	POINT = 0,
	SPHERE,
	TEAPOT
} RenderOption;

typedef struct LinkedListParticleSystem{
	int n;
	int t;
	double particle_spawn_frequency;
	hsv* current_hsv;
	double hsv_increment_amount;
	GLfloat initial_vy;

	int particle_lifespan;
	RenderOption renderOption;

	Particle* head;
  	Particle* tail;
} LinkedListParticleSystem;

void reset_attributes(LinkedListParticleSystem* ps){
  ps->n = 0;
  ps->t = 0;
  ps->head = NULL;
  ps->tail = NULL;
  ps->particle_lifespan         = DEFAULT_PARTICLE_LIFESPAN;
  ps->particle_spawn_frequency  = DEFAULT_PARTICLE_SPAWN_FREQUENCY;

  ps->hsv_increment_amount 		= DEFAULT_PARTICLE_HSV_INCREMENT_AMOUNT;
  
  ps->current_hsv->h = 0;
  ps->current_hsv->s = 1;
  ps->current_hsv->v = 1;

  ps->initial_vy = DEFAULT_PARTICLE_INITIAL_VY;

  ps->renderOption = POINT;
}

LinkedListParticleSystem* init_particle_system(){

  LinkedListParticleSystem* ps 	= (LinkedListParticleSystem*) malloc(sizeof(LinkedListParticleSystem));
  ps->current_hsv				= (hsv*) malloc(sizeof(hsv));
  reset_attributes(ps);

  return ps;
}

void reset(LinkedListParticleSystem* ps){

  Particle* p = ps->tail;

  while(p){
	ps->tail = p->prev;
	free(p);
	p = ps->tail;
  }

  reset_attributes(ps);
}

Particle* init_particle(){
  Particle* p = (Particle*) malloc(sizeof(Particle));
    p->x = 0;
    p->y = 0;
    p->z = 0;
    p->t = 0;
    p->time_lived = 0;
    p->vy = 1;
    p->orbit = myRandom() * 360;
    p->total_orbit_time   = DEFAULT_PARTICLE_ORBIT_TIME;
    p->orbital_radius     = DEFAULT_PARTICLE_ORBIT_RADIUS;

    p->prev = NULL;
    p->next = NULL;

    //TODO can probably get rid of most of these
    p->orbital_radius_resistance = 0;

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
}

void create_new_particle(LinkedListParticleSystem* ps){
	add_particle(ps, init_particle());
}