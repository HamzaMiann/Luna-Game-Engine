#ifndef _cLuaBrain_HG_
#define _cLuaBrain_HG_

extern "C" {
#include <Lua5.3.5/lua.h>
#include <Lua5.3.5/lauxlib.h>
#include <Lua5.3.5/lualib.h>
}

#include <string>
#include <vector>
#include <map>
#include "Camera.h"

// Forward declaration for cyclical reference
class cGameObject;
struct GLFWwindow;

class cLuaBrain
{
public:
	static cGameObject* current_GO;
	static GLFWwindow* window;
	static Camera* camera;

	cGameObject* gameObject;

	// Init Lua and set callback functions
	cLuaBrain(cGameObject* obj);
	~cLuaBrain();
	void LoadScript(std::string scriptName,
					std::string scriptSource);
	void DeleteScript(std::string scriptName);
	// Passes a pointer to the game object vector
	static void SetObjectVector(std::vector< cGameObject* >* p_vecGOs);
	static void SetCamera(Camera* cam);
	// Call all the active scripts that are loaded
	void Update(float deltaTime);

	// Runs a script, but doesn't save it (originally used to set the ObjectID)
	void RunScriptImmediately(std::string script);
	// Called by Lua
	// Passes object ID, new velocity, etc.
	// Returns valid (true or false)
	// Passes: 
	// - position (xyz)
	// called "setObjectState" in lua
	static int l_UpdateObject(lua_State* L);
	// Passes object ID
	// Returns valid (true or false)
	// - position (xyz)
	// called "getObjectState" in lua
	static int l_GetObjectState(lua_State* L);

	static int l_GetKey(lua_State* L);

	static int l_AddForce(lua_State* L);
	static int l_GetForce(lua_State* L);

	static int l_GetForward(lua_State* L);

	static int l_GetVelocity(lua_State* L);
	static int l_SetVelocity(lua_State* L);

	// Commands
	static int l_MoveTo(lua_State* L);
	static int l_RotateTo(lua_State* L);
	static int l_FollowCurve(lua_State* L);
	static int l_FollowCamera(lua_State* L);

	static int l_AddSerial(lua_State* L);
	static int l_AddParallel(lua_State* L);

	static int l_IsCommandDone(lua_State* L);

private:
	std::map< std::string /*scriptName*/,
		std::string /*scriptSource*/ > m_mapScripts;

	static std::vector< cGameObject* >* m_p_vecGOs;
	// returns nullptr if not found
	static cGameObject* m_findObjectByID(int ID);

	lua_State* m_pLuaState;

	std::string m_decodeLuaErrorToString(int error);
};

#endif
