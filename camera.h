#define EYE_Y_INIT 8.5f
#define EYE_ORBIT_INIT 75

#define CAMERA_DISTANCE_FROM_CENTER 10.0f

#ifndef DEG_TO_RAD
	#define DEG_TO_RAD 0.017453293
#endif

typedef struct Camera{
  GLfloat eyeX, eyeY, eyeZ;
  GLfloat lookatX, lookatY, lookatZ;
  double orbit;
} Camera;

void update_camera(Camera* camera){
	camera->eyeX = camera->lookatX + cos(camera->orbit * DEG_TO_RAD) * CAMERA_DISTANCE_FROM_CENTER; 
	camera->eyeZ = camera->lookatZ + sin(camera->orbit * DEG_TO_RAD) * CAMERA_DISTANCE_FROM_CENTER; 
}

void reset_camera(Camera* camera){
  camera->eyeY 	= EYE_Y_INIT;
  camera->orbit = EYE_ORBIT_INIT;
  camera->lookatX = 0;
  camera->lookatY = 0;
  camera->lookatZ = 0;
  update_camera(camera);
}

void lookat(Camera* camera, LinkedListParticleSystem* ps){
	camera->lookatX = ps->x;
	camera->lookatY = 0;
	camera->lookatZ = ps->z;
	update_camera(camera);
}

Camera* init_camera(){
	Camera* camera = (Camera*) malloc(sizeof(Camera));
  	reset_camera(camera);
  	return camera;
}

void view(Camera* camera){
	glLoadIdentity();
	gluLookAt(	camera->eyeX, 		camera->eyeY, 		camera->eyeZ,
	      		camera->lookatX, 	camera->lookatY, 	camera->lookatZ,
	      		0.0, 				1.0, 				0.0);
}

