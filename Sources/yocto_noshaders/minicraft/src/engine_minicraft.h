#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"
#include "boid.h"

class MEngineMinicraft : public YEngine
{

	MAvatar * Avatar;
	MWorld * World;
	int Seed = 345645;

	YVbo * VboCube = NULL;

	GLuint ShaderWorld = 0;
	GLuint ShaderSun = 0;
	GLuint ShaderCube = 0;
	GLuint ShaderCubeDebug = 0;
	GLuint ShaderPostProcess = 0;
	GLuint ShaderBoids = 0;

	YFbo * FboReflections;
	YFbo * FboShadows;
	YFbo * FboPostProcess;

	YCamera CamLight;
	Mat44 VLight;
	Mat44 PLight;

	float WaterHeight;

	typedef enum
	{
		PASS_SHADOW,
		PASS_REFLECT,
		PASS_FINAL
	}PASS;

	PASS pass = PASS_SHADOW;

	YVec3f SunPosition;
	YVec3f SunDirection;
	YColor SunColor;
	YColor SkyColor;
	float BoostTime = 0.0f;
	bool BoostingTime = false;

	MBoidManager * Boids;

public:
	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		ShaderWorld = Renderer->createProgram("shaders/world");
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderCube = Renderer->createProgram("shaders/cube");
		ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");
		ShaderPostProcess = Renderer->createProgram("shaders/postprocess");
		ShaderBoids = Renderer->createProgram("shaders/boids");
	}

	void init()
	{
		YLog::log(YLog::ENGINE_INFO, "Minicraft Started : initialisation");

		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(3)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(2)); //UV

		VboCube->createVboCpu();
		fillVBOCube(VboCube, MCube::CUBE_SIZE);
		VboCube->createVboGpu();
		VboCube->deleteVboCpu();

		FboReflections = new YFbo(false, 1, 2);
		FboShadows = new YFbo(true, 1, 1);
		FboPostProcess = new YFbo();

		World = new MWorld();
		Avatar = new MAvatar(Renderer->Camera, World);

		World->setPerlinZDecay(MWorld::MAT_HEIGHT_CUBES - 5, 0.5f);
		World->init_world(Seed);
		WaterHeight = 59;

		Renderer->setBackgroundColor(YColor(0.7f, 0.8f, 1.f, 1.f));
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f(0, 0, 10));

		CamLight.setProjectionOrtho(-MWorld::MAT_SIZE_METERS / 1.2, MWorld::MAT_SIZE_METERS / 1.2,
			-MWorld::MAT_SIZE_METERS / 1.2, MWorld::MAT_SIZE_METERS / 1.2,
			-MWorld::MAT_HEIGHT_METERS / 1.1, MWorld::MAT_HEIGHT_METERS / 1.1);

		Boids = new MBoidManager(World, Avatar);
	}

	void update(float elapsed)
	{
		updateLights(elapsed);
		Avatar->update(elapsed);
		Avatar->Run = GetKeyState(VK_LSHIFT) & 0x80;
		Renderer->Camera->moveTo(Avatar->Position + YVec3f(0, 0, Avatar->CurrentHeight / 2));
		Boids->updateBoids(elapsed);
	}

	YVec3f symetry(float zplane, YVec3f pos) {
		pos.Z -= (pos.Z - zplane) * 2;
		return pos;
	}

	void renderObjects()
	{
		//Passe ombres
		FboShadows->setAsOutFBO(true);

		CamLight.setPosition(YVec3f(MWorld::MAT_SIZE_METERS / 2.0f, MWorld::MAT_SIZE_METERS / 2.0f, MWorld::MAT_HEIGHT_METERS / 2.0f));
		CamLight.setLookAt(CamLight.Position - (SunDirection));
		CamLight.look();
		Renderer->updateMatricesFromOgl(&CamLight);

		VLight = Renderer->MatV;
		PLight = Renderer->MatP;

		renderScene(PASS::PASS_SHADOW);

		//Passe reflections
		FboReflections->setAsOutFBO(true);
		glEnable(GL_CLIP_DISTANCE0);
		YVec3f realcamPos = Renderer->Camera->Position;
		YVec3f realcamLookAt = Renderer->Camera->LookAt;

		Renderer->Camera->setPosition(symetry(WaterHeight, realcamPos));
		Renderer->Camera->setLookAt(symetry(WaterHeight, realcamLookAt));
		Renderer->Camera->look();
		Renderer->updateMatricesFromOgl();

		renderScene(PASS::PASS_REFLECT);

		Renderer->Camera->setPosition(realcamPos);
		Renderer->Camera->setLookAt(realcamLookAt);

		//Passe de rendu final
		FboPostProcess->setAsOutFBO(true);
		Renderer->Camera->look();
		renderScene(PASS::PASS_FINAL);

		//Passe de post process
		FboPostProcess->setAsOutFBO(false);

		glUseProgram(ShaderPostProcess);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		FboPostProcess->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
		FboPostProcess->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");

		GLint var = glGetUniformLocation(ShaderPostProcess, "sunPos");
		glUniform3f(var, SunPosition.X, SunPosition.Y, SunPosition.Z);

		var = glGetUniformLocation(ShaderPostProcess, "skyColor");
		glUniform3f(var, SkyColor.R, SkyColor.V, SkyColor.B);

		Renderer->sendNearFarToShader(ShaderPostProcess);
		Renderer->sendScreenSizeToShader(ShaderPostProcess);
		Renderer->sendMatricesToShader(ShaderPostProcess);
		Renderer->drawFullScreenQuad();

	}

	void renderScene(PASS pass) {

		glUseProgram(0);
		GLuint shader = 0;

		if (pass == PASS::PASS_FINAL) {
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

		//Rendu du world
		if (pass != PASS::PASS_SHADOW) {
			glUseProgram(ShaderWorld);
			shader = ShaderWorld;

			FboShadows->setDepthAsShaderInput(GL_TEXTURE0, "TexShadow");
			if (pass != PASS::PASS_REFLECT)
				FboReflections->setColorAsShaderInput(0, GL_TEXTURE1, "TexReflect");

			GLint var = glGetUniformLocation(ShaderWorld, "V_light");
			glUniformMatrix4fv(var, 1, true, VLight.Mat.t);
			var = glGetUniformLocation(ShaderWorld, "P_light");
			glUniformMatrix4fv(var, 1, true, PLight.Mat.t);

			var = glGetUniformLocation(ShaderWorld, "reflexionMapPass");
			glUniform1i(var, pass == PASS_REFLECT ? 1 : 0);

			var = glGetUniformLocation(ShaderWorld, "water_height");
			glUniform1f(var, WaterHeight);

			var = glGetUniformLocation(ShaderWorld, "lightDir");
			glUniform3f(var, SunDirection.X, SunDirection.Y, SunDirection.Z);

			var = glGetUniformLocation(ShaderWorld, "camPos");
			glUniform3f(var, Renderer->Camera->Position.X, Renderer->Camera->Position.Y, Renderer->Camera->Position.Z);

			var = glGetUniformLocation(ShaderWorld, "sunColor");
			glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);

			var = glGetUniformLocation(ShaderWorld, "skyColor");
			glUniform3f(var, SkyColor.R, SkyColor.V, SkyColor.B);

			var = glGetUniformLocation(ShaderWorld, "world_size");
			glUniform1f(var, MWorld::MAT_SIZE_METERS);


			Renderer->updateMatricesFromOgl();
			Renderer->sendScreenSizeToShader(ShaderWorld);
			Renderer->sendMatricesToShader(ShaderWorld);
			this->sendTimeToShader(ShaderWorld);
			this->sendAllTexToShader(ShaderWorld);
		}

		bool drawTransparent = true;
		if (pass == PASS::PASS_REFLECT)
			drawTransparent = false;

		World->render_world_vbo(shader, false, drawTransparent);

		if (pass != PASS::PASS_SHADOW) {
			glPushMatrix();
			glUseProgram(ShaderSun);
			GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
			glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);
			glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
			glScalef(10, 10, 10);
			Renderer->updateMatricesFromOgl();
			Renderer->sendMatricesToShader(ShaderSun);
			VboCube->render();
			glPopMatrix();
		}

		glUseProgram(ShaderBoids);
		Boids->render(ShaderBoids,VboCube);
	}

	void resize(int width, int height) {
		FboReflections->resize(width, height);
		FboShadows->resize(width, height);
		FboPostProcess->resize(width, height);
	}

	/*FONCTION MINECRAFT*/

	//On commence par le point en UV 0,0 et on tourne en CCW
	int addQuadToVbo(YVbo * vbo, int iVertice, YVec3f & a, YVec3f & b, YVec3f & c, YVec3f & d) {
		YVec3f normal = (b - a).cross(d - a);
		normal.normalize();

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 1);

		iVertice++;

		return 6;

	}

	void fillVBOCube(YVbo * vbo, float size = 5.0f)
	{
		int iVertice = 0;

		YVec3f a(size / 2.0f, -size / 2.0f, -size / 2.0f);
		YVec3f b(size / 2.0f, size / 2.0f, -size / 2.0f);
		YVec3f c(size / 2.0f, size / 2.0f, size / 2.0f);
		YVec3f d(size / 2.0f, -size / 2.0f, size / 2.0f);
		YVec3f e(-size / 2.0f, -size / 2.0f, -size / 2.0f);
		YVec3f f(-size / 2.0f, size / 2.0f, -size / 2.0f);
		YVec3f g(-size / 2.0f, size / 2.0f, size / 2.0f);
		YVec3f h(-size / 2.0f, -size / 2.0f, size / 2.0f);

		iVertice += addQuadToVbo(vbo, iVertice, a, b, c, d); //x+
		iVertice += addQuadToVbo(vbo, iVertice, f, e, h, g); //x-
		iVertice += addQuadToVbo(vbo, iVertice, b, f, g, c); //y+
		iVertice += addQuadToVbo(vbo, iVertice, e, a, d, h); //y-
		iVertice += addQuadToVbo(vbo, iVertice, c, g, h, d); //z+
		iVertice += addQuadToVbo(vbo, iVertice, e, f, b, a); //z-
	}

	bool getSunDirFromDayTime(YVec3f & sunDir, float mnLever, float mnCoucher, float boostTime)
	{
		bool nuit = false;

		SYSTEMTIME t;
		GetLocalTime(&t);

		//On borne le tweak time à une journée (cyclique)
		while (boostTime > 24 * 60)
			boostTime -= 24 * 60;

		//Temps écoulé depuis le début de la journée
		float fTime = (float)(t.wHour * 60 + t.wMinute);
		fTime += boostTime;
		while (fTime > 24 * 60)
			fTime -= 24 * 60;

		//Si c'est la nuit
		if (fTime < mnLever || fTime > mnCoucher) {
			nuit = true;
			if (fTime < mnLever)
				fTime += 24 * 60;
			fTime -= mnCoucher;
			fTime /= (mnLever + 24 * 60 - mnCoucher);
			fTime *= (float)M_PI;
		} else {
			//c'est le jour
			nuit = false;
			fTime -= mnLever;
			fTime /= (mnCoucher - mnLever);
			fTime *= (float)M_PI;
		}

		//Direction du soleil en fonction de l'heure
		sunDir.X = cos(fTime);
		sunDir.Y = 0.2f;
		sunDir.Z = sin(fTime);
		sunDir.normalize();

		return nuit;
	}

	void updateLights(float elapsed) {

		if (BoostingTime)
			BoostTime += elapsed * 60.0f;

		//On recup la direciton du soleil
		bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, BoostTime);
		SunPosition = Renderer->Camera->Position + SunDirection * 500.0f;

		//Pendant la journée
		if (!nuit) {
			//On definit la couleur
			SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
			SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
			YColor downColor(0.9f, 0.5f, 0.1f, 1);

			SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
			SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
		} else {
			//La nuit : lune blanche et ciel noir
			SunColor = YColor(1, 1, 1, 1);
			SkyColor = YColor(0, 0, 0, 1);
		}

		Renderer->setBackgroundColor(SkyColor);
	}


	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2) {
		if (key == 'z')
			Avatar->avance = down;
		if (key == 's')
			Avatar->recule = down;
		if (key == 'q')
			Avatar->gauche = down;
		if (key == 'd')
			Avatar->droite = down;
		if (key == ' ')
			Avatar->Jump = down;
		if (key == 'g')
			BoostingTime = down;
		if (key == 'e' && !down) {
			int xC, yC, zC;
			YVec3f inter;
			World->getRayCollision(Renderer->Camera->Position,
				Renderer->Camera->Position + Renderer->Camera->Direction * 30,
				inter, xC, yC, zC);
			World->deleteCube(xC, yC, zC);
		}

		if (key == 'v' && down)
			Boids->setNewTargetBoids();

		if (key == 'b' && down)
			Boids->createBoids();

		if (key == 'n' && down)
			Boids->goToNestBoids();

	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		Renderer->Camera->move(Renderer->Camera->Direction * 10.0f * dir);
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		}
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		static int lastx = -1;
		static int lasty = -1;

		if (!pressed) {
			lastx = x;
			lasty = y;
			showMouse(true);
		} else {
			if (lastx == -1 && lasty == -1) {
				lastx = x;
				lasty = y;
			}

			int dx = x - lastx;
			int dy = y - lasty;

			if (dx == 0 && dy == 0)
				return;

			lastx = x;
			lasty = y;

			if (MouseBtnState & GUI_MRBUTTON) {
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80) {
					Renderer->Camera->rotateAround((float)-dx / 300.0f);
					Renderer->Camera->rotateUpAround((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				} else {
					showMouse(false);
					Renderer->Camera->rotate((float)-dx / 300.0f);
					Renderer->Camera->rotateUp((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}

			if (MouseBtnState & GUI_MMBUTTON) {
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80) {
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 2.0f;

					YVec3f avance = Renderer->Camera->Direction;
					avance.Z = 0;
					avance.normalize();
					avance *= (float)dy / 2.0f;

					Renderer->Camera->move(avance + strafe);
				} else {
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 5.0f;

					Renderer->Camera->move(Renderer->Camera->UpRef * (float)dy / 5.0f);
					Renderer->Camera->move(strafe);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}
		}
	}
};


#endif