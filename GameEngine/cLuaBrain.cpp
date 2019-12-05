#include "cLuaBrain.h"

#include <iostream>
#include <locale>
#include "cGameObject.h"
#include "Window.h"
#include "cFollowCurve.h"
#include "cRotateTo.h"
#include "cMoveTo.h"
#include "cFollowCamera.h"

enum COMMAND_TYPE
{
	MOVE,
	ROTATE,
	CURVE,
	FOLLOW
};

cGameObject* cLuaBrain::current_GO = 0;
GLFWwindow* cLuaBrain::window = 0;

cLuaBrain::cLuaBrain(cGameObject* obj)
{
	this->gameObject = obj;
	this->m_p_vecGOs = nullptr;

	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);					/* Lua 5.3.3 */


	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_UpdateObject);
	lua_setglobal(this->m_pLuaState, "setPosition");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectState);
	lua_setglobal(this->m_pLuaState, "getPosition");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetKey);
	lua_setglobal(this->m_pLuaState, "getKey");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddForce);
	lua_setglobal(this->m_pLuaState, "addForce");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetForce);
	lua_setglobal(this->m_pLuaState, "getForce");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetForward);
	lua_setglobal(this->m_pLuaState, "getForward");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetVelocity);
	lua_setglobal(this->m_pLuaState, "getVelocity");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetVelocity);
	lua_setglobal(this->m_pLuaState, "setVelocity");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_MoveTo);
	lua_setglobal(this->m_pLuaState, "Move");
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_RotateTo);
	lua_setglobal(this->m_pLuaState, "Rotate");
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_FollowCurve);
	lua_setglobal(this->m_pLuaState, "Curve");
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_FollowCamera);
	lua_setglobal(this->m_pLuaState, "Follow");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddSerial);
	lua_setglobal(this->m_pLuaState, "AddSerial");
	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_AddParallel);
	lua_setglobal(this->m_pLuaState, "AddParallel");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_IsCommandDone);
	lua_setglobal(this->m_pLuaState, "isDone");

	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}


// Saves (and overwrites) any script
// scriptName is just so we can delete them later
void cLuaBrain::LoadScript(std::string scriptName,
						   std::string scriptSource)
{
	this->m_mapScripts[scriptName] = scriptSource;
	return;
}


void cLuaBrain::DeleteScript(std::string scriptName)
{
	this->m_mapScripts.erase(scriptName);
	return;
}

// Passes a pointer to the game object vector
void cLuaBrain::SetObjectVector(std::vector< cGameObject* >* p_vecGOs)
{
	m_p_vecGOs = p_vecGOs;
	return;
}

Camera* cLuaBrain::camera = 0;
void cLuaBrain::SetCamera(Camera* cam)
{
	camera = cam;
}

// Call all the active scripts that are loaded
void cLuaBrain::Update(float deltaTime)
{
	std::string delta = "delta_time = ";
	delta += std::to_string(deltaTime) + "\n";

	RunScriptImmediately(delta);



	current_GO = this->gameObject;
	//	std::cout << "cLuaBrain::Update() called" << std::endl;
	for (std::map< std::string /*name*/, std::string /*source*/>::iterator itScript =
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++)
	{

		// Pass the script into Lua and update
//		int error = luaL_loadbuffer(L, buffer, strlen(buffer), "line");

		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring(this->m_pLuaState,
									curLuaScript.c_str());

		if (error != 0 /*no error*/)
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
						  0,	/* nargs: number of arguments pushed onto the lua stack */
						  0,	/* nresults: number of results that should be on stack at end*/
						  0);	/* errfunc: location, in stack, of error function.
								   if 0, results are on top of stack. */
		if (error != 0 /*no error*/)
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append(lua_tostring(this->m_pLuaState, -1));

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}

	return;
}

// Runs a script, but doesn't save it (originally used to set the ObjectID)
void cLuaBrain::RunScriptImmediately(std::string script)
{
	current_GO = this->gameObject;
	int error = luaL_loadstring(this->m_pLuaState,
								script.c_str());

	if (error != 0 /*no error*/)
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	// execute funtion in "protected mode", where problems are 
	//  caught and placed on the stack for investigation
	error = lua_pcall(this->m_pLuaState,	/* lua state */
					  0,	/* nargs: number of arguments pushed onto the lua stack */
					  0,	/* nresults: number of results that should be on stack at end*/
					  0);	/* errfunc: location, in stack, of error function.
							  if 0, results are on top of stack. */
	if (error != 0 /*no error*/)
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->m_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack)
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}


// Called by Lua
// Passes object ID, new velocity, etc.
// Returns valid (true or false)
// Passes: 
// - position (xyz)
// - velocity (xyz)
// called "setObjectState" in lua
/*static*/ int cLuaBrain::l_UpdateObject(lua_State* L)
{
	//int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObject* pGO = current_GO;// cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Get the values that lua pushed and update object
	pGO->pos.x = (float)lua_tonumber(L, 1);	/* get argument */
	pGO->pos.y = (float)lua_tonumber(L, 2);	/* get argument */
	pGO->pos.z = (float)lua_tonumber(L, 3);	/* get argument */

	lua_pushboolean(L, true);	// index is OK

	return 1;		// There were 7 things on the stack

}

// Passes object ID
// Returns valid (true or false)
// - position (xyz)
// - velocity (xyz)
// called "getObjectState" in lua
/*static*/ int cLuaBrain::l_GetObjectState(lua_State* L)
{
	//int objectID = (int)lua_tonumber(L, 1);	/* get argument */

	// Exist? 
	cGameObject* pGO = current_GO;// cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	//lua_pushboolean(L, true);	// index is OK
	lua_pushnumber(L, pGO->pos.x);
	lua_pushnumber(L, pGO->pos.y);
	lua_pushnumber(L, pGO->pos.z);

	return 3;		// There were 7 things on the stack
}

int cLuaBrain::l_GetKey(lua_State * L)
{
	
	std::string key = std::string(lua_tostring(L, 1));

	if (key == "w" && glfwGetKey(global::window, GLFW_KEY_W))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "a" && glfwGetKey(global::window, GLFW_KEY_A))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "s" && glfwGetKey(global::window, GLFW_KEY_S))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "d" && glfwGetKey(global::window, GLFW_KEY_D))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "q" && glfwGetKey(global::window, GLFW_KEY_Q))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "e" && glfwGetKey(global::window, GLFW_KEY_E))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	if (key == "shift" && glfwGetKey(global::window, GLFW_KEY_LEFT_SHIFT))
	{
		lua_pushboolean(L, true);
		return 1;
	}
	

	lua_pushboolean(L, false);

	return 1;
}

int cLuaBrain::l_AddForce(lua_State * L)
{
	cGameObject* pGO = current_GO;
	pGO->AddForce(
		glm::vec3(
			(float)lua_tonumber(L, 1),
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3)
		)
	);
	return 0;
}

int cLuaBrain::l_GetForce(lua_State * L)
{
	cGameObject* pGO = current_GO;
	glm::vec3 force = pGO->GetForce();
	lua_pushnumber(L, force.x);
	lua_pushnumber(L, force.y);
	lua_pushnumber(L, force.z);
	return 3;
}

int cLuaBrain::l_GetForward(lua_State * L)
{
	cGameObject* pGO = current_GO;
	glm::vec3 forward = (pGO->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	lua_pushnumber(L, forward.x);
	lua_pushnumber(L, forward.y);
	lua_pushnumber(L, forward.z);
	return 3;
}

int cLuaBrain::l_GetVelocity(lua_State * L)
{
	cGameObject* pGO = current_GO;
	glm::vec3 vel = pGO->GetVelocity();
	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);
	lua_pushnumber(L, vel.z);
	return 3;
}

int cLuaBrain::l_SetVelocity(lua_State * L)
{
	cGameObject* pGO = current_GO;
	pGO->SetVelocity(
		glm::vec3(
			(float)lua_tonumber(L, 1),
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3)
		)
	);
	return 0;
}

int cLuaBrain::l_MoveTo(lua_State* L)
{
	lua_pushnumber(L, (int)COMMAND_TYPE::MOVE);
	return 1;
}

int cLuaBrain::l_RotateTo(lua_State* L)
{
	lua_pushnumber(L, (int)COMMAND_TYPE::ROTATE);
	return 1;
}

int cLuaBrain::l_FollowCurve(lua_State* L)
{
	lua_pushnumber(L, (int)COMMAND_TYPE::CURVE);
	return 1;
}

int cLuaBrain::l_FollowCamera(lua_State* L)
{
	lua_pushnumber(L, (int)COMMAND_TYPE::FOLLOW);
	return 1;
}

iCommand* get_command(COMMAND_TYPE type, lua_State* L)
{
	iCommand* cmd = nullptr;
	switch (type)
	{
	case MOVE:
		glm::vec3 target = glm::vec3(
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3),
			(float)lua_tonumber(L, 4)
		);
		cmd = new cMoveTo(cLuaBrain::current_GO, target, (float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6));
		break;
	case ROTATE:
	{
		glm::vec3 target_angle = glm::vec3(
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3),
			(float)lua_tonumber(L, 4)
		);
		cmd = new cRotateTo(cLuaBrain::current_GO, target_angle, (float)lua_tonumber(L, 5));
	}
		break;
	case CURVE:
	{
		glm::vec3 target = glm::vec3(
			(float)lua_tonumber(L, 2),
			(float)lua_tonumber(L, 3),
			(float)lua_tonumber(L, 4)
		);
		glm::vec3 offset = glm::vec3(
			(float)lua_tonumber(L, 5),
			(float)lua_tonumber(L, 6),
			(float)lua_tonumber(L, 7)
		);
		cmd = new cFollowCurve(cLuaBrain::current_GO, target, offset, (float)lua_tonumber(L, 8));
	}
		break;
	case FOLLOW:
	{
		float min = (float)lua_tonumber(L, 2);
		glm::vec3 offset = glm::vec3(
			(float)lua_tonumber(L, 3),
			(float)lua_tonumber(L, 4),
			(float)lua_tonumber(L, 5)
		);
		cmd = new cFollowCamera(cLuaBrain::current_GO, min, offset, cLuaBrain::camera);
	}
		break;
	}
	return cmd;
}

int cLuaBrain::l_AddSerial(lua_State* L)
{
	cGameObject* pGO = current_GO;
	COMMAND_TYPE type = (COMMAND_TYPE)((int)lua_tonumber(L, 1));
	iCommand* cmd = get_command(type, L);
	if (cmd)
	{
		pGO->cmd_group->AddSerial(cmd);
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}

int cLuaBrain::l_AddParallel(lua_State* L)
{
	cGameObject* pGO = current_GO;
	COMMAND_TYPE type = (COMMAND_TYPE)((int)lua_tonumber(L, 1));
	iCommand* cmd = get_command(type, L);
	if (cmd)
	{
		pGO->cmd_group->AddParallel(cmd);
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}

int cLuaBrain::l_IsCommandDone(lua_State* L)
{
	cGameObject* pGO = current_GO;
	if (pGO->cmd_group->Is_Done())
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}

/*static*/
std::vector< cGameObject* >* cLuaBrain::m_p_vecGOs;


// returns nullptr if not found
/*static*/ cGameObject* cLuaBrain::m_findObjectByID(int ID)
{
	for (std::vector<cGameObject*>::iterator itGO = cLuaBrain::m_p_vecGOs->begin();
		 itGO != cLuaBrain::m_p_vecGOs->end(); itGO++)
	{
		if ((*itGO)->id == ID)
		{	// Found it!
			return (*itGO);
		}
	}//for ( std::vector<cGameObject*>::iterator itGO...
	// Didn't find it
	return nullptr;
}


std::string cLuaBrain::m_decodeLuaErrorToString(int error)
{
	switch (error)
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}
