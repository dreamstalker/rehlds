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
#include "precompiled.h"

CRehldsFlightRecorder::CRehldsFlightRecorder() {
	m_MetaRegion = (uint8*) sys_allocmem(META_REGION_SIZE);
	m_DataRegion = (uint8*) sys_allocmem(DATA_REGION_SIZE);

	if (!m_MetaRegion || !m_DataRegion) {
		Sys_Error("%s: direct allocation failed", __func__);
	}

	//initialize meta region header
	char* metaPos = (char*)m_MetaRegion;
	const char* metaSignature = "REHLDS_FLIGHTREC_META";
	metaPos += Q_sprintf(metaPos, "%s%s%s:", metaSignature, metaSignature, metaSignature);

	m_pMetaHeader = (meta_header*)metaPos;
	metaPos += sizeof(meta_header);
	m_pRecorderState = (recorder_state*)metaPos;
	metaPos += sizeof(recorder_state);

	if ((metaPos - (char*)m_MetaRegion) > META_REGION_HEADER) {
		Sys_Error("%s: Meta header overflow", __func__);
	}

	//initialize data region header
	char* dataPos = (char*)m_DataRegion;
	const char* dataSignature = "REHLDS_FLIGHTREC_DATA";
	dataPos += Q_sprintf(dataPos, "%s%s%s:", dataSignature, dataSignature, dataSignature);

	m_pDataHeader = (data_header*)dataPos;
	dataPos += sizeof(data_header);

	if ((dataPos - (char*)m_pDataHeader) > DATA_REGION_HEADER) {
		Sys_Error("%s: Data header overflow", __func__);
	}

	InitHeadersContent();

	m_MetaRegionPtr = m_MetaRegion + META_REGION_HEADER;
	m_DataRegionPtr = m_DataRegion + DATA_REGION_HEADER;
}

CRehldsFlightRecorder::~CRehldsFlightRecorder() {
	sys_freemem(m_MetaRegion, META_REGION_SIZE);
	sys_freemem(m_DataRegion, DATA_REGION_SIZE);
}

void CRehldsFlightRecorder::InitHeadersContent() {
	m_pMetaHeader->version = FLIGHT_RECORDER_VERSION;
	m_pMetaHeader->regionSize = META_REGION_SIZE;
	m_pMetaHeader->metaRegionPos = 0;
	m_pMetaHeader->numMessages = 0;
	m_pMetaHeader->headerCrc32 = crc32c_t(0, m_MetaRegion, ((uint8*)m_pMetaHeader - m_MetaRegion) + offsetof(meta_header, headerCrc32));

	m_pRecorderState->wpos = 0;
	m_pRecorderState->lastMsgBeginPos = 0xFFFFFFFF;
	m_pRecorderState->curMessage = 0;

	m_pDataHeader->version = FLIGHT_RECORDER_VERSION;
	m_pDataHeader->regionSize = DATA_REGION_SIZE;
	m_pDataHeader->prevItrLastPos = 0xFFFFFFFF;
	m_pDataHeader->headerCrc32 = crc32c_t(0, m_DataRegion, ((uint8*)m_pDataHeader - m_DataRegion) + offsetof(data_header, headerCrc32));
}

void CRehldsFlightRecorder::MoveToStart() {
	if (m_pRecorderState->curMessage == 0) {
		m_pDataHeader->prevItrLastPos = m_pRecorderState->wpos;
		m_pRecorderState->wpos = 0;
	} else {
		Q_memcpy(m_DataRegionPtr, m_DataRegionPtr + m_pRecorderState->lastMsgBeginPos, m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos);
		m_pRecorderState->wpos -= m_pRecorderState->lastMsgBeginPos;
		m_pDataHeader->prevItrLastPos = m_pRecorderState->lastMsgBeginPos;
		m_pRecorderState->lastMsgBeginPos = 0;
	}
}

void CRehldsFlightRecorder::StartMessage(uint16 msg, bool entrance) {
	if (msg == 0 || msg > m_pMetaHeader->numMessages) {
		Sys_Error("%s: Invalid message id %u", __func__, msg);
	}

	if (entrance) {
		msg = msg | 0x8000;
	}

	if (m_pRecorderState->curMessage != 0) {
		Sys_Error("%s: overlapping messages", __func__);
	}

	unsigned int sz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (sz < 6) {
		MoveToStart();
	}

	m_pRecorderState->curMessage = msg;
	m_pRecorderState->lastMsgBeginPos = m_pRecorderState->wpos;
	*(uint16*)(m_DataRegionPtr + m_pRecorderState->wpos) = msg;
	m_pRecorderState->wpos += 2;
}

void CRehldsFlightRecorder::EndMessage(uint16 msg, bool entrance) {
	if (entrance) {
		msg = msg | 0x8000;
	}

	if (m_pRecorderState->curMessage != msg) {
		Sys_Error("%s: invalid message %u", __func__, msg);
	}

	unsigned int freeSz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (freeSz < 2) {
		MoveToStart();
	}

	unsigned int msgSize = m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos;
	if (msgSize > MSG_MAX_SIZE) {
		Sys_Error("%s: too big message %u; size %u", __func__, msg, msgSize);
	}
	*(uint16*)(m_DataRegionPtr + m_pRecorderState->wpos) = msgSize;
	m_pRecorderState->wpos += 2;

	m_pRecorderState->curMessage = 0;
}

void CRehldsFlightRecorder::CheckSize(unsigned int wantToWriteLen) {
	unsigned int msgSize = m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos;
	if (msgSize + wantToWriteLen > MSG_MAX_SIZE) {
		Sys_Error("%s: too big message %u; size %u", __func__, m_pRecorderState->curMessage, msgSize);
	}
}

void CRehldsFlightRecorder::WriteBuffer(const void* data, unsigned int len) {
	if (m_pRecorderState->curMessage == 0) {
		Sys_Error("%s: Could not write, invalid state", __func__);
	}

	CheckSize(len);
	unsigned int freeSz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (freeSz < len) {
		MoveToStart();
	}

	Q_memcpy(m_DataRegionPtr + m_pRecorderState->wpos, data, len);
	m_pRecorderState->wpos += len;
}

void CRehldsFlightRecorder::WriteString(const char* s) {
	WriteBuffer(s, Q_strlen(s) + 1);
}

void CRehldsFlightRecorder::WriteInt8(int8 v) {
	WritePrimitive<int8>(v);
}

void CRehldsFlightRecorder::WriteUInt8(uint8 v) {
	WritePrimitive<uint8>(v);
}

void CRehldsFlightRecorder::WriteInt16(int16 v) {
	WritePrimitive<int16>(v);
}

void CRehldsFlightRecorder::WriteUInt16(uint16 v) {
	WritePrimitive<uint16>(v);
}

void CRehldsFlightRecorder::WriteInt32(int32 v) {
	WritePrimitive<int32>(v);
}

void CRehldsFlightRecorder::WriteUInt32(uint32 v) {
	WritePrimitive<uint32>(v);
}

void CRehldsFlightRecorder::WriteInt64(int64 v) {
	WritePrimitive<int64>(v);
}

void CRehldsFlightRecorder::WriteUInt64(uint64 v) {
	WritePrimitive<uint64>(v);
}

void CRehldsFlightRecorder::WriteFloat(float v) {
	WritePrimitive<float>(v);
}

void CRehldsFlightRecorder::WriteDouble(double v) {
	WritePrimitive<double>(v);
}

uint16 CRehldsFlightRecorder::RegisterMessage(const char* module, const char *message, unsigned int version, bool inOut) {
	if (m_pMetaHeader->numMessages >= MSG_MAX_ID) {
		Sys_Error("%s: can't register message; limit exceeded", __func__);
	}

	uint16 msgId = ++m_pMetaHeader->numMessages;

	sizebuf_t sb;
	sb.buffername = "FlightRecorded Meta";
	sb.cursize = m_pMetaHeader->metaRegionPos;
	sb.maxsize = META_REGION_MAIN_SIZE - META_REGION_HEADER;
	sb.flags = 0;
	sb.data = m_MetaRegionPtr;

	MSG_WriteByte(&sb, MRT_MESSAGE_DEF);
	MSG_WriteShort(&sb, msgId);
	MSG_WriteString(&sb, module);
	MSG_WriteString(&sb, message);
	MSG_WriteLong(&sb, version);
	MSG_WriteChar(&sb, inOut ? 1 : 0);

	m_pMetaHeader->metaRegionPos = sb.cursize;

	return msgId;
}

void CRehldsFlightRecorder::dump(const char* fname) {
	FILE* fl = fopen(fname, "wb");
	if (fl == NULL) {
		Con_Printf("Failed to write Flight Recorder dump: could not open '%s' for writing\n", fname);
		return;
	}

	fwrite(m_MetaRegion, META_REGION_SIZE, 1, fl);
	fwrite(m_DataRegion, DATA_REGION_SIZE, 1, fl);
	fclose(fl);

	Con_Printf("Written flightrecorder dump to '%s'\n", fname);
}
