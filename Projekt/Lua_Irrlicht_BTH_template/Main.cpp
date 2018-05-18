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
static int camera(lua_State* L);
static int snapshot(lua_State* L);

irr::scene::IAnimatedMeshSceneNode* node;
irr::core::vector3df cameraPosition;
irr::core::vector3df cameraTarget;

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

	// create device
	MyEventReceiver receiver;

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	irr::scene::ISceneManager* smgr		= device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	//Model
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
		node->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	}
	
	irr::scene::ICameraSceneNode *cam = smgr->addCameraSceneNodeFPS();

	device->getCursorControl()->setVisible(false);

	/*--------------------------------------------------------------------*/
	lua_register(L, "addMesh", addMesh);
	lua_register(L, "addBox", addBox);
	lua_register(L, "getNodes", getNodes);
	lua_register(L, "camera", camera);
	lua_register(L, "snapshot", snapshot);
	/*--------------------------------------------------------------------*/


	while (device->run())
	{
		//-------------Input Receiver Check-----------
		if (device->isWindowActive()) {
			cam->setInputReceiverEnabled(true);
		}
		else {
			cam->setInputReceiverEnabled(false);
			cam->setPosition(cameraPosition);
			cam->setTarget(cameraTarget);
		}
		/*------------------------------------------*/

		driver->beginScene(true, true, video::SColor(255, 113, 113, 133));

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

static int camera(lua_State* L) {
	luaL_argcheck(L, lua_istable(L, 1), -1, "Error position table");

	lua_rawgeti(L, 1, 1);
	lua_rawgeti(L, 1, 2);
	lua_rawgeti(L, 1, 3);

	lua_rawgeti(L, 2, 1);
	lua_rawgeti(L, 2, 2);
	lua_rawgeti(L, 2, 3);

	cameraPosition = irr::core::vector3df(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	cameraTarget = irr::core::vector3df(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));

	for (int i = 3; i < 9; i++) {
		std::cout << i << ": " << lua_tonumber(L, i) << std::endl;
	}

	return 0;
}

static int snapshot(lua_State* L) {
	return 0;
}