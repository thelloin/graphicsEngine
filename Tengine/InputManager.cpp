#include "InputManager.h"

namespace Tengine {

	InputManager::InputManager() : _mouseCoords(0.0f)
	{
	}


	InputManager::~InputManager()
	{
	}

	void InputManager::pressKey(unsigned int keyID) {
		// Here we are treating _keyMap as an associative array.
		// If keyID doesn't already exists in _keyMap, it will get added.
		_keyMap[keyID] = true;
	}

	void InputManager::releaseKey(unsigned int keyID) {
		_keyMap[keyID] = false;
	}

	void InputManager::setMouseCoords(float x, float y) {
		_mouseCoords.x = x;
		_mouseCoords.y = y;
	}

	bool InputManager::isKeyPressed(unsigned int keyID) {
		// We don't want to use the associative array approach here
		// because we don't want to create a key if it doesn't exists
		// so we do it manually
		auto it = _keyMap.find(keyID);
		if (it != _keyMap.end()) {
			// found the key
			return it->second;
		}
		else {
			// didn't find the key
			return false;
		}
	}

}