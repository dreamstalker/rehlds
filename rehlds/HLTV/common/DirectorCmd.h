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

#include "hltv.h"
#include "BitBuffer.h"

class DirectorCmd {
public:
	DirectorCmd();
	virtual ~DirectorCmd();

	void Clear();
	void Copy(DirectorCmd *cmd);
	void Resize(int size);
	void FromString(char *string);
	char *ToString();
	void WriteToStream(BitBuffer *stream);
	bool ReadFromStream(BitBuffer *stream);
	int GetType();
	char *GetName();
	float GetTime();
	void SetTime(float time);
	void SetStartData();
	void SetEventData(int entity1, int entity2, int flags);
	void SetChaseData(int entity1, int entity2, float distance, int flags);
	void SetModeData(int mode);
	void SetInEyeData(int entity);
	void SetMapData(int entity, float angle, float distance);
	void SetCameraData(vec_t *position, vec_t *angles, float fov, int entity);
	void SetCamPathData(vec_t *position, vec_t *angles, float fov, int flags);
	void SetTimeScaleData(float factor);
	void SetMessageData(int effect, unsigned int color, vec_t *position, float fadein, float fadeout, float holdtime, float fxtime, char *text);
	void SetSoundData(char *name, float volume);
	void SetStatusData(int slots, int spectators, int proxies);
	void SetBannerData(char *filename);
	void SetStuffTextData(char *commands);
	void SetWayPoints(int number);

	bool GetEventData(int &entity1, int &entity2, int &flags);
	bool GetChaseData(int &entity1, int &entity2, float &distance, int &flags);
	bool GetInEyeData(int &player);
	bool GetMapData(int &entity, float &angle, float &distance);
	bool GetCameraData(vec_t *position, vec_t *angles, float &fov, int &entity);
	bool GetCamPathData(vec_t *position, vec_t *angles, float &fov, int &flags);
	bool GetTimeScaleData(float &factor);
	bool GetMessageData(int &effect, int &color, vec_t *position, float &fadein, float &fadeout, float &holdtime, float &fxtime, char *text);
	bool GetSoundData(char *name, float &volume);
	bool GetModeData(int &mode);
	bool GetStatusData(int &slots, int &spectators, int &proxies);
	bool GetBannerData(char *filename);
	bool GetStuffTextData(char *commands);
	bool GetWayPointsData(int &number);

public:
	static char *m_CMD_Name[];

	float m_Time;
	int m_Type;
	int m_Size;
	BitBuffer m_Data;
	int m_Index;
};
