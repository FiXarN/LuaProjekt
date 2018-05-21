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
#include <string>

static int addMesh(lua_State* L);
static int addBox(lua_State* L);
static int getNodes(lua_State* L);
static int camera(lua_State* L);
static int snapshot(lua_State* L);

irr::scene::ISceneNode * boxNode;
irr::scene::IMeshSceneNode* triNode;
irr::core::vector3df cameraPosition;
irr::core::vector3df cameraTarget;
irr::scene::ISceneNode * meshNode;
irr::scene::ISceneManager* smgr;
irr::video::IVideoDriver* driver;
irr::video::IImage *screenshot;
irr::scene::ICameraSceneNode *cam;

int id = 0;
std::string name;

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

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	driver	= device->getVideoDriver();
	smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	//Model
	irr::scene::IAnimatedMesh* mesh = smgr->getMesh("../../Bin/Meshes/sydney.md2");

	if (!mesh) {
		device->drop();
		return 1;
	}

	cam = smgr->addCameraSceneNodeFPS();
	cam->setName("Camera");
	cam->setID(++id);
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

		driver->beginScene(true, true, irr::video::SColor(255, 113, 113, 133));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
		screenshot = driver->createScreenShot();
	}

	device->drop();

	conThread.join();
	return 0;
}

//Exempelinput
//1st addMesh({{-10.0, -10.0, 50.0}, {10.0, -10.0, 50.0}, {0.0, 10.0, 50.0}})
//2st addMesh({{-10.0, -10.0, 50.0}, {10.0, -10.0, 50.0}, {0.0, 10.0, 50.0}, {-15.0, -15.0, 40.0}, {15.0, -15.0, 40.0}, {0.0, 15.0, 40.0}})
//5st addMesh({{-10.0, -10.0, 50.0}, {10.0, -10.0, 50.0}, {0.0, 10.0, 50.0}, {-15.0, -15.0, 40.0}, {15.0, -15.0, 40.0}, {0.0, 15.0, 40.0}, {-20.0, -20.0, 30.0}, {20.0, -20.0, 30.0}, {0.0, 20.0, 30.0}, {-25.0, -25.0, 20.0}, {25.0, -25.0, 20.0}, {0.0, 25.0, 20.0}, {-30.0, -30.0, 15.0}, {30.0, -30.0, 15.0}, {0.0, 30.0, 15.0}})
int triCounter = 0;
static int addMesh(lua_State* L) {
	luaL_argcheck(L, lua_istable(L, 1), -1, "Error input is not a table");

	lua_len(L, 1);
	int nrOfVertices = lua_tonumber(L, -1);
	luaL_argcheck(L, nrOfVertices % 3 == 0, -1, "Error: not a valid number of vertices");
	lua_pop(L, 1);

	irr::core::vector3df triU;
	irr::core::vector3df triV;
	irr::core::vector3df triW;

	int length = 0;

	for (int i = 1; i <= (nrOfVertices / 3); i++) {
		//X
		lua_rawgeti(L, 1, (i * 3) - 2);
		luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

		lua_len(L, 2);
		length = lua_tonumber(L, -1);
		luaL_argcheck(L, length == 3, -1, "Error: number of components in first vertex");
		lua_pop(L, 1);

		lua_rawgeti(L, 2, 1);
		luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
		triU.X = lua_tonumber(L, 3);

		lua_rawgeti(L, 2, 2);
		luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
		triU.Y = lua_tonumber(L, 4);

		lua_rawgeti(L, 2, 3);
		luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
		triU.Z = lua_tonumber(L, 5);

		lua_pop(L, 4);

		//Y
		lua_rawgeti(L, 1, (i * 3) - 1);
		luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

		lua_len(L, 2);
		length = lua_tonumber(L, -1);
		luaL_argcheck(L, length == 3, -1, "Error: number of components in second vertex");
		lua_pop(L, 1);

		lua_rawgeti(L, 2, 1);
		luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
		triV.X = lua_tonumber(L, 3);

		lua_rawgeti(L, 2, 2);
		luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
		triV.Y = lua_tonumber(L, 4);

		lua_rawgeti(L, 2, 3);
		luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
		triV.Z = lua_tonumber(L, 5);

		lua_pop(L, 4);

		//Z
		lua_rawgeti(L, 1, (i * 3));
		luaL_argcheck(L, lua_istable(L, -1), -1, "Error input is not a table");

		lua_len(L, 2);
		length = lua_tonumber(L, -1);
		luaL_argcheck(L, length == 3, -1, "Error: number of components in third vertex");
		lua_pop(L, 1);

		lua_rawgeti(L, 2, 1);
		luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: non-numeric coordinates");
		triW.X = lua_tonumber(L, 3);

		lua_rawgeti(L, 2, 2);
		luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: non-numeric coordinates");
		triW.Y = lua_tonumber(L, 4);

		lua_rawgeti(L, 2, 3);
		luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: non-numeric coordinates");
		triW.Z = lua_tonumber(L, 5);

		lua_pop(L, 4);


		irr::scene::SMesh *Tri = new irr::scene::SMesh();
		irr::scene::SMeshBuffer *meshBuf = new irr::scene::SMeshBuffer();

		Tri->addMeshBuffer(meshBuf);
		meshBuf->drop();

		meshBuf->Vertices.reallocate(3);
		meshBuf->Vertices.set_used(3);

		meshBuf->Vertices[0] = irr::video::S3DVertex(triU.X, triU.Y, triU.Z, 1, 1, 0, irr::video::SColor(150, 200, 160, 255), 0, 1);
		meshBuf->Vertices[1] = irr::video::S3DVertex(triV.X, triV.Y, triV.Z, 1, 1, 0, irr::video::SColor(240, 115, 160, 255), 1, 1);
		meshBuf->Vertices[2] = irr::video::S3DVertex(triW.X, triW.Y, triW.Z, 1, 1, 0, irr::video::SColor(130, 160, 230, 255), 1, 0);

		meshBuf->Indices.reallocate(3);
		meshBuf->Indices.set_used(3);

		meshBuf->Indices[0] = 0;
		meshBuf->Indices[1] = 1;
		meshBuf->Indices[2] = 2;

		meshBuf->recalculateBoundingBox();


		triNode = smgr->addMeshSceneNode(Tri);

		triNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		triNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		triNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);

		triNode->setID(++id);
		std::string name = "TriangleNr" + std::to_string(++triCounter);
		triNode->setName(name.c_str());
	}
	return 0;
}

int boxCounter = 0;
static int addBox(lua_State* L) {

	irr::core::vector3df pos;
	irr::f32 size = 0.0f;

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
		name = "BoxNameNr " + std::to_string(++boxCounter);
	}

	int nrOfComponents = 0;
	lua_len(L, 1);
	nrOfComponents = lua_tonumber(L, -1);
	lua_pop(L, 1);

	if (nrOfComponents == 3) {
		lua_rawgeti(L, 1, 1); //Hämtar x värdet och lägger högst upp på stacken
		lua_rawgeti(L, 1, 2); //Hämtar y värdet och lägger högst upp på stacken
		lua_rawgeti(L, 1, 3); //Hämtar z värdet och lägger högst upp på stacken

		if (lua_isnumber(L, -1) || lua_isnumber(L, -2) || lua_isnumber(L, -3)) {
			pos.X = lua_tonumber(L, -3);
			pos.Y = lua_tonumber(L, -2);
			pos.Z = lua_tonumber(L, -1);
		}
		/*---------------AddBox----------------------*/
		boxNode = smgr->addCubeSceneNode(size, 0, -1, pos, irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1));
		if (boxNode) {
			boxNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
			boxNode->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		}
		/*-------------------------------------------*/
	}
	else {
		luaL_argcheck(L, nrOfComponents == 3, -1, "Wrong input of vertex components");
	}

	boxNode->setID(++id);
	boxNode->setName(name.c_str());

	return 0;
}

static int getNodes(lua_State* L) {
	//addMesh({{-10.0, -10.0, 50.0}, {10.0, -10.0, 50.0}, {0.0, 10.0, 50.0}})
	// lägg till error checks
	lua_newtable(L);

	for (int i = 1; i <= id; i++) {
		lua_newtable(L);
		irr::scene::ISceneNode* tempNode = smgr->getSceneNodeFromId(i);

		lua_pushstring(L, "id");
		lua_pushnumber(L, i);
		lua_settable(L, -3);

		lua_pushstring(L, "name");
		lua_pushstring(L, tempNode->getName());
		lua_settable(L, -3);

		irr::u32 fourcc = (irr::u32)tempNode->getType();
		irr::c8* chars = (irr::c8*)&fourcc;
		std::string code;
		for (int j = 0; j < 4; j++)
			code += chars[j];

		lua_pushstring(L, "type");
		lua_pushstring(L, code.c_str());
		lua_settable(L, -3);

		lua_rawseti(L, 1, i);
	}

	return 1;
}

static int camera(lua_State* L) {
	luaL_argcheck(L, lua_istable(L, 1), -1, "Error: Position 1 is not a table");
	luaL_argcheck(L, lua_istable(L, 2), -1, "Error: Position 2 is not a table");


	int length;
	lua_len(L, 1);
	length = lua_tonumber(L, -1);
	lua_pop(L, 1);

	int length1;
	lua_len(L, 2);
	length1 = lua_tonumber(L, -1);
	lua_pop(L, 1);


	lua_rawgeti(L, 1, 1);
	lua_rawgeti(L, 1, 2);
	lua_rawgeti(L, 1, 3);


	lua_rawgeti(L, 2, 1);
	lua_rawgeti(L, 2, 2);
	lua_rawgeti(L, 2, 3);

	if (length == 3) {
		luaL_argcheck(L, lua_isnumber(L, 3), -1, "Error: coordinate is not a number");
		luaL_argcheck(L, lua_isnumber(L, 4), -1, "Error: coordinate is not a number");
		luaL_argcheck(L, lua_isnumber(L, 5), -1, "Error: coordinate is not a number");
	}
	else
		luaL_argcheck(L, length % 3 == 0, -1, "Error: target has wrong number of coordinates");
	if (length1 == 3) {
		luaL_argcheck(L, lua_isnumber(L, 6), -1, "Error: coordinate is not a number");
		luaL_argcheck(L, lua_isnumber(L, 7), -1, "Error: coordinate is not a number");
		luaL_argcheck(L, lua_isnumber(L, 8), -1, "Error: coordinate is not a number");
	}
	else
		luaL_argcheck(L, length % 3 == 0, -1, "Error: target has wrong number of coordinates");

	cameraPosition = irr::core::vector3df(lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	cameraTarget = irr::core::vector3df(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));

	return 0;
}

static int snapshot(lua_State* L) {
	irr::video::IImage *image = screenshot;
	std::string fileName;

	luaL_argcheck(L, lua_isstring(L, 1), -1, "Not a vaild filename");

	if (lua_isstring(L, 1)) {
		fileName = lua_tostring(L, 1);
	}

	// File name string check
	luaL_argcheck(L, fileName.substr(fileName.find_last_of(".") + 1) == "jpg" || fileName.substr(fileName.find_last_of(".") + 1) == "png", -1, "Bad filename");

	std::string filePath = "../../Bin/Screenshot/" + fileName;

	if (driver->writeImageToFile(image, filePath.c_str())) {
		std::cout << "Image is saved" << std::endl;
		image->drop();
	}
	else {
		std::cout << "Image couldn't be saved" << std::endl;
	}
	return 0;
}