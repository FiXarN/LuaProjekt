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
static int camera(lua_State* L);
static int snapshot(lua_State* L);

irr::scene::IAnimatedMeshSceneNode* node;
irr::scene::ISceneNode * boxNode;
irr::core::vector3df cameraPosition;
irr::core::vector3df cameraTarget;
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
			cameraPosition = cam->getPosition();
			cameraTarget = cam->getTarget();
			cam->setPosition(cameraPosition);
			cam->setTarget(cameraTarget);
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

float counter = 0;
static int addBox(lua_State* L) {

	irr::core::vector3df pos;
	irr::f32 size = 0.0f;
	std::string name;

	luaL_argcheck(L, lua_istable(L, 1), -1, "Error position table");
	luaL_argcheck(L, lua_isnumber(L, 2), -1, "Error size, must be a number");

	if (lua_gettop(L) == 3) {
		if (lua_isnumber(L, 2)) {
			size = lua_tonumber(L, 2);
		}
		if (lua_isstring(L, 3)) {
			name = lua_tostring(L, 3);
		}
	}
	else {
		if (lua_isnumber(L, 2)) {
			size = lua_tonumber(L, 2);
		}
		counter++;
		name = std::to_string(counter);
	}

	lua_rawgeti(L, 1, 1); //Hämtar x värdet och lägger högst upp på stacken
	lua_rawgeti(L, 1, 2); //Hämtar y värdet och lägger högst upp på stacken
	lua_rawgeti(L, 1, 3); //Hämtar z värdet och lägger högst upp på stacken

	if (lua_isnumber(L, -1) || lua_isnumber(L, -2) || lua_isnumber(L, -3)) {
		pos.X = lua_tonumber(L, -3);
		pos.Y = lua_tonumber(L, -2);
		pos.Z = lua_tonumber(L, -1);
	}
	///*---------------AddBox----------------------*/
	boxNode = smgr->addCubeSceneNode(size, 0, -1, pos, irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));
	if (boxNode) {
		boxNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	///*-------------------------------------------*/
	return 0;
}

static int getNodes(lua_State* L) {
	return 0;
}

static int camera(lua_State* L) {
	luaL_argcheck(L, lua_istable(L, 1), -1, "Error: Position 1 is not a table");
	luaL_argcheck(L, lua_istable(L, 2), -1, "Error: Position 2 is not a table");


	lua_rawgeti(L, 1, 1);
	lua_rawgeti(L, 1, 2);
	lua_rawgeti(L, 1, 3);

	lua_rawgeti(L, 2, 1);
	lua_rawgeti(L, 2, 2);
	lua_rawgeti(L, 2, 3);

	luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: Position 3 is not a number");
	luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: Position 4 is not a number");
	luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: Position 5 is not a number");
	luaL_argcheck(L, lua_isnumber(L, 6), -1, "Error: Position 6 is not a number");
	luaL_argcheck(L, lua_isnumber(L, 7), -1, "Error: Position 7 is not a number");
	luaL_argcheck(L, lua_isnumber(L, 8), -1, "Error: Position 8 is not a number");

	cameraPosition = irr::core::vector3df(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	cameraTarget = irr::core::vector3df(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));

	return 0;
}

static int snapshot(lua_State* L) {
	return 0;
}