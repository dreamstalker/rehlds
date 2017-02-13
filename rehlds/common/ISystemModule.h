#ifndef ISYSTEMMODULE_H
#define ISYSTEMMODULE_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class IBaseSystem;
class ISystemModule;

class ISystemModule: public IBaseInterface {
public:
	virtual ~ISystemModule() {}
	virtual bool Init(IBaseSystem *system, int serial, char *name) = 0;

	virtual void RunFrame(double time) = 0;
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data) = 0;
	virtual void ExecuteCommand(int commandID, char *commandLine) = 0;
	virtual void RegisterListener(ISystemModule *module) = 0;
	virtual void RemoveListener(ISystemModule *module) = 0;

	virtual IBaseSystem *GetSystem() = 0;

	virtual int GetSerial() = 0;
	virtual char *GetStatusLine() = 0;
	virtual char *GetType() = 0;
	virtual char *GetName() = 0;

	virtual int GetState() = 0;
	virtual int GetVersion() = 0;
	virtual void ShutDown() = 0;
};

#endif // ISYSTEMMODULE_H
