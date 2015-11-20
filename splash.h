typedef struct Splash Splash;

struct Splash
{
  GLfloat x, z;
  double r, g, b;
  Splash* next;
};

typedef struct LinkedSplashes{
  int n;
	Splash* head;
} LinkedSplashes;

LinkedSplashes* init_splashes(){

  LinkedSplashes* ls 	= (LinkedSplashes*) malloc(sizeof(LinkedSplashes));
  ls->head = NULL;

  return ls;
}

Splash* init_splash(GLfloat x, GLfloat z, double r, double g, double b){
  Splash* s = (Splash*) malloc(sizeof(Splash));
  s->x = x;
  s->z = z;
  s->r = r;
  s->g = g;
  s->b = b;
  s->next = NULL;

  return s;
}

void add_splash(LinkedSplashes* ls, Splash* s){
  Splash* tmp = ls->head;
  ls->head = s;
  s->next = tmp;
  ls->n++;
}


void drawSplashes(LinkedSplashes* ls){
  Splash* s = ls->head;

  glBegin(GL_POINTS);
    while(s){
      //drawCross(s->x, s->z, s->r, s->g, s->b);
      //glColor3f(s->r, s->g, s->b);
      //glVertex3f(s->x,0,s->z);
      s = s->next;
    }
  glEnd();
}