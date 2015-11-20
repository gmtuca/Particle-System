#define EYE_Y_INIT 8.5f
#define EYE_ORBIT_INIT 75

#define CAMERA_DISTANCE_FROM_CENTER 10.0f

#ifndef DEG_TO_RAD
	#define DEG_TO_RAD 0.017453293
#endif

typedef struct Camera{
  GLfloat eyeX, eyeY, eyeZ;
  double orbit;
} Camera;

void update_camera(Camera* camera){
	camera->eyeX = cos(camera->orbit * DEG_TO_RAD) * CAMERA_DISTANCE_FROM_CENTER; 
	camera->eyeZ = sin(camera->orbit * DEG_TO_RAD) * CAMERA_DISTANCE_FROM_CENTER; 
}

void reset_camera(Camera* camera){
  camera->eyeY 	= EYE_Y_INIT;
  camera->orbit = EYE_ORBIT_INIT;
  update_camera(camera);
}

Camera* init_camera(){
	Camera* camera = (Camera*) malloc(sizeof(Camera));
  	reset_camera(camera);
  	return camera;
}

void view(Camera* camera){
	gluLookAt(	camera->eyeX, camera->eyeY, camera->eyeZ,
	      		0.0, 0.0, 0.0,
	      		0.0, 1.0, 0.0);
}

