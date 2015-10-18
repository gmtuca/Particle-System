/*
    Prints text on the viewport.
    Taken mostly from code provided by Toby Howard on the second year lab.
    I aknowledge this has not been fully written by me.
*/

void print_n(char* prefix, float n, GLfloat x, GLfloat y) {
  /* x, y: text position in window: range [0,0] (bottom left of window)
           to [1,1] (top right of window). */

  void* font = GLUT_BITMAP_HELVETICA_12;
  GLclampf r = 0.0;
  GLclampf g = 0.0;
  GLclampf b = 0.0;

  char str[30]; 
  char *ch; 
  GLint matrixMode;
  GLboolean lightingOn;

  sprintf(str, "%s: %g", prefix, n);

  lightingOn= glIsEnabled(GL_LIGHTING);        /* lighting on? */
  if (lightingOn) glDisable(GL_LIGHTING);

  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);  /* matrix mode? */

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glLoadIdentity();
      glPushAttrib(GL_COLOR_BUFFER_BIT);       /* save current colour */
        glColor3f(r, g, b);
        glRasterPos3f(x,y, 0.0);
        for(ch= str; *ch; ch++) {
           glutBitmapCharacter(font, (int)*ch);
        }
      glPopAttrib();
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(matrixMode);
  if (lightingOn) glEnable(GL_LIGHTING);
}