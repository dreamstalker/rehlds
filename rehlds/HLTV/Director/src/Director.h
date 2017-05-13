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

#pragma once

#include <HLTV/IProxy.h>
#include <HLTV/IWorld.h>
#include <HLTV/IDirector.h>

#include "BaseSystemModule.h"
#include "ObjectDictionary.h"

class Director: public IDirector, public BaseSystemModule {
public:
	Director() {}
	virtual ~Director() {}

	virtual bool Init(IBaseSystem *system, int serial, char *name);
	virtual void RunFrame(double time);
	virtual void ReceiveSignal(ISystemModule *module, unsigned int signal, void *data);
	virtual void ExecuteCommand(int commandID, char *commandLine);
	virtual char *GetStatusLine();
	virtual char *GetType();
	virtual void ShutDown();

	virtual void NewGame(IWorld *world, IProxy *proxy);
	virtual char *GetModName();
	virtual void WriteCommands(BitBuffer *stream, float startTime, float endTime);
	virtual int AddCommand(DirectorCmd *cmd);
	virtual bool RemoveCommand(int index);
	virtual DirectorCmd *GetLastCommand();
	virtual IObjectContainer *GetCommands();

	typedef struct playerData_s {
		vec3_t origin;
		vec3_t angles;
		int active;
		int target;
		float rank;
	} playerData_t;

	typedef struct worldHistory_s
	{
		float time;
		unsigned int seqNr;

		typedef struct worldEvent_s {
			int entity1;
			int entity2;
			unsigned int flags;
		} worldEvent_t;

		worldEvent_t event;
		playerData_t players[MAX_CLIENTS];

	} worldHistory_t;

	worldHistory_t *FindBestEvent();

	void ExecuteDirectorCommands();
	void RandomizeCommand(DirectorCmd *cmd);
	int GetClosestPlayer(frame_t *frame, int entityIndex);
	void AddEvent(int entity1, int entity2, unsigned int flags);
	void SmoothRank(int playerNum, float rank);
	void AnalysePlayer(int playerNum);
	void AnalyseFrame(frame_t *frame);
	void ClearDirectorCommands();
	float AddBestMODCut();
	float AddBestGenericCut();
	void WriteSignonData();
	void WriteProxyStatus(BitBuffer *stream);

private:
	enum LocalCommandIDs { CMD_ID_SLOWMOTION = 1 };
	void CMD_SlowMotion(char *cmdLine);

protected:
	ObjectDictionary m_Commands;

	worldHistory_t *m_history;
	float m_maxRank;

	enum { MAX_WORLD_HISTORY = 200 };
	float m_gaussFilter[MAX_WORLD_HISTORY];

	int m_historyLength;
	unsigned int m_nextCutSeqnr;
	unsigned int m_currentSeqnr;

	float m_nextCutTime;
	float m_currentTime;
	float m_LastExecTime;
	float m_slowMotion;

	worldHistory_t::worldEvent_t m_frameEvent;
	DirectorCmd *m_lastCut;
	IBSPModel *m_WorldModel;
	bool m_Active;
	IWorld *m_World;
	IProxy *m_Proxy;
};
