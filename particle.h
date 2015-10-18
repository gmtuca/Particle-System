#define DEFAULT_PARTICLE_LIFESPAN         1700
#define DEFAULT_PARTICLE_SPAWN_FREQUENCY  0.2
#define DEFAULT_PARTICLE_ORBIT_TIME       200
#define DEFAULT_PARTICLE_ORBIT_RADIUS     0.2

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

typedef struct LinkedListParticleSystem{
	int n;
	int t;
	double particle_spawn_frequency;

	int particle_lifespan;

	Particle* head;
  Particle* tail;
} LinkedListParticleSystem;

LinkedListParticleSystem* init_particle_system(){

  LinkedListParticleSystem* ps = (LinkedListParticleSystem*) malloc(sizeof(LinkedListParticleSystem));
  ps->n = 0;
  ps->t = 0;
  ps->head = NULL;
  ps->tail = NULL;
  ps->particle_lifespan         = DEFAULT_PARTICLE_LIFESPAN;
  ps->particle_spawn_frequency  = DEFAULT_PARTICLE_SPAWN_FREQUENCY;

  return ps;
}

void reset(LinkedListParticleSystem* ps){

  Particle* p;

  while ((p = ps->head) != NULL) {
     ps->head = ps->head->next;
     free(p);
  }

  ps->n = 0;
  ps->t = 0;
  ps->particle_spawn_frequency  = DEFAULT_PARTICLE_SPAWN_FREQUENCY;
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
}

