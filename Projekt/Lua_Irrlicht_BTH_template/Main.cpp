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

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	if(!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	irr::scene::ISceneManager* smgr		= device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);


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

	smgr->addCameraSceneNode(0, irr::core::vector3df(0, 30, -40), irr::core::vector3df(0, 5, 0));


	/*--------------------------------------------------------------------*/
	lua_register(L, "addMesh", addMesh);
	lua_register(L, "addBox", addBox);
	lua_register(L, "getNodes", getNodes);
	/*--------------------------------------------------------------------*/



	while(device->run()) {
		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

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