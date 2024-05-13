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

// Constructs a Message object
MessageImpl::MessageImpl()
{
	m_buffer.buffername = "MessageManager/Begin/End";
	m_buffer.data       = m_bufferData;
	m_buffer.flags      = SIZEBUF_ALLOW_OVERFLOW;
	m_buffer.cursize    = 0;
	m_buffer.maxsize    = sizeof(m_bufferData);

	m_paramCount        = 0;
}

// Sets the active state of the message with the given parameters
void MessageImpl::setActive(int dest, int type, const float *origin, edict_t *edict)
{
	m_dest   = static_cast<Dest>(dest);
	m_type   = type;
	m_edict  = edict;

	// Reset buffer size
	m_buffer.flags   = SIZEBUF_ALLOW_OVERFLOW;
	m_buffer.cursize = 0;

	// Copy origin vector if provided
	if (origin)
		VectorCopy(origin, m_origin);
	else
		VectorClear(m_origin);
}

// Sets the buffer for the message
void MessageImpl::setBuffer(sizebuf_t *pbuf)
{
	// Copy data from the provided buffer to the message buffer
	memcpy(m_buffer.data, pbuf->data, pbuf->cursize);
	m_buffer.cursize = pbuf->cursize;
}

// Sets the copyback buffer for the message
void MessageImpl::setCopybackBuffer(sizebuf_t *pbuf)
{
	// Copy data from the message buffer back to the provided buffer
	memcpy(pbuf->data, m_buffer.data, m_buffer.cursize);
	pbuf->cursize = m_buffer.cursize;
}

// Clears the message parameters
void MessageImpl::clear()
{
	m_paramCount = 0;
}

// An array containing fixed sizes for various types of parameters
static size_t SIZEOF_PARAMTYPE[] =
{
	sizeof(uint8),  // Byte
	sizeof(int8),   // Char
	sizeof(int16),  // Short
	sizeof(uint32), // Long
	sizeof(uint8),  // Angle
	sizeof(int16),  // Coord
	0,              // String
	sizeof(int16),  // Entity
};

// Adds a parameter to the message
void MessageImpl::addParam(IMessage::ParamType type, size_t length)
{
	Param_t &param = m_params[m_paramCount++];
	param.type = type;
	param.len  = (length == -1) ? SIZEOF_PARAMTYPE[static_cast<size_t>(type)] : length;
	param.pos  = gMsgBuffer.cursize;
}

// Sets the value of a primitive parameter at the given index
template <typename T>
void MessageImpl::setParamPrimitive(size_t index, T value)
{
	// Ensure index is within bounds
	if (index < 0 || index >= m_paramCount)
		return;

	const Param_t &param = m_params[index];

	void *pbuf = m_buffer.data + param.pos;

	// Set value based on parameter type
	switch (param.type)
	{
	case IMessage::ParamType::Byte:
		*(uint8 *)pbuf = value;
		break;
	case IMessage::ParamType::Char:
		*(int8 *)pbuf = value;
		break;
	case IMessage::ParamType::Short:
	case IMessage::ParamType::Entity:
		*(int16 *)pbuf = value;
		break;
	case IMessage::ParamType::Long:
		*(uint16 *)pbuf = value;
		break;
	case IMessage::ParamType::Angle:
		// Convert angle value to byte representation with loss of precision
		*(uint8 *)pbuf = (int64)(fmod((double)value, 360.0) * 256.0 / 360.0) & 0xff;
		break;
	case IMessage::ParamType::Coord:
		// Convert coordinate value to short integer representation with loss of precision
		*(int16 *)pbuf = (int16)(int)(value * 8.0);
		break;
	default:
		return; // bad type
	}

	// Mark message as modified
	m_modified = true;
}

// Transforms the buffer after setting a string parameter at the given index
void MessageImpl::setTxformBuffer(size_t index, size_t startPos, size_t oldLength, size_t newLength)
{
	// Calculate the difference in length
	int32_t diffLength = newLength - oldLength;
	if (diffLength != 0)
	{
		// Check if the buffer size limit will be exceeded
		if (m_buffer.cursize + diffLength > m_buffer.maxsize)
		{
			Sys_Error(
				"%s: Refusing to transform string with %i param of user message of %i bytes, "
				"user message size limit is %i bytes\n", __func__, index, gMsgBuffer.cursize + diffLength, gMsgBuffer.maxsize);
		}

		// Move the data in the buffer
		size_t moveLength = m_buffer.cursize - (startPos + oldLength);
		Q_memmove(m_buffer.data + startPos + newLength, m_buffer.data + startPos + oldLength, moveLength);
		m_buffer.cursize += diffLength;

		// Update the position of all subsequent parameters
		for (size_t i = index + 1; i < m_paramCount; i++)
			m_params[i].pos += diffLength;
	}
}

// Returns the integer value of the parameter at the given index
int MessageImpl::getParamInt(size_t index) const
{
	// Ensure index is within bounds
	if (index < 0 || index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	void *buf = m_buffer.data + m_params[index].pos;
	switch (m_params[index].type)
	{
	case IMessage::ParamType::Byte:
		return *(uint8  *)buf;
	case IMessage::ParamType::Char:
		return *(int8   *)buf;
	case IMessage::ParamType::Short:
	case IMessage::ParamType::Entity:
		return *(int16  *)buf;
	case IMessage::ParamType::Long:
		return *(uint16 *)buf;
	default:
		return 0; // bad type
	}
}

// Returns the float value of the parameter at the given index
float MessageImpl::getParamFloat(size_t index) const
{
	// Ensure index is within bounds
	if (index < 0 || index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	const Param_t &param = m_params[index];
	void *buf = m_buffer.data + param.pos;
	switch (param.type)
	{
	case IMessage::ParamType::Angle:
		return (float)(*(uint8 *)buf * (360.0 / 256.0));
	case IMessage::ParamType::Coord:
		return (float)(*(int16 *)buf * (1.0 / 8));
	default:
		break; // bad type
	}

	return 0;
}

// Returns the string value of the parameter at the given index
const char *MessageImpl::getParamString(size_t index) const
{
	// Ensure index is within bounds
	if (index < 0 || index >= m_paramCount)
		return nullptr;

	// Get the parameter value if it is a string
	const Param_t &param = m_params[index];
	if (param.type == IMessage::ParamType::String)
		return (const char *)m_buffer.data + param.pos;

	return nullptr;
}

// Sets the integer value of the parameter at the given index
void MessageImpl::setParamInt(size_t index, int value)
{
	setParamPrimitive(index, value);
}

// Sets the float value of the parameter at the given index
void MessageImpl::setParamFloat(size_t index, float value)
{
	setParamPrimitive(index, value);
}

// Sets the vector value of the parameter at the given index
void MessageImpl::setParamVec(size_t index, const float *pos)
{
	if (!pos)
		return;

	// Ensure index is within bounds
	if (index < 0 || (index + 3) >= m_paramCount)
		return;

	// Get the parameter position in the buffer
	const Param_t &param = m_params[index];

	int16 *pbuf = (int16 *)m_buffer.data + param.pos;

	// Set each component of the vector parameter
	*(int16 *)pbuf++ = (int16)(pos[0] * 8.0);
	*(int16 *)pbuf++ = (int16)(pos[1] * 8.0);
	*(int16 *)pbuf++ = (int16)(pos[2] * 8.0);

	// Mark message as modified
	m_modified = true;
}

// Sets the string value of the parameter at the given index
void MessageImpl::setParamString(size_t index, const char *value)
{
	if (!value)
		return;

	// Ensure index is within bounds
	if (index < 0 || index >= m_paramCount)
		return;

	// Calculate the length of the string
	size_t length = Q_strlen(value) + 1;
	const Param_t &param = m_params[index];

	// Transform buffer to accommodate the new string length
	setTxformBuffer(index, param.pos, param.len, length);

	// Copy the string value to the buffer
	memcpy(m_buffer.data + param.pos, value, length);

	// Mark message as modified
	m_modified = true;
}

MessageManagerImpl::MessageManagerImpl()
{
	m_inblock = false;
	m_inhook  = false;
}

// Register hook function for the game message type
void MessageManagerImpl::registerHook(int msgType, hookfunc_t handler, int priority)
{
	m_hooks[msgType].registerHook(handler, priority);
}

// Unregister hook function for the game message type
void MessageManagerImpl::unregisterHook(int msgType, hookfunc_t handler)
{
	m_hooks[msgType].unregisterHook(handler);
}

// Get the block type for the game message type
IMessage::BlockType MessageManagerImpl::getMessageBlock(int msgType) const
{
	return m_blocks[msgType];
}

// Set the block type for the game message type
void MessageManagerImpl::setMessageBlock(int msgType, IMessage::BlockType blockType)
{
	m_blocks[msgType] = blockType;
}

bool MessageManagerImpl::MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	// Check if the message type is blocked
	if (m_blocks[msg_type] != IMessage::BlockType::Not)
	{
		m_inblock = true;
		return false;
	}

	// Check if there are hooks registered for the message type
	m_inhook = m_hooks[msg_type].getCount() > 0;

	if (m_inhook)
	{
		// Check for stack overflow
		if (m_stack.size() >= m_stack.max_size() - 1)
			Sys_Error("%s: stack overflow in #%i user message.\nIndicate potential recursive calls...\n", __func__, msg_type);

		// Push a new game message onto the stack
		m_stack.push();

		// Initialize the message
		MessageImpl &msg = m_stack.back();
		msg.setActive(msg_dest, msg_type, pOrigin, ed);
	}

	return true;
}

static void EXT_FUNC SendUserMessageData(IMessage *msg)
{
	// Set global variables with message data
	gMsgType   = msg->getType();
	gMsgEntity = msg->getEdict();
	gMsgDest   = static_cast<int>(msg->getDest());

	gMsgOrigin[0] = msg->getOrigin()[0];
	gMsgOrigin[1] = msg->getOrigin()[1];
	gMsgOrigin[2] = msg->getOrigin()[2];

	gMsgStarted = TRUE;

	// Copy message data to global buffer and call end of the hookchain
	msg->setCopybackBuffer(&gMsgBuffer);
	PF_MessageEnd_I();
}

bool MessageManagerImpl::MessageEnd()
{
	// Check if in block mode
	if (m_inblock)
	{
		m_inblock = false;

		// Unblock the message type if it was blocked once
		if (m_blocks[gMsgType] == IMessage::BlockType::Once)
			m_blocks[gMsgType] =  IMessage::BlockType::Not;

		return false;
	}

	// Check if not in hook
	if (!m_inhook)
		return true;

	gMsgStarted = FALSE;

	// Get the message from the top of the stack
	MessageImpl &msg = m_stack.back();

	// Set buffer from global buffer and call hookchain
	msg.setBuffer(&gMsgBuffer);
	m_hooks[msg.getType()].callChain(SendUserMessageData, &msg);
	m_inhook = false;

	// Clear the message and pop from the stack
	msg.clear();
	m_stack.pop();

	return false;
}

bool MessageManagerImpl::WriteParam(IMessage::ParamType type, size_t length)
{
	// Check if in block mode
	if (m_inblock)
		return false;

	// Check if in hook mode
	if (m_inhook)
	{
		// Add parameter to top stack message
		MessageImpl &msg = m_stack.back();
		msg.addParam(type, length);
	}

	return true;
}

//
// Functions intercept to handle messages
//

void EXT_FUNC PF_MessageBegin_Intercept(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	// Set global message type
	gMsgType = msg_type;

	// Begin message manager
	if (MessageManager().MessageBegin(msg_dest, msg_type, pOrigin, ed))
		PF_MessageBegin_I(msg_dest, msg_type, pOrigin, ed);
}

void EXT_FUNC PF_MessageEnd_Intercept(void)
{
	// End message manager
	if (MessageManager().MessageEnd())
		PF_MessageEnd_I(); // Call original message end function if the manager allows it
}

void EXT_FUNC PF_WriteByte_Intercept(int iValue)
{
	// Write byte parameter to the message if the manager allows it
	if (MessageManager().WriteParam(IMessage::ParamType::Byte))
		PF_WriteByte_I(iValue);
}

void EXT_FUNC PF_WriteChar_Intercept(int iValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Char))
		PF_WriteChar_I(iValue);
}

void EXT_FUNC PF_WriteShort_Intercept(int iValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Short))
		PF_WriteShort_I(iValue);
}

void EXT_FUNC PF_WriteLong_Intercept(int iValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Long))
		PF_WriteLong_I(iValue);
}

void EXT_FUNC PF_WriteAngle_Intercept(float flValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Angle))
		PF_WriteAngle_I(flValue);
}

void EXT_FUNC PF_WriteCoord_Intercept(float flValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Coord))
		PF_WriteCoord_I(flValue);
}

void EXT_FUNC PF_WriteString_Intercept(const char *sz)
{
	if (MessageManager().WriteParam(IMessage::ParamType::String, sz ? Q_strlen(sz) + 1 : 1))
		PF_WriteString_I(sz);
}

void EXT_FUNC PF_WriteEntity_Intercept(int iValue)
{
	if (MessageManager().WriteParam(IMessage::ParamType::Entity))
		PF_WriteEntity_I(iValue);
}

// Initialization function to set up function interception
void MessageManagerImpl::Init()
{
	// Set function callback to intercept functions
	g_engfuncsExportedToDlls.pfnMessageBegin = PF_MessageBegin_Intercept;
	g_engfuncsExportedToDlls.pfnWriteByte    = PF_WriteByte_Intercept;
	g_engfuncsExportedToDlls.pfnWriteChar    = PF_WriteChar_Intercept;
	g_engfuncsExportedToDlls.pfnWriteShort   = PF_WriteShort_Intercept;
	g_engfuncsExportedToDlls.pfnWriteLong    = PF_WriteLong_Intercept;
	g_engfuncsExportedToDlls.pfnWriteAngle   = PF_WriteAngle_Intercept;
	g_engfuncsExportedToDlls.pfnWriteCoord   = PF_WriteCoord_Intercept;
	g_engfuncsExportedToDlls.pfnWriteString  = PF_WriteString_Intercept;
	g_engfuncsExportedToDlls.pfnWriteEntity  = PF_WriteEntity_Intercept;
	g_engfuncsExportedToDlls.pfnMessageEnd   = PF_MessageEnd_Intercept;
}

MessageManagerImpl &MessageManager()
{
	// Instance of the message manager singleton
	static MessageManagerImpl instance{};
	return instance;
}
