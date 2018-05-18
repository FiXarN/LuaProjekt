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
#include <string>

static int addMesh(lua_State* L);
static int addBox(lua_State* L);
static int getNodes(lua_State* L);
static int cameraFunction(lua_State* L);
static int snapshot(lua_State* L);

irr::scene::IAnimatedMeshSceneNode* node;
irr::scene::ISceneNode * meshNode;
irr::scene::ISceneManager* smgr;

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

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	smgr = device->getSceneManager();
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
	
	scene::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS();

	device->getCursorControl()->setVisible(false);

	/*---------------AddBox----------------------*/
	meshNode = smgr->addCubeSceneNode();
	if (meshNode) {
		//meshNode->setPosition(pos);
		meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	/*-------------------------------------------*/

	/*--------------------------------------------------------------------*/
	lua_register(L, "addMesh", addMesh);
	lua_register(L, "addBox", addBox);
	lua_register(L, "getNodes", getNodes);
	lua_register(L, "camera", cameraFunction);
	lua_register(L, "snapshot", snapshot);
	/*--------------------------------------------------------------------*/


	while (device->run())
	{
		//-------------Input Receiver Check-----------
		if (device->isWindowActive()) {
			camera->setInputReceiverEnabled(true);
		}
		else {
			camera->setInputReceiverEnabled(false);
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

//Exempelinput
//addMesh({{-10.0, -10.0, 50.0}, {10.0, -10.0, 50.0}, {0.0, 10.0, 50.0}})
static int addMesh(lua_State* L) {
	luaL_argcheck(L, lua_istable(L, 1), -1, "Error input is not a table");

	lua_len(L, 1);
	int length = lua_tonumber(L, -1);
	luaL_argcheck(L, length % 3 == 0, -1, "Error: number of components in triangle list");
	lua_pop(L, 1);

	irr::core::vector3df triX;
	irr::core::vector3df triY;
	irr::core::vector3df triZ;

	//X
	lua_rawgeti(L, 1, 1);
	luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

	lua_len(L, 2);
	length = lua_tonumber(L, -1);
	luaL_argcheck(L, length == 3, -1, "Error: number of components in first vertex");
	lua_pop(L, 1);

	lua_rawgeti(L, 2, 1);
	luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
	triX.X = lua_tonumber(L, 3);

	lua_rawgeti(L, 2, 2);
	luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
	triX.Y = lua_tonumber(L, 4);

	lua_rawgeti(L, 2, 3);
	luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
	triX.Z = lua_tonumber(L, 5);

	lua_pop(L, 4);

	//Y
	lua_rawgeti(L, 1, 2);
	luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

	lua_len(L, 2);
	length = lua_tonumber(L, -1);
	luaL_argcheck(L, length == 3, -1, "Error: number of components in second vertex");
	lua_pop(L, 1);

	lua_rawgeti(L, 2, 1);
	luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
	triY.X = lua_tonumber(L, 3);

	lua_rawgeti(L, 2, 2);
	luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
	triY.Y = lua_tonumber(L, 4);

	lua_rawgeti(L, 2, 3);
	luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
	triY.Z = lua_tonumber(L, 5);

	lua_pop(L, 4);

	//Z
	lua_rawgeti(L, 1, 3);
	luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

	lua_len(L, 2);
	length = lua_tonumber(L, -1);
	luaL_argcheck(L, length == 3, -1, "Error: number of components in third vertex");
	lua_pop(L, 1);

	lua_rawgeti(L, 2, 1);
	luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
	triZ.X = lua_tonumber(L, 3);

	lua_rawgeti(L, 2, 2);
	luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
	triZ.Y = lua_tonumber(L, 4);

	lua_rawgeti(L, 2, 3);
	luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
	triZ.Z = lua_tonumber(L, 5);

	lua_pop(L, 4);


	irr::scene::SMesh *Tri = new irr::scene::SMesh();
	irr::scene::SMeshBuffer *meshBuf = new irr::scene::SMeshBuffer();
	
	Tri->addMeshBuffer(meshBuf);
	meshBuf->drop();

	meshBuf->Vertices.reallocate(3);
	meshBuf->Vertices.set_used(3);

	meshBuf->Vertices[0] = irr::video::S3DVertex(triX.X, triX.Y, triX.Z,	1, 1, 0,	irr::video::SColor(150, 200, 160, 255), 0, 1);
	meshBuf->Vertices[1] = irr::video::S3DVertex(triY.X, triY.Y, triY.Z,	1, 1, 0,	irr::video::SColor(240, 115, 160, 255), 1, 1);
	meshBuf->Vertices[2] = irr::video::S3DVertex(triZ.X, triZ.Y, triZ.Z,	1, 1, 0,	irr::video::SColor(130, 160, 230, 255), 1, 0);
	
	meshBuf->Indices.reallocate(3);
	meshBuf->Indices.set_used(3);

	meshBuf->Indices[0] = 0;
	meshBuf->Indices[1] = 1;
	meshBuf->Indices[2] = 2;

	meshBuf->recalculateBoundingBox();


	irr::scene::IMeshSceneNode* triNode = smgr->addMeshSceneNode(Tri);

	triNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	triNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	triNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	//triNode->setMaterialTexture(0, irrDriver->getTexture("rust0.jpg"));

	return 0;
}

static int addBox(lua_State* L) {

	irr::core::vector3df pos;
	irr::f32 size;
	std::string name;

	luaL_argcheck(L, lua_istable(L, 1), -1, "Error position table");

	pos.X = lua_tonumber(L, 1);
	pos.Y = lua_tonumber(L, 2);
	pos.Z = lua_tonumber(L, 3);
	//size = lua_tonumber(L, 4);


	///*---------------AddBox----------------------*/
	//meshNode = smgr->addCubeSceneNode();
	//if (meshNode) {
	//	meshNode->setPosition(pos);
	//	meshNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	//}

	///*-------------------------------------------*/

	return 0;
}

static int getNodes(lua_State* L) {
	return 0;
}

static int cameraFunction(lua_State* L) {
	return 0;
}

static int snapshot(lua_State* L) {
	return 0;
}