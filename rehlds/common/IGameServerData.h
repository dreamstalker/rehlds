#pragma once

#include "maintypes.h"
#include "interface.h"

class IGameServerData : public IBaseInterface {
public:
	virtual ~IGameServerData() { };

	virtual void WriteDataRequest(const void *buffer, int bufferSize) = 0;

	virtual int ReadDataResponse(void *data, int len) = 0;
};

#define GAMESERVERDATA_INTERFACE_VERSION "GameServerData001"
