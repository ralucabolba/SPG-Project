/////////////////////////////////////////////////////////////////////////////////////
//																				   //
//	Demo application for texture mapping										   //	
//																				   //	
//	This application uses texture mapping to draw a tree surrounded by a landscape //
//																				   //	
//	The landscape is created using a "skybox"									   //
//																				   //
//	Use W A S D keys to rotate the camera										   //	
//																				   //
/////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include "stdafx.h"
#include "glm.h"
#include "tga.h"
#include "glut.h"

#define MAX_PARTICLES 1000
#define RAIN	0
#define SNOW	1
#define NONE	2



float slowdown = 2.0;
float velocity = 0.0;
float zoom = -15.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;
float xposs[25] ;
float zposs[25] ;

float xpos, zpos;

//bool first = true;
bool fog = false;
bool bulb = false;
bool sun = true;
bool anim = true;

int loop;
int fall = NONE;

int screen_width = 1366;
int screen_height = 768;
int pas = 0;

GLclampf fogColor[] = { 1.0, 1.0, 1.0 };

GLuint treeTexture; //variable used for the ID of the tree texture
GLuint skyboxTexture[6]; //variable used for the IDs of the six textures that form the skybox 
GLfloat fSkyboxSizeX, fSkyboxSizeY, fSkyboxSizeZ; //skybox size on X, Y and Z axes
GLfloat fTreeSize;

GLfloat fGlobalAngleX, fGlobalAngleY, fGlobalAngleZ, 
		fTranslateX, fTranslateY, fTranslateZ,
		fScaleX = 1.0, fScaleY = 1.0, fScaleZ = 1.0,
		bec_x = 0.8, bec_y = -2.7, bec_z = 0.55; //global rotation angles, translation and scale factors

GLfloat pos_x, pos_y, pos_z;
GLfloat rot_h, rot_m;

//GLMmodel *bench;
//GLuint aaaTex;

GLMmodel *couch;
GLMmodel *lamp;
GLMmodel *house;
GLMmodel *tree;
GLMmodel *clock_face;
GLMmodel *clock_h;
GLMmodel *clock_m;
GLMmodel *sneakers;

GLuint couchTexture;
GLuint lampTexture;
GLuint houseTexture;
GLuint bradTexture;
GLuint clockTexture;
GLuint sneakerTexture;

GLfloat mylight_ambient[] = { 1, 1, 1, 1 };
GLfloat mylight_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mylight_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mylight_position[] = { 0, 3.0, 0, 1 };


GLfloat lamp_ambient[] = { 0.1, 0.1, 0.1, 1 };
GLfloat lamp_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lamp_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lamp_position[] = { 0.8, -2.7, 0.55, 1 };

GLfloat material_3_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat material_3_diffuse[] = { 0.75164f, 0.60648f, 0.60648f, 1.0f };
GLfloat material_3_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat material_3_shininess[] = { 0.4f };
GLfloat material_3_emission[] = { 1.0, 1.0, 0.0, 1.0 };GLfloat material_no_emission[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat *vert;


/////////////////////////////////////////UNMBRE/////////////////////////////
//equations of the planes on which the shadow is cast
GLfloat fvFloorPlaneEq[4];
GLfloat fvLeftWallPlaneEq[4];
GLfloat fvRightWallPlaneEq[4];
GLfloat fvBackWallPlaneEq[4];
GLfloat fvCeilingPlaneEq[4];



//projected shadows matrices
GLfloat fvFloorShadowMat[16];
GLfloat fvLeftWallShadowMat[16];
GLfloat fvBackWallShadowMat[16];
GLfloat fvRightWallShadowMat[16];
GLfloat fvCeilingShadowMat[16];


/*GLfloat fvLightAmbient[4] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat fvLightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fvLightSpecular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fvLightPos[4] = { 2.0, 2.0, 2.0, 1.0 };

GLfloat fvTeapotMatAmbient[4] = { 0.7, 0.0, 0.0, 1.0 };
GLfloat fvTeapotMatDiffuse[4] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat fvTeapotMatSpecular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fvTeapotMatShininess[1] = { 32.0 };

GLfloat fvGenericMatAmbient[4] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat fvGenericMatDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fvGenericMatSpecular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fvGenericMatShininess[1] = { 64.0 };
*/
//forward declarations for functions
void PlaneEq(GLfloat plane[4], GLfloat p0[4], GLfloat p1[4], GLfloat p2[4]);
void ComputePlaneEquations();
void ComputeShadowMatrix(GLfloat shadowMat[16], GLfloat plane[4], GLfloat lightPos[4]);
void ComputeShadowMatrices();
//////////////////////////////////////////////////////////////////
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

void special_keys(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		bec_x += 0.1;
		printf("bec_x = %f\n", bec_x);
	}
	if (key == GLUT_KEY_DOWN) {
		bec_x -= 0.1;
		printf("bec_x = %f\n", bec_x); 
	}
	if (key == GLUT_KEY_RIGHT) {
		bec_y += 0.1;
		printf("bec_y = %f\n", bec_y);
	}
	if (key == GLUT_KEY_LEFT) {
		bec_y -= 0.1;
		printf("bec_y = %f\n", bec_y);
	}
	if (key == GLUT_KEY_PAGE_UP) {
		bec_z += 0.1;
		printf("bec_z = %f\n", bec_z);
	}
	if (key == GLUT_KEY_PAGE_DOWN) {
		bec_z -= 0.1;
		printf("bec_z = %f\n", bec_z);
	}
}

bool no_collision(GLfloat x, GLfloat y, GLfloat z) {
	if (!(y >= -2.8*fScaleY && y <= -1.0*fScaleY &&
		x >= -4.0*fScaleX && x <= 3.5*fScaleX &&
		z >= 6.0*fScaleZ && z <= 14.0*fScaleZ)) {

		return false;
	}
	vert = house->vertices;
	int i = 3;
	while (vert[i+2] != NULL) {
		if (vert[i] == x && vert[i + 1] == y && vert[i + 2] == z) {
			printf("Collision!!!");
			return false;
		}
		i+=3;
	}

	return true;
}

void initParticles(int i) {
	par_sys[i].alive = true;
	par_sys[i].life = 1.0;
	par_sys[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	par_sys[i].xpos = (float)(rand() % 21) - 8.0;
	par_sys[i].ypos = 5.0;
	par_sys[i].zpos = (float)(rand() % 21) + 2.0;

	par_sys[i].red = 0.5;
	par_sys[i].green = 0.5;
	par_sys[i].blue = 1.0;

	par_sys[i].vel = velocity;
	par_sys[i].gravity = -0.8;//-0.8;

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

void initOpenGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glViewport(0, 0, screen_width, screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 1.0f, 1000.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, mylight_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mylight_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mylight_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, mylight_position);

	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		initParticles(loop);
	}

	//generate new texture IDs and load each texture that will be used
	glGenTextures(1, &treeTexture);
	loadTGA("Textures\\pine.tga", treeTexture);
	
	glGenTextures(6, skyboxTexture);
	
	loadTGA("Textures\\hangingstone_rt.tga", skyboxTexture[0]);
	loadTGA("Textures\\hangingstone_dn.tga", skyboxTexture[1]);
	loadTGA("Textures\\hangingstone_bk.tga", skyboxTexture[2]);
	loadTGA("Textures\\hangingstone_lf.tga", skyboxTexture[3]);
	loadTGA("Textures\\hangingstone_up.tga", skyboxTexture[4]);
	loadTGA("Textures\\hangingstone_ft.tga", skyboxTexture[5]);
	
	//set skybox size
	fSkyboxSizeX = 5.0;
	fSkyboxSizeY = 3.0;
	fSkyboxSizeZ = 5.0;

	//set tree size
	fTreeSize = 1.0;

	InitModel(&couch, "sofa.obj");
	glGenTextures(1, &couchTexture);
	loadTGA("Textures\\sofa.tga", couchTexture);

	InitModel(&lamp, "lampa.obj");
	glGenTextures(1, &lampTexture);
	loadTGA("Textures\\lamp.tga", lampTexture);

	InitModel(&house, "house.obj");
	glGenTextures(1, &houseTexture);
	loadTGA("Textures\\house.tga", houseTexture);

	InitModel(&clock_face, "clock.obj");
	InitModel(&clock_h, "hour.obj");
	InitModel(&clock_m, "min2.obj");
	glGenTextures(1, &clockTexture);
	loadTGA("Textures\\clock.tga", clockTexture);


	InitModel(&sneakers, "tenisi.obj");
	glGenTextures(1, &sneakerTexture);
	loadTGA("Textures\\sneaker_texture.tga", sneakerTexture);

	ComputePlaneEquations();
	ComputeShadowMatrices();
}

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

void ComputePlaneEquations()
{
	//floor points
	GLfloat fvFloorP0[4] = { fSkyboxSizeX / 2, -fSkyboxSizeY / 2, fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvFloorP1[4] = { fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvFloorP2[4] = { -fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };

	//left wall points
	GLfloat fvLeftWallP0[4] = { -fSkyboxSizeX / 2, -fSkyboxSizeY / 2, fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP1[4] = { -fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvLeftWallP2[4] = { -fSkyboxSizeX / 2, fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };

	//Right wall points
	GLfloat fvRightWallP0[4] = { fSkyboxSizeX / 2, fSkyboxSizeY / 2, fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvRightWallP1[4] = { fSkyboxSizeX / 2, fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvRightWallP2[4] = { fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };

	//back wall points
	GLfloat fvBackWallP0[4] = { -fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvBackWallP1[4] = { fSkyboxSizeX / 2, -fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvBackWallP2[4] = { fSkyboxSizeX / 2, fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };

	//ceiling points
	GLfloat fvCeilingP0[4] = { -fSkyboxSizeX / 2, fSkyboxSizeY / 2, fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvCeilingP1[4] = { -fSkyboxSizeX / 2, fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };
	GLfloat fvCeilingP2[4] = { fSkyboxSizeX / 2, fSkyboxSizeY / 2, -fSkyboxSizeZ / 2, 1.0 };

	PlaneEq(fvFloorPlaneEq, fvFloorP0, fvFloorP1, fvFloorP2);
	PlaneEq(fvLeftWallPlaneEq, fvLeftWallP0, fvLeftWallP1, fvLeftWallP2);
	PlaneEq(fvRightWallPlaneEq, fvRightWallP0, fvRightWallP1, fvRightWallP2);
	PlaneEq(fvBackWallPlaneEq, fvBackWallP0, fvBackWallP1, fvBackWallP2);
	PlaneEq(fvCeilingPlaneEq, fvCeilingP0, fvCeilingP1, fvCeilingP2);



}

void ComputeShadowMatrices()
{
	ComputeShadowMatrix(fvFloorShadowMat, fvFloorPlaneEq, mylight_position);
	ComputeShadowMatrix(fvLeftWallShadowMat, fvLeftWallPlaneEq, mylight_position);
	ComputeShadowMatrix(fvRightWallShadowMat, fvRightWallPlaneEq, mylight_position);
	ComputeShadowMatrix(fvBackWallShadowMat, fvBackWallPlaneEq, mylight_position);
	ComputeShadowMatrix(fvCeilingShadowMat, fvCeilingPlaneEq, mylight_position);


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

void DrawSkybox(GLfloat sizeX, GLfloat sizeY, GLfloat sizeZ)
{
	glEnable(GL_TEXTURE_2D); //enable 2D texturing


	//////////////////////////////////////////////////////////////////
	// please consult the orientation convention for this skybox    //
	// ("orientation_convention.png" file in the "Textures" folder) //
	//////////////////////////////////////////////////////////////////


	//negative x plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[0]); //select the current texture
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-sizeX, sizeY, -sizeZ); //assign each corner of the texture to a 3D vertex in the OpenGL scene
	glTexCoord2f(0, 1); glVertex3f(-sizeX, sizeY, sizeZ); //(0,0) is the left lower corner, while (1,1) is the right upper corner of the texture
	glTexCoord2f(0, 0); glVertex3f(-sizeX, -sizeY, sizeZ);
	glTexCoord2f(1, 0); glVertex3f(-sizeX, -sizeY, -sizeZ);
	glEnd();

	//negative y plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeX, -sizeY, -sizeZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeX, -sizeY, -sizeZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeX, -sizeY, sizeZ);
	glTexCoord2f(1, 0); glVertex3f(sizeX, -sizeY, sizeZ);
	glEnd();

	//negative z plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(-sizeX, sizeY, sizeZ);
	glTexCoord2f(0, 1); glVertex3f(sizeX, sizeY, sizeZ);
	glTexCoord2f(0, 0); glVertex3f(sizeX, -sizeY, sizeZ);
	glTexCoord2f(1, 0); glVertex3f(-sizeX, -sizeY, sizeZ);
	glEnd();

	//positive x plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeX, sizeY, sizeZ);
	glTexCoord2f(0, 1); glVertex3f(sizeX, sizeY, -sizeZ);
	glTexCoord2f(0, 0); glVertex3f(sizeX, -sizeY, -sizeZ);
	glTexCoord2f(1, 0); glVertex3f(sizeX, -sizeY, sizeZ);
	glEnd();

	//positive y plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeX, sizeY, sizeZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeX, sizeY, sizeZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeX, sizeY, -sizeZ);
	glTexCoord2f(1, 0); glVertex3f(sizeX, sizeY, -sizeZ);
	glEnd();

	//positive z plane
	glBindTexture(GL_TEXTURE_2D, skyboxTexture[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex3f(sizeX, sizeY, -sizeZ);
	glTexCoord2f(0, 1); glVertex3f(-sizeX, sizeY, -sizeZ);
	glTexCoord2f(0, 0); glVertex3f(-sizeX, -sizeY, -sizeZ);
	glTexCoord2f(1, 0); glVertex3f(sizeX, -sizeY, -sizeZ);
	glEnd();

	glDisable(GL_TEXTURE_2D); //disable 2D texuring
}

void animation() {
	pas++;
	if (pas <= 10) {
		fTranslateZ += 0.1;
	}
	else if (pas >= 15 && pas <= 20) {
		fTranslateY += 0.1;
	}
	else if (pas >= 25 && pas <= 60) {
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 65 && pas <= 140)
	{
		fGlobalAngleY -= 1.0;
		if (fGlobalAngleY < -360) {
			fGlobalAngleY = 360;
		}
	}
	else if (pas >= 145 && pas <= 176) {
		fTranslateX += 0.1;
	}
	else if (pas >= 180 && pas <= 190) {
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 195 && pas <= 240) {
		fTranslateZ += 0.1;
	}
	else if (pas >= 245 && pas <= 260) {
		fTranslateY -= 0.1;
	}
	else if (pas >= 265 && pas <= 435) {
		fGlobalAngleY += 1.0;
		if (fGlobalAngleY > 360) {
			fGlobalAngleY = -360;
		}
	}
	else if (pas >= 440 && pas <= 450) {
		fGlobalAngleX += 1;
		if (fGlobalAngleX >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
	}
	else if (pas >= 455 && pas <= 480) {
		fTranslateZ -= 0.1;
	}
	else if (pas >= 485 && pas <= 515) {
		fGlobalAngleY -= 1;
		if (fGlobalAngleY <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleY = (GLint)fGlobalAngleY % 360;
	}
	else if (pas >= 520 && pas <= 565) {
		fTranslateX -= 0.1;
	}
	else if (pas >= 570 && pas <= 630) {
		fGlobalAngleY -= 1;
		if (fGlobalAngleY <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleY = (GLint)fGlobalAngleY % 360;
	}
	else if (pas >= 635 && pas <= 640) {
		fGlobalAngleX += 1;
		if (fGlobalAngleX >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
	}
	else if (pas >= 645 && pas <= 680) {
		fGlobalAngleY -= 1;
		if (fGlobalAngleY <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleY = (GLint)fGlobalAngleY % 360;
	}
	/*else if (pas >= 685 && pas <= 700) {
		fGlobalAngleX += 1;
		if (fGlobalAngleX >= 360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
	}*/
	else if (pas >= 685 && pas <= 690) {
		fGlobalAngleX -= 1;
		if (fGlobalAngleX <= -360) //clamp the rotation angle in the [0,360) interval
			fGlobalAngleX = (GLint)fGlobalAngleX % 360;
	}
	else if (pas >= 705 && pas <= 720) {
		fTranslateZ -= 0.1;
	}
	else if (pas >= 725 && pas <= 740) {
		fTranslateY += 0.1;
	}
	else if (pas >= 745 && pas <= 755) {
		fTranslateX += 0.1;
	}
	else if (pas >= 760 && pas <= 785) {
		fTranslateZ += 0.1;
	}
	else if (pas >= 800) {
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

	gluLookAt(0.0, -fSkyboxSizeY + 1.2, 6.0, 0.0, -fSkyboxSizeY + 1.2, -10.0, 0.0, 1.0, 0.0);

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
	glTranslatef(fTranslateX, fTranslateY, fTranslateZ);
	pos_x += fTranslateX;
	pos_y += fTranslateY;
	pos_z += fTranslateZ;

	//scale
	glScalef(fScaleX, fScaleY, fScaleZ);
	pos_x *= fScaleX;
	pos_y *= fScaleY;
	pos_z *= fScaleZ;

	/////////////////////////////PENTRU UMBRE//////////////////////////
	///////////////////////////////////////////////////////////////////

	//draw skybox
	glPushMatrix();
		DrawSkybox(fSkyboxSizeX, fSkyboxSizeY, fSkyboxSizeZ);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2.8, -fSkyboxSizeY + fTreeSize, 0);
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

	glPopMatrix();
	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	//enable lighting
	glEnable(GL_LIGHTING);

	if (sun == false) {
		glDisable(GL_LIGHT0);
	}
	else {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	

	glPushMatrix();

	if (fog == true) {
		EnableFog();
	}
	else {
		glDisable(GL_FOG);
	}
	if (fall == RAIN) {
		drawRain();
	}
	else if (fall == SNOW) {
		drawSnow();
	}
	else if (fall == NONE) {
		glPushMatrix();
		glColor3f(1.0, 1.0, 1.0);
		glPopMatrix();
	}
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		
		glTranslatef(-1.8, -5.55, 1.0);
		glScalef(0.5, 0.5, 0.5);
		glTranslatef(1.8, 5.55, -1.0);

		glBindTexture(GL_TEXTURE_2D, couchTexture);
		glmDraw(couch, GLM_SMOOTH | GLM_TEXTURE);
		
	glPopMatrix();

	glPushMatrix();
		
	if (bulb == true) {
		//glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT1);
		//glEnable(GL_NORMALIZE);

		glLightfv(GL_LIGHT1, GL_AMBIENT, lamp_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lamp_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lamp_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, lamp_position);

		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
		// Fairly shiny spot
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 100.0f);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_3_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_3_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_3_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_3_shininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_3_emission);
	}
	else {
		glDisable(GL_LIGHT1);
		//glDisable(GL_LIGHTING);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_no_emission);

	glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(bec_x, bec_y, bec_z);
		glutSolidSphere(0.025, 10, 10);
	glPopMatrix();

	glTranslatef(1.0, -3.45, 0.7);
		glScalef(0.2, 0.2, 0.2);
		glTranslatef(-1.0, 3.45, -0.7);

		//glDisable(GL_LIGHTING);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glColor4f(1, 1, 1, 0.5); //blending depends on the alpha value
		
		glBindTexture(GL_TEXTURE_2D, lampTexture);
		glmDraw(lamp, GLM_SMOOTH | GLM_TEXTURE);

		//glDisable(GL_BLEND);
		//glEnable(GL_LIGHTING);
		
		
		
	glPopMatrix();

	glPushMatrix();


		glTranslatef(0, -1.65, 0);
		glScalef(3,3,3);
		//glTranslatef(-2.0, 5.0, 0);

		glBindTexture(GL_TEXTURE_2D, houseTexture);
		glmDraw(house, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();

	//clock

	glScalef(0.5, 0.5, 0.5);
	glTranslatef(0.0, -1.0, -4.7);
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


	glPushMatrix();


		//glTranslatef(0, -1.65, 0);
		glScalef(0.5, 0.5, 0.5);
		glTranslatef(5.0, -9.5, 2.0);

		glBindTexture(GL_TEXTURE_2D, sneakerTexture);
		glmDraw(sneakers, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	/*glPushMatrix();


		glTranslatef(2, -2, 0);
		glScalef(2, 2, 2);
		//glTranslatef(-2.0, 5.0, 0);

		glBindTexture(GL_TEXTURE_2D, bradTexture);
		glmDraw(tree, GLM_SMOOTH | GLM_TEXTURE);

	glPopMatrix();
	*/
	glDisable(GL_TEXTURE_2D);



	glutSwapBuffers(); //swap the buffers used in the double-buffering technique
}

void changeSize(int w, int h)
{
	screen_width = w;
	screen_height = h;

	if (h == 0)
		h = 1;

	float ratio = 1.0*w / h;

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

	switch (key)
	{
	case ';':
		if (anim == true) {
			Timer(pas);
		}
		break;
	case 'f':
		fog = fog ^ 1;
		glutPostRedisplay();
		break;
	case 'b':

		bulb = bulb ^ 1;
		glutPostRedisplay();
		break;
	case 'v':

		sun = sun ^ 1;
		glutPostRedisplay();
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
	case 'r':
		fall = RAIN;
		glutPostRedisplay();
		break;
	case 'y':
		fall = SNOW;
		glutPostRedisplay();
		break;
	case 'n':
		fall = NONE;
		glutPostRedisplay();
		break;
	case '4':
		//if (fTranslateX < 4.0) {
		//	fTranslateX += 0.1;
		//}
		if (no_collision(pos_x + 0.1, pos_y, pos_z)) {
			fTranslateX += 0.1;
		}
		break;
	case '1':
		/*if (fTranslateX > -4.0) {
			fTranslateX -= 0.1;
		}*/
		if (no_collision(pos_x - 0.1, pos_y, pos_z)) {
			fTranslateX -= 0.1;
		}
		break;
	case '5':
		/*if (fTranslateY < 0.8) {
			fTranslateY += 0.1;
		}*/
		if (no_collision(pos_x , pos_y + 0.1, pos_z)) {
			fTranslateY += 0.1;
		}
		break;
	case '2':
		/*if (fTranslateY > -1.0) {
			fTranslateY -= 0.1;
		}*/
		if (no_collision(pos_x, pos_y - 0.1, pos_z)) {
			fTranslateY -= 0.1;
		}
		break;
	case '6':
		/*if (fTranslateZ < 6.0) {
			fTranslateZ += 0.1;
		}*/
		if (no_collision(pos_x, pos_y, pos_z + 0.1)) {
			fTranslateZ += 0.1;
		}
		break;
	case '3':
		/*if (fTranslateZ > -4.0) {
			fTranslateZ -= 0.1;
		}*/
		if (no_collision(pos_x, pos_y, pos_z - 0.1)) {
			fTranslateZ -= 0.1;
		}
		break;
	}

	//printf("x = %f\ny = %f\nz = %f\n_________________\n", pos_x, pos_y, pos_z);
}

int main(int argc, char* argv[])
{
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("First OpenGL Application");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	//glutKeyboardFunc(normal_keys);
	glutSpecialFunc(special_keys);
	initOpenGL();
	glutMainLoop();
	return 0;
}

