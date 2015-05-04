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
	m_MetaRegionPtr = (uint8_t*) sys_allocmem(META_REGION_SIZE);
	m_DataRegionPtr = (uint8_t*) sys_allocmem(DATA_REGION_SIZE);

	if (!m_MetaRegionPtr || !m_DataRegionPtr) {
		rehlds_syserror("%s: direct allocation failed", __FUNCTION__);
	}

	//initialize meta region header
	char* metaPos = (char*)m_MetaRegionPtr;
	const char* metaSignature = "REHLDS_FLIGHTREC_META";
	metaPos += sprintf(metaPos, "%s%s%s:", metaSignature, metaSignature, metaSignature);

	m_pMetaHeader = (meta_header*)metaPos;
	metaPos += sizeof(meta_header);
	m_pRecorderState = (recorder_state*)metaPos;
	metaPos += sizeof(recorder_state);

	if ((metaPos - (char*)m_MetaRegionPtr) > META_REGION_HEADER) {
		rehlds_syserror("%s: Meta header overflow", __FUNCTION__);
	}

	//initialize data region header
	char* dataPos = (char*)m_DataRegionPtr;
	const char* dataSignature = "REHLDS_FLIGHTREC_DATA";
	dataPos += sprintf(dataPos, "%s%s%s:", dataSignature, dataSignature, dataSignature);

	m_pDataHeader = (data_header*)dataPos;
	dataPos += sizeof(data_header);

	if ((dataPos - (char*)m_pDataHeader) > DATA_REGION_HEADER) {
		rehlds_syserror("%s: Data header overflow", __FUNCTION__);
	}

	InitHeadersContent();

	m_MetaRegionPtr += META_REGION_HEADER;
	m_DataRegionPtr += DATA_REGION_HEADER;
}

void CRehldsFlightRecorder::InitHeadersContent() {
	m_pMetaHeader->version = FLIGHT_RECORDER_VERSION;
	m_pMetaHeader->metaRegionPos = 0;
	m_pMetaHeader->numMessages = 0;

	m_pRecorderState->wpos = 0;
	m_pRecorderState->lastMsgBeginPos = 0xFFFFFFFF;
	m_pRecorderState->curMessage = 0;

	m_pDataHeader->version = FLIGHT_RECORDER_VERSION;
	m_pDataHeader->prevItrLastPos = 0xFFFFFFFF;
}

void CRehldsFlightRecorder::MoveToStart() {
	if (m_pRecorderState->curMessage == 0) {
		m_pDataHeader->prevItrLastPos = m_pRecorderState->wpos;
		m_pRecorderState->wpos = 0;
	} else {
		memcpy(m_DataRegionPtr, m_DataRegionPtr + m_pRecorderState->lastMsgBeginPos, m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos);
		m_pRecorderState->wpos -= m_pRecorderState->lastMsgBeginPos;
		m_pDataHeader->prevItrLastPos = m_pRecorderState->lastMsgBeginPos;
		m_pRecorderState->lastMsgBeginPos = 0;
	}
}

void CRehldsFlightRecorder::StartMessage(uint16_t msg, bool entrance) {
	if (msg == 0 || msg > m_pMetaHeader->numMessages) {
		rehlds_syserror("%s: Invalid message id %u", __FUNCTION__, msg);
	}

	if (entrance) {
		msg = msg | 0x8000;
	}

	if (m_pRecorderState->curMessage != 0) {
		rehlds_syserror("%s: overlapping messages", __FUNCTION__);
	}

	unsigned int sz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (sz < 6) {
		MoveToStart();
	}



	m_pRecorderState->curMessage = msg;
	m_pRecorderState->lastMsgBeginPos = m_pRecorderState->wpos;
	*(uint16_t*)(m_DataRegionPtr + m_pRecorderState->wpos) = msg;
	m_pRecorderState->wpos += 2;
}

void CRehldsFlightRecorder::EndMessage(uint16_t msg, bool entrance) {
	if (entrance) {
		msg = msg | 0x8000;
	}

	if (m_pRecorderState->curMessage != msg) {
		rehlds_syserror("%s: invalid message %u", __FUNCTION__, msg);
	}

	unsigned int freeSz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (freeSz < 2) {
		MoveToStart();
	}

	unsigned int msgSize = m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos;
	if (msgSize > MSG_MAX_SIZE) {
		rehlds_syserror("%s: too big message %u; size%u", __FUNCTION__, msg, msgSize);
	}
	*(uint16_t*)(m_DataRegionPtr + m_pRecorderState->wpos) = msgSize;
	m_pRecorderState->wpos += 2;

	m_pRecorderState->curMessage = 0;
}

void CRehldsFlightRecorder::CheckSize(unsigned int wantToWriteLen) {
	unsigned int msgSize = m_pRecorderState->wpos - m_pRecorderState->lastMsgBeginPos;
	if (msgSize + wantToWriteLen > MSG_MAX_SIZE) {
		rehlds_syserror("%s: too big message %u; size%u", __FUNCTION__, m_pRecorderState->curMessage, msgSize);
	}
}

void CRehldsFlightRecorder::WriteBuffer(const void* data, unsigned int len) {
	if (m_pRecorderState->curMessage == 0) {
		rehlds_syserror("%s: Could not write, invalid state", __FUNCTION__);
	}

	CheckSize(len);
	unsigned int freeSz = DATA_REGION_MAIN_SIZE - m_pRecorderState->wpos;
	if (freeSz < len) {
		MoveToStart();
	}

	memcpy(m_DataRegionPtr + m_pRecorderState->wpos, data, len);
	m_pRecorderState->wpos += len;
}

void CRehldsFlightRecorder::WriteString(const char* s) {
	WriteBuffer(s, strlen(s) + 1);
}

void CRehldsFlightRecorder::WriteInt8(int8_t v) {
	WritePrimitive<int8_t>(v);
}

void CRehldsFlightRecorder::WriteUInt8(uint8_t v) {
	WritePrimitive<uint8_t>(v);
}

void CRehldsFlightRecorder::WriteInt16(int16_t v) {
	WritePrimitive<int16_t>(v);
}

void CRehldsFlightRecorder::WriteUInt16(uint16_t v) {
	WritePrimitive<uint16_t>(v);
}

void CRehldsFlightRecorder::WriteInt32(int32_t v) {
	WritePrimitive<int32_t>(v);
}

void CRehldsFlightRecorder::WriteUInt32(uint32_t v) {
	WritePrimitive<uint32_t>(v);
}

void CRehldsFlightRecorder::WriteInt64(int64_t v) {
	WritePrimitive<int64_t>(v);
}

void CRehldsFlightRecorder::WriteUInt64(uint64_t v) {
	WritePrimitive<uint64_t>(v);
}

void CRehldsFlightRecorder::WriteFloat(float v) {
	WritePrimitive<float>(v);
}

void CRehldsFlightRecorder::WriteDouble(double v) {
	WritePrimitive<double>(v);
}

uint16_t CRehldsFlightRecorder::RegisterMessage(const char* module, const char *message, unsigned int version, bool inOut) {
	if (m_pMetaHeader->numMessages >= MSG_MAX_ID) {
		rehlds_syserror("%s: can't register message; limit exceeded", __FUNCTION__);
	}

	uint16_t msgId = ++m_pMetaHeader->numMessages;

	sizebuf_t sb;
	sb.buffername = "FlightRecorded Meta";
	sb.cursize = m_pMetaHeader->metaRegionPos;
	sb.maxsize = META_REGION_MAIN_SIZE;
	sb.flags = 0;
	sb.data = m_MetaRegionPtr;

	MSG_WriteShort(&sb, msgId);
	MSG_WriteString(&sb, module);
	MSG_WriteString(&sb, message);
	MSG_WriteLong(&sb, version);
	MSG_WriteChar(&sb, inOut ? 1 : 0);

	m_pMetaHeader->metaRegionPos = sb.cursize;

	return msgId;
}
