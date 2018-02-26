#include "Render.h"

void renderAxis(void)
{
	//Rendu des axes
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(10000, 0, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 10000, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 10000);
	glEnd();
}

//rendu soleil
void renderSun(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	//Rendu du soleil

	//On sauve la matrice
	glPushMatrix();

	//Position du soleil
	glTranslatef(g_renderer->_Camera->_Position.X, g_renderer->_Camera->_Position.Y, g_renderer->_Camera->_Position.Z);
	glTranslatef(g_sun_dir.X * 1000, g_sun_dir.Y * 1000, g_sun_dir.Z * 1000);

	//Material du soleil : de l'emissive
	GLfloat sunEmissionMaterial[] = { 0.0, 0.0, 0.0,1.0 };
	sunEmissionMaterial[0] = g_sun_color.R;
	sunEmissionMaterial[1] = g_sun_color.V;
	sunEmissionMaterial[2] = g_sun_color.B;
	glMaterialfv(GL_FRONT, GL_EMISSION, sunEmissionMaterial);

	//On dessine un cube pour le soleil
	glutSolidCube(50.0f);

	//On reset le material emissive pour la suite
	sunEmissionMaterial[0] = 0.0f;
	sunEmissionMaterial[1] = 0.0f;
	sunEmissionMaterial[2] = 0.0f;
	glMaterialfv(GL_FRONT, GL_EMISSION, sunEmissionMaterial);

	//Reset de la matrice
	glPopMatrix();

	
}

//ancien rendu cube / sphere
void renderMyObjects(void)
{
	//Materiau spéculaire, le meme pour tout le cube
	return;

	//juste apres le rendu des axes

/*shader
	glUseProgram(g_program);
	GLuint elap = glGetUniformLocation(g_program, "elapsed");
	glUniform1f(elap, NYRenderer::_DeltaTimeCumul);

	GLuint camPos = glGetUniformLocation(g_program, "cameraPosition");
	glUniform3f(camPos, g_renderer->_Camera->_Position.X,
		g_renderer->_Camera->_Position.Y,
		g_renderer->_Camera->_Position.Z);

	GLuint invView = glGetUniformLocation(g_program, "viewMatrix");
	glUniformMatrix4fv(invView, 1, false, g_renderer->_Camera->_InvertViewMatrix.Mat.t);

	*/

	//Rendu du Cube

	//On sauve la matrice
	glPushMatrix();

	//Rotation du cube en fonction du temps
	glRotatef(NYRenderer::_DeltaTimeCumul*50.0f, 0, 0, 1);
	glRotatef(NYRenderer::_DeltaTimeCumul*50.0f, 0, 1, 0);

	//Materiau spéculaire, le meme pour tout le cube
	GLfloat whiteSpecularMaterial[] = { 0.3, 0.3, 0.3,1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	GLfloat mShininess = 100;
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	//On debut les quads 
	glBegin(GL_QUADS);

	//On va grouper les faces par material

	//Face1
	GLfloat materialDiffuse[] = { 0, 0.7, 0,1.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	GLfloat materialAmbient[] = { 0, 0.2, 0,1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	glNormal3f(0, -1, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);

	//Face2
	glNormal3f(0, 1, 0);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);

	//Face3
	materialDiffuse[0] = 0.7f;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialAmbient[0] = 0.2f;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	glNormal3f(1, 0, 0);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);

	//Face4
	glNormal3f(-1, 0, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, 1, -1);

	//Face5
	materialDiffuse[0] = 0;
	materialDiffuse[1] = 0;
	materialDiffuse[2] = 0.7f;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialAmbient[0] = 0;
	materialAmbient[1] = 0;
	materialAmbient[2] = 0.2f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);

	//Face6
	glNormal3f(0, 0, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);

	//Fin des quads
	glEnd();

	//On recharge la matrice précédente
	glPopMatrix();

	//Sphère blanche transparente pour bien voir le shading et le reflet du soleil
	GLfloat whiteSpecularMaterialSphere[] = { 0.3, 0.3, 0.3,0.8 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterialSphere);
	mShininess = 100;
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);

	GLfloat materialDiffuseSphere[] = { 0.7, 0.7, 0.7,0.8 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuseSphere);
	GLfloat materialAmbientSphere[] = { 0.2, 0.2, 0.2,0.8 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbientSphere);

//glUseProgram(0);


	glutSolidSphere(2, 30, 30);

	
}
