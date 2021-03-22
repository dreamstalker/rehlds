/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

SystemWrapper gSystemWrapper;

void SystemWrapper_Init()
{
	gSystemWrapper.Init(&gSystemWrapper, 0, "SystemWrapper");
}

void SystemWrapper_RunFrame(double time)
{
	gSystemWrapper.RunFrame(time);
}

void SystemWrapper_ShutDown()
{
	gSystemWrapper.ShutDown();
}

BOOL SystemWrapper_LoadModule(char *interfacename, char *library, char *instancename)
{
	if (gSystemWrapper.GetModule(interfacename, library, instancename)) {
		return TRUE;
	}

	return FALSE;
}

void SystemWrapper_ExecuteString(const char *command)
{
	gSystemWrapper.ExecuteString(command);
}

void SystemWrapper_CommandForwarder()
{
	char cmd[MAX_CMD_LINE];
	Q_strcpy(cmd, Cmd_Argv(0));

	if (Cmd_Argc() > 1)
	{
		Q_strcat(cmd, " ");
		Q_strcat(cmd, Cmd_Args());
	}

	cmd[sizeof(cmd) - 1] = '\0';

	if (cmd[0]) {
		SystemWrapper_ExecuteString(cmd);
	}
}

char *SystemWrapper::CheckParam(char *param)
{
	int pos = COM_CheckParm(param);
	if (pos)
	{
		if ((pos + 1) >= com_argc) {
			return "";
		}

		return com_argv[pos + 1];
	}

	return nullptr;
}

void SystemWrapper::Errorf(char *fmt, ...)
{
	va_list argptr;
	static char string[1024];
	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	Printf("***** FATAL ERROR *****\n");
	Printf("%s", string);
	Printf("*** STOPPING SYSTEM ***\n");

	Stop();
}

char *SystemWrapper::GetStatusLine()
{
	return "No status available.\n";
}

bool SystemWrapper::Init(IBaseSystem *system, int serial, char *name)
{
	BaseSystemModule::Init(system, serial, name);

	m_Commands.Init();
	m_Modules.Init();
	m_Libraries.Init();
	m_Listener.Init();

	m_EngineWrapper = new EngineWrapper;

	AddModule((ISystemModule *)m_EngineWrapper, ENGINEWRAPPER_INTERFACE_VERSION);
	m_State = MODULE_RUNNING;

	return true;
}

void SystemWrapper::Stop()
{
	m_State = MODULE_DISCONNECTED;
}

void SystemWrapper::ShutDown()
{
	m_Listener.Clear();
	m_Commands.Clear(true);

	ISystemModule *module = (ISystemModule *)m_Modules.GetFirst();
	while (module)
	{
		module->ShutDown();
		module = (ISystemModule *)m_Modules.GetNext();
	}

	library_t *lib = (library_t *)m_Libraries.RemoveTail();
	while (lib)
	{
		if (lib->handle) {
			Sys_UnloadModule(lib->handle);
		}

		Mem_Free(lib);
		lib = (library_t *)m_Libraries.RemoveTail();
	}

	if (m_EngineWrapper) {
		delete m_EngineWrapper;
		m_EngineWrapper = nullptr;
	}

	Cmd_RemoveWrapperCmds();
	m_State = MODULE_DISCONNECTED;
}

void SystemWrapper::ExecuteCommand(int commandID, char *commandLine)
{
	switch (commandID)
	{
	case CMD_ID_MODULES:
		CMD_Modules(commandLine);
		break;
	case CMD_ID_LOADMODULE:
		CMD_LoadModule(commandLine);
		break;
	case CMD_ID_UNLOADMODULE:
		CMD_UnloadModule(commandLine);
		break;
	default:
		Printf("SystemWrapper::ExecuteCommand: unknown command ID %i.\n", commandID);
		break;
	}
}

void SystemWrapper::CMD_LoadModule(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() < 2) {
		Printf("Syntax: loadmodule <module> [<library>] [<name>]\n");
		return;
	}

	switch (params.CountToken()) {
	case 2:
		GetModule(params.GetToken(1), params.GetToken(1));
		break;
	case 3:
		GetModule(params.GetToken(1), params.GetToken(2));
		break;
	default:
		GetModule(params.GetToken(1), params.GetToken(2), params.GetToken(3));
		break;
	}
}

void SystemWrapper::CMD_UnloadModule(char *cmdLine)
{
	TokenLine params(cmdLine);
	if (params.CountToken() < 2) {
		Printf("Syntax: unloadmodule <module> [<name>]\n");
		return;
	}

	ISystemModule *module = nullptr;
	switch (params.CountToken()) {
	case 2:
		module = FindModule(params.GetToken(1));
		break;
	case 3:
		module = FindModule(params.GetToken(1), params.GetToken(2));
		break;
	}

	if (!module) {
		Printf("Module not found.\n");
		return;
	}

	RemoveModule(module);
}

SystemWrapper::library_t *SystemWrapper::GetLibrary(char *name)
{
	char fixedname[MAX_PATH-4]; // reserve for extension so/dll
	Q_strlcpy(fixedname, name);
	COM_FixSlashes(fixedname);

	library_t *lib = (library_t *)m_Libraries.GetFirst();
	while (lib)
	{
		if (Q_stricmp(lib->name, name) == 0) {
			return lib;
		}

		lib = (library_t *)m_Libraries.GetNext();
	}

	lib = (library_t *)Mem_Malloc(sizeof(library_t));
	if (!lib) {
		DPrintf("ERROR! System::GetLibrary: out of memory (%s).\n", name);
		return nullptr;
	}

	Q_snprintf(lib->name, sizeof(lib->name), "%s." LIBRARY_PREFIX, fixedname);
	FS_GetLocalCopy(lib->name);

	lib->handle = (CSysModule *)Sys_LoadModule(lib->name);
	if (!lib->handle) {
		DPrintf("ERROR! System::GetLibrary: coulnd't load library (%s).\n", lib->name);
		Mem_Free(lib);
		return nullptr;
	}

	lib->createInterfaceFn = (CreateInterfaceFn)Sys_GetFactory(lib->handle);
	if (!lib->createInterfaceFn) {
		DPrintf("ERROR! System::GetLibrary: coulnd't get object factory(%s).\n", lib->name);
		Mem_Free(lib);
		return nullptr;
	}

	m_Libraries.Add(lib);
	DPrintf("Loaded library %s.\n", lib->name);

	return lib;
}

void SystemWrapper::SetTitle(char *text)
{
	Con_Printf("TODO: SystemWrapper::SetTitle ?\n");
}

void SystemWrapper::SetStatusLine(char *text)
{
	Con_Printf("TODO: SystemWrapper::SetStatusLine ?\n");
}

double SystemWrapper::GetTime()
{
	return m_SystemTime;
}

void SystemWrapper::RedirectOutput(char *buffer, int maxSize)
{
	Con_Printf("WARNIG! SystemWrapper::RedirectOutput: not implemented.\n");
}

void SystemWrapper::Printf(char *fmt, ...)
{
	va_list argptr;
	static char string[8192];

	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	Con_Printf("%s", string);
}

void SystemWrapper::DPrintf(char *fmt, ...)
{
	va_list argptr;
	static char string[8192];
	va_start(argptr, fmt);
	Q_vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	Con_DPrintf(string);
}

void SystemWrapper::RunFrame(double time)
{
	m_Tick++;
	m_SystemTime += time;

	if (m_State != MODULE_RUNNING) {
		return;
	}

	ISystemModule *module = (ISystemModule *)m_Modules.GetFirst();
	while (module)
	{
		if (m_State == MODULE_DISCONNECTED)
			break;

		module->RunFrame(m_SystemTime);
		module = (ISystemModule *)m_Modules.GetNext();
	}

	m_LastTime = m_SystemTime;
}

void SystemWrapper::ExecuteFile(char *filename)
{
	char cmd[1024];
	Q_snprintf(cmd, sizeof(cmd), "exec %s\n", filename);
	Cmd_ExecuteString(cmd, src_command);
}

bool SystemWrapper::RegisterCommand(char *name, ISystemModule *module, int commandID)
{
	command_t *cmd = (command_t *)m_Commands.GetFirst();
	while (cmd)
	{
		if (Q_stricmp(cmd->name, name) == 0) {
			Printf("WARNING! System::RegisterCommand: command \"%s\" already exists.\n", name);
			return false;
		}

		cmd = (command_t *)m_Commands.GetNext();
	}

	cmd = (command_t *)Mem_ZeroMalloc(sizeof(command_t));

	Q_strlcpy(cmd->name, name);
	cmd->module = module;
	cmd->commandID = commandID;

	m_Commands.Add(cmd);
	Cmd_AddWrapperCommand(cmd->name, SystemWrapper_CommandForwarder);

	return true;
}

IFileSystem *SystemWrapper::GetFileSystem()
{
	return g_pFileSystem;
}

unsigned char *SystemWrapper::LoadFile(const char *name, int *length)
{
	return COM_LoadFile(name, 5, length);
}

void SystemWrapper::FreeFile(unsigned char *fileHandle)
{
	COM_FreeFile(fileHandle);
}

unsigned int SystemWrapper::GetTick()
{
	return m_Tick;
}

void SystemWrapper::CMD_Modules(char *cmdLine)
{
	ISystemModule *module = (ISystemModule *)m_Modules.GetFirst();
	while (module)
	{
		Printf("%s(%s):%s\n",
			module->GetName(),
			module->GetType(),
			module->GetVersion());

		module = (ISystemModule *)m_Modules.GetNext();
	}

	Printf("--- %i modules in total ---\n", m_Modules.CountElements());
}

char *SystemWrapper::GetType()
{
	return BASESYSTEM_INTERFACE_VERSION;
}

void SystemWrapper::GetCommandMatches(char *string, ObjectList *pMatchList)
{
#ifdef REHLDS_FIXES
	if (!string || !string[0])
		return;

	int len = Q_strlen(string);
	for (auto cmd = Cmd_GetFirstCmd(); cmd; cmd = cmd->next)
	{
		if (!Q_strnicmp(cmd->name, string, len)) {
			pMatchList->Add((void *)cmd->name);
		}
	}

	for (auto var = cvar_vars; var; var = var->next)
	{
		if (!Q_strnicmp(var->name, string, len)) {
			pMatchList->Add((void *)var->name);
		}
	}

#else
	pMatchList->Clear(true);
#endif // REHLDS_FIXES
}

ISystemModule *SystemWrapper::FindModule(char *type, char *name)
{
	if (!type || !type[0])
		return nullptr;

	ISystemModule *module = (ISystemModule *)m_Modules.GetFirst();
	while (module)
	{
		if (Q_stricmp(type, module->GetType()) == 0
		&& (!name || Q_stricmp(name, module->GetType()) == 0)) {
			return module;
		}

		module = (ISystemModule *)m_Modules.GetNext();
	}

	return nullptr;
}

void SystemWrapper::LogConsole(char *filename)
{
	if (filename) {
		Cmd_ExecuteString("log on", src_command);
	} else {
		Cmd_ExecuteString("log off", src_command);
	}
}

Panel *SystemWrapper::GetPanel()
{
	return (Panel *)VGuiWrap2_GetPanel();
}

bool SystemWrapper::InitVGUI(IVGuiModule *module)
{
	return false;
}

bool SystemWrapper::AddModule(ISystemModule *module, char *name)
{
	if (!module)
		return false;

	if (!module->Init(this, m_SerialCounter, name))
	{
		Printf("ERROR! System::AddModule: couldn't initialize module %s.\n", name);
		return false;
	}

	m_Modules.AddHead(module);
	m_SerialCounter++;
	return true;
}

ISystemModule *SystemWrapper::GetModule(char *interfacename, char *library, char *instancename)
{
	ISystemModule *module = FindModule(interfacename, instancename);
	if (module) {
		return module;
	}

	library_t *lib = GetLibrary(library);
	if (!lib) {
		return nullptr;
	}

	module = (ISystemModule *)lib->createInterfaceFn(interfacename, nullptr);
	if (!module) {
		DPrintf("ERROR! System::GetModule: interface \"%s\" not found in library %s.\n", interfacename, lib->name);
		return nullptr;
	}

	AddModule(module, instancename);
	return module;
}

bool SystemWrapper::RemoveModule(ISystemModule *module)
{
	if (!module) {
		return true;
	}

	module->ShutDown();

	command_t *cmd = (command_t *)m_Commands.GetFirst();
	while (cmd)
	{
		if (cmd->module->GetSerial() == module->GetSerial()) {
			m_Commands.Remove(cmd);
			Mem_Free(cmd);
		}

		cmd = (command_t *)m_Commands.GetNext();
	}

	ISystemModule *mod = (ISystemModule *)m_Modules.GetFirst();
	while (mod)
	{
		if (mod->GetSerial() == module->GetSerial()) {
			m_Modules.Remove(mod);
			return true;
		}

		mod = (ISystemModule *)m_Modules.GetNext();
	}

	return false;
}

void SystemWrapper::ExecuteString(const char *commands)
{
	if (!commands || !commands[0])
		return;

	// Remove format characters to block format string attacks
	COM_RemoveEvilChars(const_cast<char *>(commands));

	bool bInQuote = false;

	char *pszDest;
	char singleCmd[256] = {0};

	const char *pszSource = commands;
	while (*pszSource)
	{
		// Parse out single commands and execute them
		pszDest = singleCmd;

		unsigned int i;
		for (i = 0; i < ARRAYSIZE(singleCmd); i++)
		{
			const char c = *pszSource;

			if (c == '"')
			{
				bInQuote = !bInQuote;
			}
			else if ((c == ';' && !bInQuote) || !c)
			{
				// End of command and not in a quoted string
				break;
			}

			*pszDest++ = c;
			pszSource++;
		}

		if (i >= ARRAYSIZE(singleCmd))
		{
			Printf("WARNING! System::ExecuteString: Command token too long.\n");
			return;
		}

		*pszDest = '\0';

		char *pszCmd = singleCmd;
		while (*pszCmd == ' ')
			pszCmd++; // skip leading whitespaces

		DispatchCommand(pszCmd);
	}
}

bool SystemWrapper::DispatchCommand(char *command)
{
	if (!command || !command[0])
		return true;

	TokenLine params(command);
	command_t *cmd = (command_t *)m_Commands.GetFirst();
	while (cmd)
	{
		if (Q_stricmp(cmd->name, params.GetToken(0)) == 0) {
			cmd->module->ExecuteCommand(cmd->commandID, command);
			return true;
		}

		cmd = (command_t *)m_Commands.GetNext();
	}

	Cmd_ExecuteString(command, src_command);
	return true;
}

int COM_BuildNumber()
{
	return build_number();
}

void COM_RemoveEvilChars(char *string)
{
	char *c = string;
	if (!c) {
		return;
	}

	while (*c)
	{
		if (*c < ' ' || *c == '%') {
			*c = ' ';
		}

		c++;
	}
}

bool EngineWrapper::GetViewOrigin(float *origin)
{
#ifndef SWDS
	origin[0] = r_refdef.vieworg[0];
	origin[1] = r_refdef.vieworg[1];
	origin[2] = r_refdef.vieworg[2];
	return true;
#else
	return false;
#endif // SWDS
}

bool EngineWrapper::GetViewAngles(float *angles)
{
#ifndef SWDS
	angles[0] = r_refdef.viewangles[0];
	angles[1] = r_refdef.viewangles[1];
	angles[2] = r_refdef.viewangles[2];
	return true;
#else
	return false;
#endif // SWDS
}

float EngineWrapper::GetCvarFloat(char *szName)
{
	cvar_t *cv = Cvar_FindVar(szName);
	if (cv) {
		return cv->value;
	}

	return 0;
}

char *EngineWrapper::GetCvarString(char *szName)
{
	cvar_t *cv = Cvar_FindVar(szName);
	if (cv) {
		return cv->string;
	}

	return 0;
}

void EngineWrapper::SetCvar(char *szName, char *szValue)
{
	Cvar_Set(szName, szValue);
}

void EngineWrapper::DemoUpdateClientData(client_data_t *cdat)
{
#ifndef SWDS
	ClientDLL_DemoUpdateClientData(cdat);
	scr_fov_value = cdat->fov;
#endif // SWDS
}

void EngineWrapper::CL_QueueEvent(int flags, int index, float delay, event_args_t *pargs)
{
#ifndef SWDS
	::CL_QueueEvent(flags, index, delay, pargs);
#endif // SWDS
}

void EngineWrapper::HudWeaponAnim(int iAnim, int body)
{
#ifndef SWDS
	hudWeaponAnim(iAnim, body);
#endif // SWDS
}

void EngineWrapper::CL_DemoPlaySound(int channel, char *sample, float attenuation, float volume, int flags, int pitch)
{
#ifndef SWDS
	::CL_DemoPlaySound(channel, sample, attenuation, volume, flags, pitch);
#endif // SWDS
}

void EngineWrapper::ClientDLL_ReadDemoBuffer(int size, unsigned char *buffer)
{
#ifndef SWDS
	::ClientDLL_ReadDemoBuffer();
#endif // SWDS
}

int EngineWrapper::GetTraceEntity()
{
	return 0;
}

char *EngineWrapper::GetType()
{
	return ENGINEWRAPPER_INTERFACE_VERSION;
}

char *EngineWrapper::GetStatusLine()
{
	return "No status available.\n";
}

void EngineWrapper::Cbuf_AddText(char *text)
{
	::Cbuf_AddText(text);
}

#ifdef REHLDS_FIXES
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(SystemWrapper, IBaseSystem, BASESYSTEM_INTERFACE_VERSION, gSystemWrapper);
#endif // REHLDS_FIXES
