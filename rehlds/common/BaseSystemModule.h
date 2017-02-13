#ifndef BASESYSTEMMODULE_H
#define BASESYSTEMMODULE_H
#ifdef _WIN32
#pragma once
#endif

#include "ObjectList.h"
#include "IBaseSystem.h"

#define SIGNAL_MODULE_CLOSE 8	// closing down module

enum ModuleState
{
	MODULE_INACTIVE,
	MODULE_PENDING,
	MODULE_RUNNING,
	MODULE_LOAD,
	MODULE_UNLOAD
};

// C4250 - 'class1' : inherits 'BaseSystemModule::member' via dominance
#pragma warning(disable:4250)

class BaseSystemModule: virtual public ISystemModule {
public:
	BaseSystemModule();
	virtual ~BaseSystemModule();

	virtual bool Init(IBaseSystem *system, int serial, char *name);
	virtual void RunFrame(double time);
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	virtual void ExecuteCommand(int commandID, char *commandLine);
	virtual void RegisterListener(ISystemModule *module);
	virtual void RemoveListener(ISystemModule *module);
	virtual IBaseSystem *GetSystem();
	virtual int GetSerial();
	virtual char *GetStatusLine();
	virtual char *GetType();
	virtual char *GetName();
	virtual int GetState();
	virtual int GetVersion();
	virtual void ShutDown();
	virtual char *COM_GetBaseDir() { return ""; }
	void FireSignal(unsigned int signal, void *data);

protected:
	IBaseSystem *m_System;
	ObjectList m_Listener;
	char m_Name[255];
	unsigned int m_State;
	unsigned int m_Serial;
	double m_SystemTime;
};

#endif // BASESYSTEMMODULE_H
