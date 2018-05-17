#ifdef _MSC_VER
#include <irrlicht.h>
#include "driverChoice.h"
#endif

using namespace irr;

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

			switch (event.MouseInput.Event) 
			{
			case: EMIE_MOUSE_MOVED
				Position.X = event.MouseInput.X;
				Position.Y = event.MouseInput.Y;
				break;
			}
		}

		return false;
	}

	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver()
	{
		for (u32 i = 0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};