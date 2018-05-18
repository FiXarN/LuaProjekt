#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

using namespace irr;

#include <irrlicht.h>
#include <vector3d.h>


class CSampleSceneNode : public scene::ISceneNode {
	core::triangle3d<f32> Triangle;
	core::vector3df Vertices[3];
	video::SMaterial Material;
public:
	CSampleSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id)
		: scene::ISceneNode(parent, smgr, id) {
		Material.Wireframe = false;
		Material.Lighting = false;

		Vertices[0] = { 0.0, 0.0, 0.0 };
		Vertices[1] = { 0.0, 0.0, 0.0 };
		Vertices[2] = { 0.0, 0.0, 0.0 };
	}
};