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
*/
#pragma once
#include "osconfig.h"
#include "FlightRecorder.h"

class CRehldsFlightRecorder : public IRehldsFlightRecorder {
public:
	static const unsigned int FLIGHT_RECORDER_VERSION = 1;

	static const unsigned int META_REGION_SIZE = 128 * 1024;
	static const unsigned int META_REGION_HEADER = 128;
	static const unsigned int META_REGION_MAIN_SIZE = META_REGION_SIZE - META_REGION_HEADER;

	static const unsigned int DATA_REGION_SIZE = 768 * 1024;
	static const unsigned int DATA_REGION_HEADER = 128;
	static const unsigned int DATA_REGION_MAIN_SIZE = DATA_REGION_SIZE - DATA_REGION_HEADER;

	static const unsigned int MSG_MAX_SIZE = 0x7FF0;
	static const unsigned int MSG_MAX_ID = 0x7FF0;

private:

	enum meta_record_type_t {
		MRT_MESSAGE_DEF = 1,
		MRT_SHUNT = 2,
	};

#pragma pack(push, 1)
	struct recorder_state {
		unsigned int wpos;
		unsigned int lastMsgBeginPos;
		uint16 curMessage;
	};

	struct meta_header {
		unsigned int version;
		unsigned int regionSize;
		unsigned int headerCrc32;
		unsigned int numMessages;
		unsigned int metaRegionPos;
	};

	struct data_header {
		unsigned int version;
		unsigned int regionSize;
		unsigned int headerCrc32;
		unsigned int prevItrLastPos;
	};
#pragma pack(pop)

	uint8* m_MetaRegion;
	uint8* m_DataRegion;

	uint8* m_MetaRegionPtr;
	uint8* m_DataRegionPtr;
	meta_header* m_pMetaHeader;
	recorder_state* m_pRecorderState;
	data_header* m_pDataHeader;

	void InitHeadersContent();
	void MoveToStart();

	template<typename T>
	void WritePrimitive(T v) {
		if (m_pRecorderState->curMessage == 0) {
			Sys_Error("%s: Could not write, invalid state", __func__);
		}

		CheckSize(sizeof(T));
		unsigned int freeSz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
		if (freeSz < sizeof(T)) {
			MoveToStart();
		}

		*(T*)(m_DataRegionPtr + m_pRecorderState->wpos) = v;
		m_pRecorderState->wpos += sizeof(T);
	}

	void CheckSize(unsigned int wantToWriteLen);

public:
	CRehldsFlightRecorder();
	virtual ~CRehldsFlightRecorder();
	void dump(const char* fname);

	EXT_FUNC virtual void StartMessage(uint16 msg, bool entrance);
	EXT_FUNC virtual void EndMessage(uint16 msg, bool entrance);

	EXT_FUNC virtual void WriteInt8(int8 v);
	EXT_FUNC virtual void WriteUInt8(uint8 v);

	EXT_FUNC virtual void WriteInt16(int16 v);
	EXT_FUNC virtual void WriteUInt16(uint16 v);

	EXT_FUNC virtual void WriteInt32(int32 v);
	EXT_FUNC virtual void WriteUInt32(uint32 v);

	EXT_FUNC virtual void WriteInt64(int64 v);
	EXT_FUNC virtual void WriteUInt64(uint64 v);

	EXT_FUNC virtual void WriteFloat(float v);
	EXT_FUNC virtual void WriteDouble(double v);

	EXT_FUNC virtual void WriteBuffer(const void* data, unsigned int len);
	EXT_FUNC virtual void WriteString(const char* s);

	EXT_FUNC virtual uint16 RegisterMessage(const char* module, const char *message, unsigned int version, bool inOut);
};
