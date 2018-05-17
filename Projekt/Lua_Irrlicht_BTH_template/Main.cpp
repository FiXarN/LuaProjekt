#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

#include <lua.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>
#include "lua.hpp"
#include <irrlicht.h>
#include "MyEventReceiver.h"

static int addMesh(lua_State* L);
static int addBox(lua_State* L);
static int getNodes(lua_State* L);

irr::scene::IAnimatedMeshSceneNode* node;

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if( luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0) )
			std::cout << lua_tostring(L, -1) << '\n';
		lua_pop(L, 1);
	}
}

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

		std::thread conThread(ConsoleThread, L);


	// ask user for driver
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	// create device
	MyEventReceiver receiver;

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	if(!device)
		return 1;


	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	irr::scene::ISceneManager* smgr		= device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	// Sphere node that will be moved with WASD
	//scene::ISceneNode * node = smgr->addSphereSceneNode();
	//if (node)
	//{
	//	node->setPosition(core::vector3df(0, 0, 30));
	//	node->setMaterialTexture(0, driver->getTexture("../../media/wall.bmp"));
	//	node->setMaterialFlag(video::EMF_LIGHTING, false);
	//}

	//// Cube scene node and an attached "fly circle"
	//scene::ISceneNode* n = smgr->addCubeSceneNode();

	//if (n)
	//{
	//	n->setMaterialTexture(0, driver->getTexture("../../media/t351sml.jpg"));
	//	n->setMaterialFlag(video::EMF_LIGHTING, false);
	//	scene::ISceneNodeAnimator* anim =
	//		smgr->createFlyCircleAnimator(core::vector3df(0, 0, 30), 20.0f);
	//	if (anim)
	//	{
	//		n->addAnimator(anim);
	//		anim->drop();
	//	}
	//}

	//Model 2
	irr::scene::IAnimatedMesh* mesh = smgr->getMesh("../../Bin/Meshes/sydney.md2");

	if (!mesh) {
		device->drop();
		return 1;
	}

	node = smgr->addAnimatedMeshSceneNode(mesh);

	if (node) {
		node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		node->setMD2Animation(irr::scene::EMAT_STAND);
		node->setMaterialTexture(0, driver->getTexture("../../Bin/Meshes/sydney.bmp"));
	}
	
	scene::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS();
	

	device->getCursorControl()->setVisible(false);



	/*--------------------------------------------------------------------*/
	lua_register(L, "addMesh", addMesh);
	lua_register(L, "addBox", addBox);
	lua_register(L, "getNodes", getNodes);


	int lastFPS = -1;
	irr::u32 then = device->getTimer()->getTime();
	irr::f32 MOVEMENT_SPEED = 0.2f;
	/*--------------------------------------------------------------------*/


	while (device->run())
	{
		/*-------------------F P S------------------*/
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.0f;
		then = now;

		// Input Receiver Check
		if (device->isWindowActive()) {
			camera->setInputReceiverEnabled(true);
		}
		else {
			camera->setInputReceiverEnabled(false);
		}
		/*------------------------------------------*/

		/*--------------M O V E M E N T-------------*/
		core::vector3df nodePosition = node->getPosition();

		if (receiver.IsKeyDown(irr::KEY_KEY_W))
			nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
		else if (receiver.IsKeyDown(irr::KEY_KEY_S))
			nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;

		if (receiver.IsKeyDown(irr::KEY_KEY_A))
			nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
		else if (receiver.IsKeyDown(irr::KEY_KEY_D))
			nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;

		node->setPosition(nodePosition);

		driver->beginScene(true, true, video::SColor(255, 113, 113, 133));

		smgr->drawAll(); // draw the 3d scene
		device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)

		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw tmp(L"Movement Example - Irrlicht Engine [");
			tmp += driver->getName();
			tmp += L"] fps: ";
			tmp += fps;

			device->setWindowCaption(tmp.c_str());
			lastFPS = fps;
		}
		/*------------------------------------------*/

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();		
	}

	device->drop();

	conThread.join();
	return 0;
}

static int addMesh(lua_State* L) {
	return 0;
}

static int addBox(lua_State* L) {
	return 0;
}

static int getNodes(lua_State* L) {
	return 0;
}