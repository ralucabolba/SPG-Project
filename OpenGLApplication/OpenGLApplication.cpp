// OpenGLApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "glut.h"
#include <gl/gl.h>
#include "glui.h"
#include "glm.h"
#include "tga.h"
#include <time.h>

#define MAX_PARTICLES 1000
#define RAIN	0
#define SNOW	1
#define NONE	2

int rain = 0;
int snow = 0;
int fog = 0;

float slowdown = 2.0;
float velocity = 0.0;
float zoom = -15.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;
float xposs[25];
float zposs[25];

float xpos, zpos;

//bool first = true;
//bool fog = false;
bool bulb = false;
bool sun = true;
bool anim = true;

int loop;
int fall = NONE;
int pas = 0;

GLclampf fogColor[] = { 1.0, 1.0, 1.0 };

typedef struct {
	// Life
	bool alive;	// is the particle alive?
	float life;	// particle lifespan
	float fade; // decay
	// color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float vel;
	// Gravity
	float gravity;
}particles;

particles par_sys[MAX_PARTICLES];

int main_window;
int screen_width=800;
int screen_height=600;
int rotateAngle = 0;

GLUI_RadioGroup *radio;
int   scene_mode = 1;
//int   wireframe = 0;
//int segments = 8;

float r_light_color = 1.0 , g_light_color = 1.0 , b_light_color = 1.0 ;
int light_value1 = 0;
int light_value2 = 0;
int shadow = 0;

#define RADIO_BUTTONS_GROUP_ID 2
#define LIGHT_ID0 3
#define LIGHT_ID1 4

float TransXYZ[] = { 0, 0, 0 };
float rot_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

GLfloat mylight_ambient[] = { 1, 1, 1, 1 };
GLfloat mylight_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mylight_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mylight_position[] = { 0, 50.0, 0, 1.0 };

GLfloat lamp_ambient[] = { 0.1, 0.1, 0.1, 1 };
GLfloat lamp_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lamp_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lamp_position[] = { 0.8, -2.7, 0.55, 1 };

GLfloat material_3_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat material_3_diffuse[] = { 0.75164f, 0.60648f, 0.60648f, 1.0f };
GLfloat material_3_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat material_3_shininess[] = { 0.4f };
GLfloat material_3_emission[] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat material_no_emission[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat material_1_ambient[] = { 1, 1, 1, 1.0 };
GLfloat material_1_diffuse[] = { 1.0, 0.0, 1.0, 1.0 };
GLfloat material_1_specular[] = { 0.0, 0.0, 0.0, 1 };


GLuint treeTexture; //variable used for the ID of the tree texture
GLuint skyboxTexture; //variable used for the IDs of the six textures that form the skybox 
GLuint roomTexture[6];
GLfloat fSkyboxSizeX, fSkyboxSizeY, fSkyboxSizeZ,
		fRoomSizeX, fRoomSizeY, fRoomSizeZ; //skybox size on X, Y and Z axes
GLfloat fTreeSize;

GLMmodel *couch;
GLMmodel *lamp;
GLMmodel *house;
GLMmodel *tree;
GLMmodel *clock_face;
GLMmodel *clock_h;
GLMmodel *clock_m;
GLMmodel *sneakers;
GLMmodel *box;
GLuint boxTexture;
GLuint couchTexture;
GLuint lampTexture;
GLuint houseTexture;
GLuint bradTexture;
GLuint clockTexture;
GLuint sneakerTexture;

GLfloat fGlobalAngleX, fGlobalAngleY, fGlobalAngleZ,
fTranslateX, fTranslateY, fTranslateZ,
fScaleX = 1.0, fScaleY = 1.0, fScaleZ = 1.0,
bec_x = -3.2, bec_y = -0.5, bec_z = -9.5; //global rotation angles, translation and scale factors


GLfloat pos_x, pos_y, pos_z;
GLfloat light_x, light_y, light_z;
GLfloat sneaker_x = 0.1, sneaker_y = -6.4, sneaker_z = -14.9,
		rot_sneaker_x=0.0, rot_sneaker_y=0.0, rot_sneaker_z = 0.0,
		scale_sneaker_x = 0.5, scale_sneaker_y = 0.5, scale_sneaker_z = 0.5;
GLfloat rot_h, rot_m;



/////////////////////////////////////////UNMBRE/////////////////////////////
//equations of the planes on which the shadow is cast
GLfloat fvFloorOutsidePlaneEq[4];
GLfloat fvFloorPlaneEq[4];
GLfloat fvFrontPlaneEq[4];
GLfloat fvLeftWallPlaneEq[4];
GLfloat fvRightWallPlaneEq[4];
GLfloat fvBackWallPlaneEq[4];
GLfloat fvCeilingPlaneEq[4];

//projected shadows matrices
GLfloat fvFloorOutsideShadowMat[16];
GLfloat fvFloorShadowMat[16];
GLfloat fvFrontShadowMat[16];
GLfloat fvLeftWallShadowMat[16];
GLfloat fvBackWallShadowMat[16];
GLfloat fvRightWallShadowMat[16];
GLfloat fvCeilingShadowMat[16];

void PlaneEq(GLfloat plane[4], GLfloat p0[4], GLfloat p1[4], GLfloat p2[4]);
void ComputePlaneEquations();
void ComputeShadowMatrix(GLfloat shadowMat[16], GLfloat plane[4], GLfloat lightPos[4]);
void ComputeShadowMatrices();


void PlaneEq(GLfloat plane[4], GLfloat p0[4], GLfloat p1[4], GLfloat p2[4])
{
	GLfloat vec0[3], vec1[3];

	vec0[0] = p1[0] - p0[0];
	vec0[1] = p1[1] - p0[1];
	vec0[2] = p1[2] - p0[2];

	vec1[0] = p2[0] - p0[0];
	vec1[1] = p2[1] - p0[1];
	vec1[2] = p2[2] - p0[2];

	plane[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
	plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
	plane[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];

	plane[3] = -(plane[0] * p0[0] + plane[1] * p0[1] + plane[2] * p0[2]);
}

void ComputeIndoorPlaneEquations()
{
	//floor points
	GLfloat fvFloorP0[4] = { fRoomSizeX / 2, -6.0 / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvFloorP1[4] = { fRoomSizeX / 2, -6.0 / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvFloorP2[4] = { -fRoomSizeX / 2, -6.0 / 2, -fRoomSizeZ / 2, 1.0 };

	//left wall points
	GLfloat fvLeftWallP0[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP1[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP2[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//Right wall points
	GLfloat fvRightWallP0[4] = { fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvRightWallP1[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvRightWallP2[4] = { fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//back wall points
	GLfloat fvBackWallP0[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvBackWallP1[4] = { fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvBackWallP2[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//ceiling points
	GLfloat fvCeilingP0[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvCeilingP1[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvCeilingP2[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	PlaneEq(fvFloorPlaneEq, fvFloorP0, fvFloorP1, fvFloorP2);
	PlaneEq(fvLeftWallPlaneEq, fvLeftWallP0, fvLeftWallP1, fvLeftWallP2);
	PlaneEq(fvRightWallPlaneEq, fvRightWallP0, fvRightWallP1, fvRightWallP2);
	PlaneEq(fvBackWallPlaneEq, fvBackWallP0, fvBackWallP1, fvBackWallP2);
	PlaneEq(fvCeilingPlaneEq, fvCeilingP0, fvCeilingP1, fvCeilingP2);
}
void ComputePlaneEquations()
{
	//floor outdoorspoints
	GLfloat fvFloorOutsidePO0[4] = { fSkyboxSizeX / 2, -6.0 / 2, fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvFloorOutsidePO1[4] = { fSkyboxSizeX / 2, -6.0 / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvFloorOutsidePO2[4] = { -fSkyboxSizeX / 2, -6.0 / 2, -fSkyboxSizeZ / 2, 1.0 };

	//glTexCoord2f(1, 1); glVertex3f(sizeRoomX, -sizeRoomY, -sizeRoomZ);
	//glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, -sizeRoomY, -sizeRoomZ);
	//glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, -sizeRoomY, sizeRoomZ);
	//glTexCoord2f(1, 0); glVertex3f(sizeRoomX, -sizeRoomY, sizeRoomZ);

	//floor indoors points
	GLfloat fvFloorP0[4] = { 9.0 / 2, -13.1 / 2, 9.0 / 2, 1.0 };
	GLfloat fvFloorP1[4] = { 9.0 / 2, -13.1 / 2, -9.0 / 2, 1.0 };
	GLfloat fvFloorP2[4] = { -9.0 / 2, -13.1 / 2, -9.0 / 2, 1.0 };

	//left wall points
	GLfloat fvLeftWallP0[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP1[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP2[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//Right wall points
	GLfloat fvRightWallP0[4] = { fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvRightWallP1[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvRightWallP2[4] = { fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//back wall points
	GLfloat fvBackWallP0[4] = { -fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvBackWallP1[4] = { fRoomSizeX / 2, -fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvBackWallP2[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };

	//ceiling points
	GLfloat fvCeilingP0[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvCeilingP1[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvCeilingP2[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };


	//front wall points
	GLfloat fvFrontP0[4] = { fRoomSizeX / 2, fRoomSizeY / 2, -fRoomSizeZ / 2, 1.0 };
	GLfloat fvFrontP1[4] = { fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };
	GLfloat fvFrontP2[4] = { -fRoomSizeX / 2, fRoomSizeY / 2, fRoomSizeZ / 2, 1.0 };

	//glTexCoord2f(1, 1); glVertex3f(sizeRoomX, sizeRoomY, sizeRoomZ);
	//glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, sizeRoomY, sizeRoomZ);
	//glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, sizeRoomY, -sizeRoomZ);
	//glTexCoord2f(1, 0); glVertex3f(sizeRoomX, sizeRoomY, -sizeRoomZ);

	PlaneEq(fvFloorPlaneEq, fvFloorP0, fvFloorP1, fvFloorP2);
	PlaneEq(fvLeftWallPlaneEq, fvLeftWallP0, fvLeftWallP1, fvLeftWallP2);
	PlaneEq(fvRightWallPlaneEq, fvRightWallP0, fvRightWallP1, fvRightWallP2);
	PlaneEq(fvBackWallPlaneEq, fvBackWallP0, fvBackWallP1, fvBackWallP2);
	//PlaneEq(fvCeilingPlaneEq, fvCeilingP0, fvCeilingP1, fvCeilingP2);
	PlaneEq(fvFrontPlaneEq, fvFrontP0, fvFrontP1, fvFrontP2);

	PlaneEq(fvFloorOutsidePlaneEq, fvFloorOutsidePO0, fvFloorOutsidePO1, fvFloorOutsidePO2);
}

void ComputeShadowMatrices()
{
	ComputeShadowMatrix(fvFloorOutsideShadowMat, fvFloorOutsidePlaneEq, mylight_position);

	ComputeShadowMatrix(fvFloorShadowMat, fvFloorPlaneEq, lamp_position);
	ComputeShadowMatrix(fvLeftWallShadowMat, fvLeftWallPlaneEq, lamp_position);
	ComputeShadowMatrix(fvRightWallShadowMat, fvRightWallPlaneEq, lamp_position);
	ComputeShadowMatrix(fvBackWallShadowMat, fvBackWallPlaneEq, lamp_position);
	ComputeShadowMatrix(fvFrontShadowMat, fvFrontPlaneEq, lamp_position);
}

void ComputeShadowMatrix(GLfloat shadowMat[16], GLfloat plane[4], GLfloat lightPos[4])
{
	GLfloat dotProduct;

	dotProduct = plane[0] * lightPos[0] +
		plane[1] * lightPos[1] +
		plane[2] * lightPos[2] +
		plane[3] * lightPos[3];

	shadowMat[0] = dotProduct - lightPos[0] * plane[0];
	shadowMat[1] = 0.0f - lightPos[1] * plane[0];
	shadowMat[2] = 0.0f - lightPos[2] * plane[0];
	shadowMat[3] = 0.0f - lightPos[3] * plane[0];

	shadowMat[4] = 0.0f - lightPos[0] * plane[1];
	shadowMat[5] = dotProduct - lightPos[1] * plane[1];
	shadowMat[6] = 0.0f - lightPos[2] * plane[1];
	shadowMat[7] = 0.0f - lightPos[3] * plane[1];

	shadowMat[8] = 0.0f - lightPos[0] * plane[2];
	shadowMat[9] = 0.0f - lightPos[1] * plane[2];
	shadowMat[10] = dotProduct - lightPos[2] * plane[2];
	shadowMat[11] = 0.0f - lightPos[3] * plane[2];

	shadowMat[12] = 0.0f - lightPos[0] * plane[3];
	shadowMat[13] = 0.0f - lightPos[1] * plane[3];
	shadowMat[14] = 0.0f - lightPos[2] * plane[3];
	shadowMat[15] = dotProduct - lightPos[3] * plane[3];
}
//////////////////////////////////////////////////////////////////



/////////////////////////////////////////PLOAIE SI NINSOARE///////////////////////////////////
void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 51) - 8.0;
	par_sys[i].ypos = 5.0;
	par_sys[i].zpos = (float)(rand() % 51) + 2.0;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

}

void drawRain() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(0.5, 0.5, 1.0);
			glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x, y + 0.5, z);
			glEnd();

			// Update values
			//Move
			// Adjust slowdown for speed!
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				par_sys[loop].life = -1.0;
			}
			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

void drawSnow() {
	float x, y, z;
	for (loop = 0; loop < MAX_PARTICLES; loop = loop + 2) {
		if (par_sys[loop].alive == true) {
			x = par_sys[loop].xpos;
			y = par_sys[loop].ypos;
			z = par_sys[loop].zpos + zoom;

			// Draw particles
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(x, y, z);
			glutSolidSphere(0.05, 16, 16);
			glPopMatrix();

			// Update values
			//Move
			par_sys[loop].ypos += par_sys[loop].vel / (slowdown * 1000);
			par_sys[loop].vel += par_sys[loop].gravity;
			// Decay
			par_sys[loop].life -= par_sys[loop].fade;

			if (par_sys[loop].ypos <= -10) {
				int zi = z - zoom + 10;
				int xi = x + 10;

				par_sys[loop].life = -1.0;
			}

			//Revive
			if (par_sys[loop].life < 0.0) {
				initParticles(loop);
			}
		}
	}
}

void EnableFog()
{
	glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.1f);
	glEnable(GL_FOG);
}

//////////////////////////////////////////////////////////////////////////////////
void myGlutIdleFunction(void)
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);
	glutPostRedisplay();
}

void InitModel(GLMmodel **pModel, char *filename)
{
	if (!(*pModel))
	{
		*pModel = glmReadOBJ(filename);
		if (!(*pModel))
			exit(0);
		glmUnitize(*pModel);
		glmFacetNormals(*pModel);
		glmVertexNormals(*pModel, 90.0);
	}
}
bool in_house(GLfloat x, GLfloat y, GLfloat z) {
	if (x >= -5.0 && x <= 11.0 &&
		y >= -3.2 && y <= 3.2 &&
		z >= 17.0 && z <= 33.0){
		//printf("Sunt in casa !\n");

		return true;
	}

	return false;
}
GLint tree_x[21], tree_z[21];

bool collision(float sneaker_x, float sneaker_y, float sneaker_z){
	//if (!in_house(sneaker_x, sneaker_y, sneaker_z))
	//	return true;
	if ((sneaker_x + scale_sneaker_x > -5.0)
		&& (sneaker_x < 1.9)
		&& (sneaker_y < -3.5)
		&& (sneaker_y + scale_sneaker_y > -6.4)
		&& (sneaker_z + scale_sneaker_z > -22.4)
		&& (sneaker_z < -17.6)
		)
		return true;
	if ((sneaker_x + scale_sneaker_x > -9.6)
		&& (sneaker_x < -7.1)
		&& (sneaker_y < -2.4)
		&& (sneaker_y + scale_sneaker_y > -6.8)
		&& (sneaker_z + scale_sneaker_z > -22.1)
		&& (sneaker_z < -18.7)
		)
		return true;
	/*if ((sneaker_x + scale_sneaker_x > -12.5)
		&& (sneaker_x < 4.6)
		&& (sneaker_y < 5.1)
		&& (sneaker_y + scale_sneaker_y > -6.8)
		&& (sneaker_z + scale_sneaker_z > -23.2)
		&& (sneaker_z < -6.3)
		)
		return true;*/
	return false;
	
}

//glTranslatef(-8.4, -4.6, -20.7);
//glScalef(2.0, 2.0, 2.0);

void generate_coord(){
	int i, pos_x, pos_z;
	srand(time(NULL));
	for (i = 0; i < 20; i++){
		pos_x = rand() % 50 - 25;
		pos_z = rand() % 50 - 25;

		if (!in_house(pos_x, -fSkyboxSizeY + fTreeSize, pos_z)){
			tree_x[i] = pos_x;
			tree_z[i] = pos_z;
		}
	}
}
void initOpenGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 1.0f, 1000.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_MODELVIEW);

	generate_coord();

	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}

	glGenTextures(1, &treeTexture);
	loadTGA("Textures\\pine.tga", treeTexture);

	glGenTextures(1, &skyboxTexture);
	loadTGA("Textures\\greenhill_negative_y.tga", skyboxTexture);

	glGenTextures(6, roomTexture);
	loadTGA("Textures\\wallpaper.tga", roomTexture[0]);
	loadTGA("Textures\\floor.tga", roomTexture[1]);
	loadTGA("Textures\\wallpaper.tga", roomTexture[2]);
	loadTGA("Textures\\wallpaper.tga", roomTexture[3]);
	loadTGA("Textures\\ceiling.tga", roomTexture[4]);
	loadTGA("Textures\\wallpaper.tga", roomTexture[5]);
	
	//set tree size
	fTreeSize = 1.0;

	//set skybox size
	fSkyboxSizeX = 50.0;
	fSkyboxSizeY = 3.0;
	fSkyboxSizeZ = 50.0;

	fRoomSizeX = 9.0;
	fRoomSizeY = 6.0;
	fRoomSizeZ = 9.0;

	//set tree size
	fTreeSize = 1.0;

	InitModel(&box, "skydome.obj");
	glGenTextures(1, &boxTexture);
	loadTGA("Textures\\am_02.tga", boxTexture);

	InitModel(&couch, "sofa.obj");
	glGenTextures(1, &couchTexture);
	loadTGA("Textures\\sofa.tga", couchTexture);

	InitModel(&lamp, "lampa.obj");
	glGenTextures(1, &lampTexture);
	loadTGA("Textures\\lamp.tga", lampTexture);

	InitModel(&house, "new_house.obj");
	glGenTextures(1, &houseTexture);
	loadTGA("Textures\\house2.tga", houseTexture);

	InitModel(&clock_face, "clock.obj");
	InitModel(&clock_h, "hour.obj");
	InitModel(&clock_m, "min2.obj");
	glGenTextures(1, &clockTexture);
	loadTGA("Textures\\clock.tga", clockTexture);


	InitModel(&sneakers, "tenisi.obj");
	glGenTextures(1, &sneakerTexture);
	loadTGA("Textures\\sneaker_texture.tga", sneakerTexture);

	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
}
void control_callback(int control)
{
	if (light_value1 == 1)
		glEnable(GL_LIGHT0);
	else
		glDisable(GL_LIGHT0);

	if (light_value2 == 1){
		//glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT1);
		
	}
	else{
		glDisable(GL_LIGHT1);
		//glDisable(GL_LIGHTING);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_no_emission);
	}
		
}

void special_keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		if (!collision(sneaker_x + 0.1, sneaker_y, sneaker_z)){
			sneaker_x += 0.1;
			printf("sneaker_x = %f\n", sneaker_x);
		}
	}
	if (key == GLUT_KEY_DOWN) {
		if (!collision(sneaker_x - 0.1, sneaker_y, sneaker_z)){
			sneaker_x -= 0.1;
			printf("sneaker_x = %f\n", sneaker_x);
		}
	}
	if (key == GLUT_KEY_RIGHT) {
		if (!collision(sneaker_x, sneaker_y + 0.1, sneaker_z)){
			sneaker_y += 0.1;
			printf("sneaker_y = %f\n", sneaker_y); 
		}
	}
	if (key == GLUT_KEY_LEFT) {
		if (!collision(sneaker_x, sneaker_y - 0.1, sneaker_z)){
			sneaker_y -= 0.1;
			printf("sneaker_y = %f\n", sneaker_y);
		}
	}
	if (key == GLUT_KEY_PAGE_UP) {
		if (!collision(sneaker_x, sneaker_y, sneaker_z + 0.1)){
			sneaker_z += 0.1;
			printf("sneaker_z = %f\n", sneaker_z);
		}
	}
	if (key == GLUT_KEY_PAGE_DOWN) {
		if (!collision(sneaker_x , sneaker_y, sneaker_z - 0.1)){
			sneaker_z -= 0.1;
			printf("sneaker_z = %f\n", sneaker_z);
		}
	}

	if (key == GLUT_KEY_F1) {
		scale_sneaker_x += 0.1;
	}
	if (key == GLUT_KEY_F2) {
		scale_sneaker_x -= 0.1;
	}
	if (key == GLUT_KEY_F3) {
		scale_sneaker_y += 0.1;
	}
	if (key == GLUT_KEY_F4) {
		scale_sneaker_y -= 0.1;
	}
	if (key == GLUT_KEY_F5) {
		scale_sneaker_z += 0.1;
	}
	if (key == GLUT_KEY_F6) {
		scale_sneaker_z -= 0.1;
	}
	

	if (key == GLUT_KEY_F7) {
		rot_sneaker_x += 1;
		if (rot_sneaker_x >= 360.0){
			rot_sneaker_x = (GLint)rot_sneaker_x % 360;
		}
	}
	if (key == GLUT_KEY_F8) {
		rot_sneaker_x -= 1;
		if (rot_sneaker_x <= -360.0){
			rot_sneaker_x = (GLint)rot_sneaker_x % 360;
		}
	}
	if (key == GLUT_KEY_F9) {
		rot_sneaker_y += 1;
		if (rot_sneaker_y >= 360.0){
			rot_sneaker_y = (GLint)rot_sneaker_y % 360;
		}
	}
	if (key == GLUT_KEY_F10) {
		rot_sneaker_y -= 1;
		if (rot_sneaker_y <= -360.0){
			rot_sneaker_y = (GLint)rot_sneaker_y % 360;
		}
	}
	if (key == GLUT_KEY_F11) {
		rot_sneaker_z += 1;
		if (rot_sneaker_z >= 360.0){
			rot_sneaker_z = (GLint)rot_sneaker_z % 360;
		}
	}
	if (key == GLUT_KEY_F12) {
		rot_sneaker_z -= 1;
		if (rot_sneaker_z <= 360.0){
			rot_sneaker_z = (GLint)rot_sneaker_z % 360;
		}
	}
}

void DrawSkybox(GLfloat sizeX, GLfloat sizeY, GLfloat sizeZ)
{
	glEnable(GL_TEXTURE_2D); //enable 2D texturing

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);

	//negative y plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeX, -sizeY, -sizeZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeX, -sizeY, -sizeZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeX, -sizeY, sizeZ);
	glTexCoord2f(1, 0); glVertex3f(sizeX, -sizeY, sizeZ);
	glEnd();

	glDisable(GL_TEXTURE_2D); //disable 2D texuring
}


void DrawRoom(GLfloat sizeRoomX, GLfloat sizeRoomY, GLfloat sizeRoomZ)
{
	glEnable(GL_TEXTURE_2D); //enable 2D texturing

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);

	//negative y plane
	glBindTexture(GL_TEXTURE_2D, roomTexture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, -sizeRoomY, sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(sizeRoomX, -sizeRoomY, sizeRoomZ);
	glEnd();


	

	
	//negative x plane -left
	glBindTexture(GL_TEXTURE_2D, roomTexture[0]); //select the current texture
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-sizeRoomX, sizeRoomY, -sizeRoomZ); //assign each corner of the texture to a 3D vertex in the OpenGL scene
	glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, sizeRoomY, sizeRoomZ); //(0,0) is the left lower corner, while (1,1) is the right upper corner of the texture
	glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, -sizeRoomY, sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(-sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glEnd(); 

	/*
	glStencilFunc(GL_EQUAL, 1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	//left wall shadow
	
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
		//project the teapot onto the left wall
		glMultMatrixf(fvLeftWallShadowMat);
		//apply the rotation of the teapot to the shadow
		//draw the projected teapot (the shadow)
	
		/*glPushMatrix();
		glTranslatef(-8.4, -4.6, -20.7);
		glScalef(2.0, 2.0, 2.0);
		//glBindTexture(GL_TEXTURE_2D, lampTexture);
		glmDraw(lamp, GLM_SMOOTH | GLM_MATERIAL);
		glTranslatef(8.4, 4.6, 20.7);
		glPopMatrix();
		
	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	
	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);
	*/
	//negative z plane
	glBindTexture(GL_TEXTURE_2D, roomTexture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-sizeRoomX, sizeRoomY, sizeRoomZ);
	glTexCoord2f(0, 1); glVertex3f(sizeRoomX, sizeRoomY, sizeRoomZ);
	glTexCoord2f(0, 0); glVertex3f(sizeRoomX, -sizeRoomY, sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(-sizeRoomX, -sizeRoomY, sizeRoomZ);
	glEnd();


	/*
	glStencilFunc(GL_EQUAL, 1, 1);

	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	//back wall shadow
	
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
		//project the teapot onto the back wall
		glMultMatrixf(fvBackWallShadowMat);
	
		/*glPushMatrix();
		glTranslatef(-8.4, -4.6, -20.7);
		glScalef(2.0, 2.0, 2.0);
		//glBindTexture(GL_TEXTURE_2D, lampTexture);
		glmDraw(lamp, GLM_SMOOTH | GLM_MATERIAL);
		glTranslatef(8.4, 4.6, 20.7);
		glPopMatrix();
		
	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	
	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);
	*/
	//positive x plane
	glBindTexture(GL_TEXTURE_2D, roomTexture[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeRoomX, sizeRoomY, sizeRoomZ);
	glTexCoord2f(0, 1); glVertex3f(sizeRoomX, sizeRoomY, -sizeRoomZ);
	glTexCoord2f(0, 0); glVertex3f(sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(sizeRoomX, -sizeRoomY, sizeRoomZ);
	glEnd();

	/*
	glStencilFunc(GL_EQUAL, 1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	//right wall shadow
	
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
		//project the teapot onto the right wall
		glMultMatrixf(fvRightWallShadowMat);
	
		/*glPushMatrix();
		glTranslatef(-8.4, -4.6, -20.7);
		glScalef(2.0, 2.0, 2.0);
		//glBindTexture(GL_TEXTURE_2D, lampTexture);
		glmDraw(lamp, GLM_SMOOTH | GLM_MATERIAL);
		glTranslatef(8.4, 4.6, 20.7);
		glPopMatrix();
		

	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT); */

	//positive y plane
	glBindTexture(GL_TEXTURE_2D, roomTexture[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeRoomX, sizeRoomY, sizeRoomZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, sizeRoomY, sizeRoomZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, sizeRoomY, -sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(sizeRoomX, sizeRoomY, -sizeRoomZ);
	glEnd();

	/*
	glStencilFunc(GL_EQUAL, 1, 1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	//ceiling shadow
	
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
		//project the teapot onto the ceiling
		glMultMatrixf(fvFrontShadowMat);

		/*glPushMatrix();
		glTranslatef(-8.4, -4.6, -20.7);
		glScalef(2.0, 2.0, 2.0);
		//glBindTexture(GL_TEXTURE_2D, lampTexture);
		glmDraw(lamp, GLM_SMOOTH | GLM_MATERIAL);
		glTranslatef(8.4, 4.6, 20.7);
		glPopMatrix();
		
	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);
	*/
	//positive z plane
	glBindTexture(GL_TEXTURE_2D, roomTexture[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeRoomX, sizeRoomY, -sizeRoomZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeRoomX, sizeRoomY, -sizeRoomZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glTexCoord2f(1, 0); glVertex3f(sizeRoomX, -sizeRoomY, -sizeRoomZ);
	glEnd();

	glStencilFunc(GL_EQUAL, 1, 1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D); //disable 2D texuring

	/*floor shadow	
	if (shadow == 1){
		
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


	glStencilFunc(GL_ALWAYS, 1, 0);
	glClear(GL_STENCIL_BUFFER_BIT);*/
}

void DrawSingleTreeTexture(GLfloat size)
{
	glEnable(GL_TEXTURE_2D); //enable 2D texturing		
	glBindTexture(GL_TEXTURE_2D, treeTexture); //select the current texture

	glEnable(GL_ALPHA_TEST); //enable alpha testing
	glAlphaFunc(GL_GREATER, 0.1f); //select the alpha testing function
	//GL_GREATER function passes if the incoming alpha value is greater than the reference value

	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(size, size, 0.0); //assign each corner of the texture to a 3D vertex in the OpenGL scene
	glTexCoord2f(0, 1); glVertex3f(-size, size, 0.0); //(0,0) is the left lower corner, while (1,1) is the right upper corner of the texture
	glTexCoord2f(0, 0); glVertex3f(-size, -size, 0.0);
	glTexCoord2f(1, 0); glVertex3f(size, -size, 0.0);
	glEnd();

	glDisable(GL_ALPHA_TEST); //disable alpha testing
	glDisable(GL_TEXTURE_2D); //disable 2D texturing
}




void drawTrees(){
	int i;
	for (i = 0; i < 20; i++){
		
		glPushMatrix();
			glTranslatef(tree_x[i], -fSkyboxSizeY + fTreeSize, tree_z[i]);
			DrawSingleTreeTexture(fTreeSize);
			glRotatef(45.0, 0, 1, 0);
			DrawSingleTreeTexture(fTreeSize);
			glRotatef(45.0, 0, 1, 0);
			DrawSingleTreeTexture(fTreeSize);
			glRotatef(45.0, 0, 1, 0);
			DrawSingleTreeTexture(fTreeSize);
			glRotatef(45.0, 0, 1, 0);
			DrawSingleTreeTexture(fTreeSize);
		glPopMatrix();
	}
}

void animation() {
	pas++;
	if (pas <= 10) {
		TransXYZ[2] += 0.1;
	}
	else if (pas >= 15 && pas <= 20) {
		TransXYZ[1] += 0.1;
	}
	else if (pas >= 25 && pas <= 70) {
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 80 && pas <= 170)
	{
		fGlobalAngleY -= 1.0;
		if (fGlobalAngleY < -360) {
			fGlobalAngleY = 360;
		}
	}

	else if (pas >= 180 && pas <= 225) {
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 235 && pas <= 276) {
		TransXYZ[2] += 0.1;
	}
	
	else if (pas >= 280 && pas <= 310){
		TransXYZ[0] += 0.1;
	}
	else if (pas >= 320 && pas <= 400){
		TransXYZ[2] += 0.1;
	}
	else if (pas >= 410 && pas <= 425){
		TransXYZ[1] += 0.1;
	}
	else if (pas >= 435 && pas <= 465){
		fGlobalAngleY -= 1.0;
		if (fGlobalAngleY < -360) {
			fGlobalAngleY = 360;
		}
	}
	else if (pas >= 470 && pas <= 530){
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 531) {
		anim = false;
	}

}

void Timer(int i) {

	animation();

	glutPostRedisplay();
	if (anim) {
		glutTimerFunc(50, Timer, 0);
	}

}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!in_house(pos_x, pos_y, pos_z)){
		gluLookAt(0.0, -fSkyboxSizeY + 3.0, 3.0, 0.0, -fSkyboxSizeY + 3.0, -10.0, 0.0, 1.0, 0.0);

		glMultMatrixf(rot_matrix);

		switch (scene_mode){
		case 0: //wireframe
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 1: //smooth
			glEnable(GL_SMOOTH);
			glShadeModel(GL_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 2: //solid
			glEnable(GL_FLAT);
			glShadeModel(GL_FLAT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 3: //poligonal
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		default:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
		if (fog == 1) {
			EnableFog();
		}
		else {
			glDisable(GL_FOG);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		if (rain == 1) {
			drawRain();
		}
		if (snow == 1) {
			drawSnow();
		}

		pos_x = 0;
		pos_y = -fSkyboxSizeY + 1.2;
		pos_z = 6.0;

		//set global rotation on the X,Y and Z axes
		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleX, 1.0, 0.0, 0.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleY, 0.0, 1.0, 0.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleZ, 0.0, 0.0, 1.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		//translate
		glTranslatef(TransXYZ[0], TransXYZ[1], TransXYZ[2]);
		pos_x += TransXYZ[0];
		pos_y += TransXYZ[1];
		pos_z += TransXYZ[2];

		//scale
		glScalef(fScaleX, fScaleY, fScaleZ);
		pos_x *= fScaleX;
		pos_y *= fScaleY;
		pos_z *= fScaleZ;

		glLightfv(GL_LIGHT0, GL_POSITION, mylight_position);
		//mylight_position[0] = 0.0;
		//mylight_position[1] = 50.0;
		//mylight_position[2] = 0.0;

		glLightfv(GL_LIGHT0, GL_AMBIENT, mylight_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, mylight_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, mylight_specular);

		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_1_ambient);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,material_1_diffuse);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_1_specular);

		material_1_ambient[0] = 1.0;
		material_1_ambient[1] = 1.0;
		material_1_ambient[2] = 1.0;

		material_1_diffuse[0] = 1.0;
		material_1_diffuse[1] = 1.0;
		material_1_diffuse[2] = 1.0;

		material_1_specular[0] = 1.0;
		material_1_specular[1] = 1.0;
		material_1_specular[2] = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_1_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_1_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_1_specular);

		glPushMatrix();
		DrawSkybox(fSkyboxSizeX, fSkyboxSizeY, fSkyboxSizeZ);
		glDisable(GL_TEXTURE_2D); //disable 2D texuring

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();

		glScalef(50.0, 50.0, 50.0);
		glTranslatef(0, 0.44, 0);
		glBindTexture(GL_TEXTURE_2D, boxTexture);
		glmDraw(box, GLM_SMOOTH | GLM_TEXTURE);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		if (shadow == 1){
			ComputePlaneEquations();
			ComputeShadowMatrices();

			glStencilFunc(GL_EQUAL, 1, 1);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);

			glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
			glMultMatrixf(fvFloorOutsideShadowMat);
			glPushMatrix();
				glTranslatef(2.8, -fSkyboxSizeY + fTreeSize, -6.0);
				DrawSingleTreeTexture(fTreeSize);
				glRotatef(45.0, 0, 1, 0);
				DrawSingleTreeTexture(fTreeSize);
				glRotatef(45.0, 0, 1, 0);
				DrawSingleTreeTexture(fTreeSize);
				glRotatef(45.0, 0, 1, 0);
				DrawSingleTreeTexture(fTreeSize);
				glRotatef(45.0, 0, 1, 0);
				DrawSingleTreeTexture(fTreeSize);
			glPopMatrix();

			drawTrees();

			glPushMatrix();
				glTranslatef(1.0, 0.2, 4.0);
				glTranslatef(1.0, 0.2, 4.0);
				glScalef(5, 5, 5);
				glTranslatef(-1.0, -0.2, -4.0);
				glBindTexture(GL_TEXTURE_2D, houseTexture);
				glmDraw(house, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();

			glPopMatrix();
		}

		//enable depth testing
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);


		glStencilFunc(GL_ALWAYS, 1, 0);
		glClear(GL_STENCIL_BUFFER_BIT);

		material_1_ambient[0] = 0.3;
		material_1_ambient[1] = 0.3;
		material_1_ambient[2] = 0.3;

		material_1_diffuse[0] = 1.0;
		material_1_diffuse[1] = 1.0;
		material_1_diffuse[2] = 1.0;

		material_1_specular[0] = 1.0;
		material_1_specular[1] = 1.0;
		material_1_specular[2] = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_1_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_1_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_1_specular);

		//place the tree on the negative Y plane of the skybox
		//glTranslatef(0.0, -fSkyboxSizeY + fTreeSize, 0.0);
		glPushMatrix();
		glColor3f(1, 0, 0);
		glTranslatef(mylight_position[0], mylight_position[1], mylight_position[2]);
		glutSolidSphere(1.0, 10, 10);
		glPopMatrix();

		glPopMatrix();

		glPushMatrix();
		glTranslatef(2.8, -fSkyboxSizeY + fTreeSize, -6.0);
		DrawSingleTreeTexture(fTreeSize);
		glRotatef(45.0, 0, 1, 0);
		DrawSingleTreeTexture(fTreeSize);
		glRotatef(45.0, 0, 1, 0);
		DrawSingleTreeTexture(fTreeSize);
		glRotatef(45.0, 0, 1, 0);
		DrawSingleTreeTexture(fTreeSize);
		glRotatef(45.0, 0, 1, 0);
		DrawSingleTreeTexture(fTreeSize);
		glPopMatrix();

		drawTrees();

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();



		glPushMatrix();
		glTranslatef(1.0, 0.2, 4.0);

		glTranslatef(1.0, 0.2, 4.0);
		glScalef(5, 5, 5);
		glTranslatef(-1.0, -0.2, -4.0);
		//glTranslatef(-2.0, 5.0, 0);

		glBindTexture(GL_TEXTURE_2D, houseTexture);
		glmDraw(house, GLM_SMOOTH | GLM_TEXTURE);

		//in_house(pos_x, pos_y, pos_z);

		glPopMatrix();

		/*glPushMatrix();

		glTranslatef(-3.0, 0.2, -13.0);
		glScalef(7.5, 6, 9);
		glutWireCube(1.0);
		glTranslatef(3.0, -0.2, 13.0);

		glPopMatrix();*/

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
	else{///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		gluLookAt(0.0, -fSkyboxSizeY + 1.2, 5.0, 0.0, -fSkyboxSizeY + 1.2, 0.0, 0.0, 1.0, 0.0);

		glMultMatrixf(rot_matrix);
		switch (scene_mode){
		case 0: //wireframe
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 1: //smooth
			glEnable(GL_SMOOTH);
			glShadeModel(GL_SMOOTH);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 2: //solid
			glEnable(GL_FLAT);
			glShadeModel(GL_FLAT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 3: //poligonal
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		default:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
		if (fog == 1) {
			EnableFog();
		}
		else {
			glDisable(GL_FOG);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		if (rain == 1) {
			drawRain();
		}
		if (snow == 1) {
			drawSnow();
		}

		pos_x = 0;
		pos_y = -fSkyboxSizeY + 1.2;
		pos_z = 5.0;

		//set global rotation on the X,Y and Z axes
		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleX, 1.0, 0.0, 0.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleY, 0.0, 1.0, 0.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		glTranslatef(pos_x, pos_y, pos_z);
		glRotatef(fGlobalAngleZ, 0.0, 0.0, 1.0);
		glTranslatef(-pos_x, -pos_y, -pos_z);

		//translate
		glTranslatef(TransXYZ[0], TransXYZ[1], TransXYZ[2]);
		pos_x += TransXYZ[0];
		pos_y += TransXYZ[1];
		pos_z += TransXYZ[2];

		//scale
		glScalef(fScaleX, fScaleY, fScaleZ);
		pos_x *= fScaleX;
		pos_y *= fScaleY;
		pos_z *= fScaleZ;

		

		glPushMatrix();
		glTranslatef(-4.0, -0.6, -14.8); //-16.8
		DrawRoom(fRoomSizeX, fRoomSizeY, fRoomSizeZ);
		glPopMatrix();

		if (shadow == 1){
			ComputePlaneEquations();
			ComputeShadowMatrices();

			glStencilFunc(GL_EQUAL, 1, 1);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);

			glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
			glMultMatrixf(fvFloorShadowMat);

			glPushMatrix();

			//glTranslatef(bec_x, bec_y, bec_z);

			glTranslatef(-1.5, -5.3, -20.0);
			glRotatef(-90, 0, 1, 0);
			glScalef(3.0, 3.0, 3.0);

			glBindTexture(GL_TEXTURE_2D, couchTexture);
			glmDraw(couch, GLM_SMOOTH | GLM_TEXTURE);

			glTranslatef(-1.5, 5.3, 20.0);
			//glTranslatef(-bec_x, -bec_y, -bec_z); 
			//glTranslatef(4.0, 4.5, 11.0);
			glPopMatrix();

			glPushMatrix();
			//glTranslatef(-7.0, -5.6, -21.0);
			glTranslatef(-8.4, -4.6, -20.7);
			glScalef(2.0, 2.0, 2.0);
			glBindTexture(GL_TEXTURE_2D, lampTexture);
			glmDraw(lamp, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(8.4, 4.6, 20.7);
			//glTranslatef(7.0, 5.6, 21.0);
			glPopMatrix();

			glPushMatrix();

			glScalef(2.0, 2.0, 2.0);
			glRotatef(90, 0, 1, 0);
			glTranslatef(7.5, 2.5, -6.4);

			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, clockTexture);
			glPushMatrix();
			glTranslatef(0.0, -2.0, 0.0);
			glRotatef(-90.0, 0, 1, 0);
			glmDraw(clock_face, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();
			//glRotatef(180.0, 0, 1, 0);
			glPushMatrix();
			glRotatef(90.0, 0, 1, 0);

			glTranslatef(-0.07, -2.0, 0.01);
			glRotatef(rot_h, 1, 0, 0);
			glTranslatef(0.07, 2.0, -0.01);

			rot_h += 1.0;
			if (rot_h >= 360.0) {
				rot_h = 0.0;
			}
			glTranslatef(-0.07, -1.75, 0.01);
			glmDraw(clock_h, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();

			glPushMatrix();
			glRotatef(90.0, 0, 1, 0);

			glTranslatef(0.07, -2.0, 0.01);
			glRotatef(rot_m, 1, 0, 0);
			glTranslatef(-0.07, 2.0, -0.01);

			rot_m += 0.5;
			if (rot_m >= 360.0) {
				rot_m = 0.0;
			}
			glTranslatef(-0.07, -1.8, 0.01);
			glmDraw(clock_m, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();
			glPopMatrix();
			glPopMatrix();

			glPushMatrix();
			//glTranslatef(0, -1.65, 0);

			glTranslatef(sneaker_x, sneaker_y, sneaker_z);
			glRotatef(rot_sneaker_x, 1, 0, 0);
			glRotatef(rot_sneaker_y, 0, 1, 0);
			glRotatef(rot_sneaker_z, 0, 0, 1);
			//glScalef(0.5, 0.5, 0.5);
			glScalef(scale_sneaker_x, scale_sneaker_y, scale_sneaker_z);
			glBindTexture(GL_TEXTURE_2D, sneakerTexture);
			glmDraw(sneakers, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(-sneaker_x, -sneaker_y, -sneaker_z);

			glPopMatrix();

			glPopMatrix();







			glPushMatrix();
			glColor3f(0.0f, 0.0f, 0.0f);//set the shadow color
			glMultMatrixf(fvFrontShadowMat);

			glPushMatrix();

			//glTranslatef(bec_x, bec_y, bec_z);

			glTranslatef(-1.5, -5.3, -20.0);
			glRotatef(-90, 0, 1, 0);
			glScalef(3.0, 3.0, 3.0);

			glBindTexture(GL_TEXTURE_2D, couchTexture);
			glmDraw(couch, GLM_SMOOTH | GLM_TEXTURE);

			glTranslatef(-1.5, 5.3, 20.0);
			//glTranslatef(-bec_x, -bec_y, -bec_z); 
			//glTranslatef(4.0, 4.5, 11.0);
			glPopMatrix();

			glPushMatrix();
			//glTranslatef(-7.0, -5.6, -21.0);
			glTranslatef(-8.4, -4.6, -20.7);
			glScalef(2.0, 2.0, 2.0);
			glBindTexture(GL_TEXTURE_2D, lampTexture);
			glmDraw(lamp, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(8.4, 4.6, 20.7);
			//glTranslatef(7.0, 5.6, 21.0);
			glPopMatrix();

			glPushMatrix();

			glScalef(2.0, 2.0, 2.0);
			glRotatef(90, 0, 1, 0);
			glTranslatef(7.5, 2.5, -6.4);

			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, clockTexture);
			glPushMatrix();
			glTranslatef(0.0, -2.0, 0.0);
			glRotatef(-90.0, 0, 1, 0);
			glmDraw(clock_face, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();
			//glRotatef(180.0, 0, 1, 0);
			glPushMatrix();
			glRotatef(90.0, 0, 1, 0);

			glTranslatef(-0.07, -2.0, 0.01);
			glRotatef(rot_h, 1, 0, 0);
			glTranslatef(0.07, 2.0, -0.01);

			rot_h += 1.0;
			if (rot_h >= 360.0) {
				rot_h = 0.0;
			}
			glTranslatef(-0.07, -1.75, 0.01);
			glmDraw(clock_h, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();

			glPushMatrix();
			glRotatef(90.0, 0, 1, 0);

			glTranslatef(0.07, -2.0, 0.01);
			glRotatef(rot_m, 1, 0, 0);
			glTranslatef(-0.07, 2.0, -0.01);

			rot_m += 0.5;
			if (rot_m >= 360.0) {
				rot_m = 0.0;
			}
			glTranslatef(-0.07, -1.8, 0.01);
			glmDraw(clock_m, GLM_SMOOTH | GLM_TEXTURE);
			glPopMatrix();
			glPopMatrix();
			glPopMatrix();

			glPushMatrix();
			//glTranslatef(0, -1.65, 0);

			glTranslatef(sneaker_x, sneaker_y, sneaker_z);
			glRotatef(rot_sneaker_x, 1, 0, 0);
			glRotatef(rot_sneaker_y, 0, 1, 0);
			glRotatef(rot_sneaker_z, 0, 0, 1);
			//glScalef(0.5, 0.5, 0.5);
			glScalef(scale_sneaker_x, scale_sneaker_y, scale_sneaker_z);
			glBindTexture(GL_TEXTURE_2D, sneakerTexture);
			glmDraw(sneakers, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(-sneaker_x, -sneaker_y, -sneaker_z);

			glPopMatrix();

			glPopMatrix();
		}
		glEnable(GL_TEXTURE_2D);

		glPushMatrix();
		lamp_position[0] = bec_x;
		lamp_position[1] = bec_y;
		lamp_position[2] = bec_z;

		lamp_ambient[0] = r_light_color;
		lamp_ambient[1] = g_light_color;
		lamp_ambient[2] = b_light_color;

		glLightfv(GL_LIGHT1, GL_AMBIENT, lamp_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, mylight_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, mylight_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, lamp_position);

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
		// Fairly shiny spot
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 150.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glStencilFunc(GL_ALWAYS, 1, 0);
		glClear(GL_STENCIL_BUFFER_BIT);

		material_1_ambient[0] = 0.3;
		material_1_ambient[1] = 0.3;
		material_1_ambient[2] = 0.3;

		material_1_diffuse[0] = 1.0;
		material_1_diffuse[1] = 1.0;
		material_1_diffuse[2] = 1.0;

		material_1_specular[0] = 1.0;
		material_1_specular[1] = 1.0;
		material_1_specular[2] = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_1_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_1_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_1_specular);

		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_3_ambient);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_3_diffuse);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_3_specular);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_3_shininess);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_3_emission);

		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_no_emission);

		glColor3f(1, 1, 1);
		glTranslatef(bec_x, bec_y, bec_z);
		glutSolidSphere(0.5, 10, 10);
		glPopMatrix();

		

		glPushMatrix();

		//glTranslatef(bec_x, bec_y, bec_z);

		glTranslatef(-1.5, -5.3, -20.0);
		glRotatef(-90, 0, 1, 0);
		glScalef(3.0, 3.0, 3.0);

		glBindTexture(GL_TEXTURE_2D, couchTexture);
		glmDraw(couch, GLM_SMOOTH | GLM_TEXTURE);

		glTranslatef(-1.5, 5.3, 20.0);
		//glTranslatef(-bec_x, -bec_y, -bec_z); 
		//glTranslatef(4.0, 4.5, 11.0);
		glPopMatrix();

		glPushMatrix();
			//glTranslatef(-7.0, -5.6, -21.0);
			glTranslatef(-8.4, -4.6, -20.7);
			glScalef(2.0, 2.0, 2.0);
			glBindTexture(GL_TEXTURE_2D, lampTexture);
			glmDraw(lamp, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(8.4, 4.6, 20.7);
			//glTranslatef(7.0, 5.6, 21.0);
		glPopMatrix();

		glPushMatrix();

		glScalef(2.0, 2.0, 2.0);
		glRotatef(90, 0, 1, 0);
		glTranslatef(7.5, 2.5, -6.4);

		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, clockTexture);
		glPushMatrix();
		glTranslatef(0.0, -2.0, 0.0);
		glRotatef(-90.0, 0, 1, 0);
		glmDraw(clock_face, GLM_SMOOTH | GLM_TEXTURE);
		glPopMatrix();
		//glRotatef(180.0, 0, 1, 0);
		glPushMatrix();
		glRotatef(90.0, 0, 1, 0);

		glTranslatef(-0.07, -2.0, 0.01);
		glRotatef(rot_h, 1, 0, 0);
		glTranslatef(0.07, 2.0, -0.01);

		rot_h += 1.0;
		if (rot_h >= 360.0) {
			rot_h = 0.0;
		}
		glTranslatef(-0.07, -1.75, 0.01);
		glmDraw(clock_h, GLM_SMOOTH | GLM_TEXTURE);
		glPopMatrix();

		glPushMatrix();
		glRotatef(90.0, 0, 1, 0);

		glTranslatef(0.07, -2.0, 0.01);
		glRotatef(rot_m, 1, 0, 0);
		glTranslatef(-0.07, 2.0, -0.01);

		rot_m += 0.5;
		if (rot_m >= 360.0) {
			rot_m = 0.0;
		}
		glTranslatef(-0.07, -1.8, 0.01);
		glmDraw(clock_m, GLM_SMOOTH | GLM_TEXTURE);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			//glTranslatef(0, -1.65, 0);
			
			glTranslatef(sneaker_x, sneaker_y, sneaker_z);
			glRotatef(rot_sneaker_x, 1, 0, 0);
			glRotatef(rot_sneaker_y, 0, 1, 0);
			glRotatef(rot_sneaker_z, 0, 0, 1);
			//glScalef(0.5, 0.5, 0.5);
			glScalef(scale_sneaker_x, scale_sneaker_y, scale_sneaker_z);
			glBindTexture(GL_TEXTURE_2D, sneakerTexture);
			glmDraw(sneakers, GLM_SMOOTH | GLM_TEXTURE);
			glTranslatef(-sneaker_x, -sneaker_y, -sneaker_z);

		glPopMatrix();
	}
	glutSwapBuffers();
}

void changeSize(int w, int h)
{
	screen_width=w;
	screen_height=h;

	if(h == 0)
		h = 1;

	float ratio = 1.0*w/h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 50.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
}



void processNormalKeys(unsigned char key, int x, int y)
{
	
	switch(key)
	{
	case ';':
		if (anim == true) {
			Timer(pas);
		}
		break;
	case 27: //esc
		exit(1);
		break;
		//control the global Y rotation angle using 'a' and 'd'
	case 'a':
		fGlobalAngleY += 1;
		if (fGlobalAngleY >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleY = (GLint)fGlobalAngleY % 360;
		break;
	case 'd':
		fGlobalAngleY -= 1;
		if (fGlobalAngleY <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleY = (GLint)fGlobalAngleY % 360;
		break;
		//control the global X rotation angle using 'w' and 's'
	case 'w':
		fGlobalAngleX += 1;
		if (fGlobalAngleX >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
		break;
	case 's':
		fGlobalAngleX -= 1;
		if (fGlobalAngleX <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
		break;
		//control the global Z rotation angle using 'q' and 'e'
	case 'q':
		fGlobalAngleZ += 1;
		if (fGlobalAngleZ >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleZ = (GLint)fGlobalAngleZ % 360;
		break;
	case 'e':
		fGlobalAngleZ -= 1;
		if (fGlobalAngleZ <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleZ = (GLint)fGlobalAngleZ % 360;
		break;
	case 'j':
		fScaleX += 0.01;
		break;
	case 'i':
		fScaleX -= 0.01;
		break;
	case 'o':
		fScaleY += 0.01;
		break;
	case 'p':
		fScaleZ += 0.01;
		break;
	case 'k':
		fScaleY -= 0.01;
		break;
	case 'l':
		fScaleZ -= 0.01;
		break;
	case '4':
		//if (fTranslateX < 4.0) {
		//	fTranslateX += 0.1;
		//}
		//if (no_collision(pos_x + 0.1, pos_y, pos_z)) {
		TransXYZ[0] += 0.1;
		printf("x = %f\n", pos_x);
		//}
		break;
	case '1':
		/*if (fTranslateX > -4.0) {
		fTranslateX -= 0.1;
		}*/
		//if (no_collision(pos_x - 0.1, pos_y, pos_z)) {
		TransXYZ[0] -= 0.1;
		printf("x = %f\n", pos_x);
		//}
		break;
	case '5':
		/*if (fTranslateY < 0.8) {
		fTranslateY += 0.1;
		}*/
		//if (no_collision(pos_x, pos_y + 0.1, pos_z)) {
		TransXYZ[1] += 0.1;
		printf("y = %f\n", pos_y);
		//}
		break;
	case '2':
		/*if (fTranslateY > -1.0) {
		fTranslateY -= 0.1;
		}*/
		//if (no_collision(pos_x, pos_y - 0.1, pos_z)) {
		TransXYZ[1] -= 0.1;
		printf("y = %f\n", pos_y);
		//}
		break;
	case '6':
		/*if (fTranslateZ < 6.0) {
		fTranslateZ += 0.1;
		}*/
		//if (no_collision(pos_x, pos_y, pos_z + 0.1)) {
		TransXYZ[2] += 0.1;
		printf("z = %f\n", pos_z);
		//}
		break;
	case '3':
		/*if (fTranslateZ > -4.0) {
		fTranslateZ -= 0.1;
		}*/
		//if (no_collision(pos_x, pos_y, pos_z - 0.1)) {
		TransXYZ[2] -= 0.1;
		printf("z = %f\n", pos_z);
		//}
		break;
	}
	

}



int main(int argc, char* argv[])
{
	
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screen_width, screen_height);
	main_window = glutCreateWindow("My scene");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(special_keys);
	initOpenGL();

	GLUI *glui;
	glui = GLUI_Master.create_glui("GLUI", 0, 810, 0);
	/*Weather panel*/
	GLUI_Panel *weather_panel = glui->add_panel("Weather");
	glui->add_checkbox_to_panel(weather_panel, "Rain", &rain);
	glui->add_checkbox_to_panel(weather_panel, "Snow", &snow);
	glui->add_checkbox_to_panel(weather_panel, "Fog", &fog);

	glui->add_separator_to_panel(weather_panel);

	GLUI_Panel *scene_panel = glui->add_panel("Scene Mode");

	radio = glui->add_radiogroup_to_panel(scene_panel, &scene_mode, RADIO_BUTTONS_GROUP_ID);
	glui->add_radiobutton_to_group(radio, "Wireframe");
	glui->add_radiobutton_to_group(radio, "Smooth");
	glui->add_radiobutton_to_group(radio, "Solid");
	glui->add_radiobutton_to_group(radio, "Poligonal");

	glui->add_separator_to_panel(scene_panel);

	//glui->add_checkbox_to_panel(type_panel, "Wireframe", &wireframe);
	//GLUI_Spinner *segment_spinner = glui->add_spinner_to_panel(type_panel, "Segments:", GLUI_SPINNER_INT, &segments);
	//segment_spinner->set_int_limits(3, 60);

	GLUI_Panel *light_panel = glui->add_panel("Light");
	glui->add_checkbox_to_panel(light_panel, "Outdoor light", &light_value1, LIGHT_ID0, control_callback);
	glui->add_checkbox_to_panel(light_panel, "Indoor light", &light_value2, LIGHT_ID1, control_callback);
	

	GLUI_Spinner *rcolor = glui->add_spinner_to_panel(light_panel, "R:", GLUI_SPINNER_FLOAT, &r_light_color);
	GLUI_Spinner *gcolor = glui->add_spinner_to_panel(light_panel, "G:", GLUI_SPINNER_FLOAT, &g_light_color);
	GLUI_Spinner *bcolor = glui->add_spinner_to_panel(light_panel, "B:", GLUI_SPINNER_FLOAT, &b_light_color);

	GLUI_Spinner *lightx = glui->add_spinner_to_panel(light_panel, "X:", GLUI_SPINNER_FLOAT, &bec_x);
	GLUI_Spinner *lighty = glui->add_spinner_to_panel(light_panel, "Y:", GLUI_SPINNER_FLOAT, &bec_y);
	GLUI_Spinner *lightz = glui->add_spinner_to_panel(light_panel, "Z:", GLUI_SPINNER_FLOAT, &bec_z);

	rcolor->set_int_limits(0.0, 1.0);
	gcolor->set_int_limits(0.0, 1.0);
	bcolor->set_int_limits(0.0, 1.0);

	glui->add_checkbox_to_panel(light_panel, "Shadow", &shadow);

	GLUI_Panel *rt_panel = glui->add_panel("Rotation and translation");
	GLUI_Translation *trans = glui->add_translation_to_panel(rt_panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0]);
	GLUI_Rotation *rot = glui->add_rotation_to_panel(rt_panel, "Rotation", (float *)rot_matrix);
	
	//glui->add_separator_to_panel(rt_panel);
	glui->set_main_gfx_window(main_window);
	GLUI_Master.set_glutIdleFunc(myGlutIdleFunction);
	
	glutMainLoop();
}