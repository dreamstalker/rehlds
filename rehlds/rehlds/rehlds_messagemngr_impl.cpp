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
	m_paramCount = 0;
	m_modifiedDataBits = 0;
}

// Sets the active state of the message with the given parameters
void MessageImpl::setActive(int dest, int id, const float *origin, edict_t *edict)
{
	// Initialize storage buffers
	for (int i = 0; i < MAX_STORAGE; i++)
	{
		Storage_t &storage  = m_Storage[i];
		storage.buf.cursize = 0;
		storage.dest        = static_cast<Dest>(dest);
		storage.msgid       = id;
		storage.edict       = edict;

		// Copy origin vector if provided
		if (origin)
			VectorCopy(origin, storage.origin);
		else
			VectorClear(storage.origin);
	}

	m_paramCount = 0;
	m_modifiedDataBits = 0;
}

// Sets the buffer for the message
void MessageImpl::setBuffer(sizebuf_t *pbuf)
{
	// Copy data from the provided buffer to the message buffer
	for (int i = 0; i < MAX_STORAGE; i++)
	{
		Storage_t &storage = m_Storage[i];
		Q_memcpy(storage.buf.data, pbuf->data, pbuf->cursize);
		storage.buf.cursize = pbuf->cursize;
	}
}

// Sets the copyback buffer for the message
void MessageImpl::setCopybackBuffer(sizebuf_t *pbuf)
{
	const Storage_t &storage = m_Storage[BACK];

	// Copy data from the message buffer back to the provided buffer
	Q_memcpy(pbuf->data, storage.buf.data, storage.buf.cursize);
	pbuf->cursize = storage.buf.cursize;
}

// Clears the message parameters
void MessageImpl::clear()
{
	m_paramCount = 0;
	m_modifiedDataBits = 0;
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
	param.type     = type;
	param.newlen   = param.oldlen = (length == -1) ? SIZEOF_PARAMTYPE[static_cast<size_t>(type)] : length;
	param.posBack  = param.posFront = gMsgBuffer.cursize;
}

// Sets the value of a primitive parameter at the given index
template <typename T>
void MessageImpl::setParamPrimitive(size_t index, T value)
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return;

	Param_t &param = m_params[index];
	void *pbuf = m_Storage[BACK].buf.data + param.posBack;

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
		*(uint32 *)pbuf = value;
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
	param.modified = true;

	// Mark the overall status as changed
	setModifiedDataBit(DataType::Param);
}

// Transforms the buffer after setting a string parameter at the given index
void MessageImpl::setTxformBuffer(size_t index, size_t startPos, size_t oldLength, size_t newLength)
{
	// Calculate the difference in length
	int32_t diffLength = newLength - oldLength;
	if (diffLength != 0)
	{
		sizebuf_t &buf = m_Storage[BACK].buf;

		// Check if the buffer size limit will be exceeded
		if (buf.cursize + diffLength > buf.maxsize)
		{
			Sys_Error(
				"%s: Refusing to transform string with %i param of user message of %i bytes, "
				"user message size limit is %i bytes\n", __func__, index, gMsgBuffer.cursize + diffLength, gMsgBuffer.maxsize);
		}

		// Move the data in the buffer
		size_t moveLength = buf.cursize - (startPos + oldLength);
		if (moveLength > 0)
			Q_memmove(buf.data + startPos + newLength, buf.data + startPos + oldLength, moveLength);

		buf.cursize += diffLength;

		if (newLength < oldLength)
			Q_memset(buf.data + startPos + newLength + moveLength, 0, oldLength - newLength);

		// Update the position of all subsequent parameters
		for (size_t i = index + 1; i < m_paramCount; i++)
			m_params[i].posBack += diffLength;
	}
}

// Returns the integer value of the parameter at the given index
int MessageImpl::getParamInt(size_t index) const
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	const Param_t &param = m_params[index];
	const void *buf = m_Storage[BACK].buf.data + param.posBack;
	switch (param.type)
	{
	case IMessage::ParamType::Byte:
		return *(uint8  *)buf;
	case IMessage::ParamType::Char:
		return *(int8   *)buf;
	case IMessage::ParamType::Short:
	case IMessage::ParamType::Entity:
		return *(int16  *)buf;
	case IMessage::ParamType::Long:
		return *(uint32 *)buf;
	default:
		return 0; // bad type
	}
}

// Returns the float value of the parameter at the given index
float MessageImpl::getParamFloat(size_t index) const
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	const Param_t &param = m_params[index];
	const void *buf = m_Storage[BACK].buf.data + param.posBack;
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
	if (index >= m_paramCount)
		return nullptr;

	// Get the parameter value if it is a string
	const Param_t &param = m_params[index];
	if (param.type == IMessage::ParamType::String)
		return (const char *)m_Storage[BACK].buf.data + param.posBack;

	return nullptr;
}

int MessageImpl::getOriginalParamInt(size_t index) const
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	const Param_t &param = m_params[index];
	const void *buf = m_Storage[FRONT].buf.data + param.posFront;
	switch (param.type)
	{
	case IMessage::ParamType::Byte:
		return *(uint8  *)buf;
	case IMessage::ParamType::Char:
		return *(int8   *)buf;
	case IMessage::ParamType::Short:
	case IMessage::ParamType::Entity:
		return *(int16  *)buf;
	case IMessage::ParamType::Long:
		return *(uint32 *)buf;
	default:
		return 0; // bad type
	}
}

float MessageImpl::getOriginalParamFloat(size_t index) const
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return 0;

	// Get the parameter value based on its type
	const Param_t &param = m_params[index];
	const void *buf = m_Storage[FRONT].buf.data + param.posFront;
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

const char *MessageImpl::getOriginalParamString(size_t index) const
{
	// Ensure index is within bounds
	if (index >= m_paramCount)
		return nullptr;

	// Get the parameter value if it is a string
	const Param_t &param = m_params[index];
	if (param.type == IMessage::ParamType::String)
		return (const char *)m_Storage[FRONT].buf.data + param.posFront;

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
	if ((index + 3) >= m_paramCount)
		return;

	// Get the parameter position in the buffer
	Param_t &param = m_params[index];

	int16 *pbuf = (int16 *)m_Storage[BACK].buf.data + param.posBack;

	// Set each component of the vector parameter
	*(int16 *)pbuf++ = (int16)(pos[0] * 8.0);
	*(int16 *)pbuf++ = (int16)(pos[1] * 8.0);
	*(int16 *)pbuf++ = (int16)(pos[2] * 8.0);

	// Mark message as modified
	param.modified = true;

	// Mark the overall status as modified
	setModifiedDataBit(DataType::Param);
}

// Sets the string value of the parameter at the given index
void MessageImpl::setParamString(size_t index, const char *value)
{
	if (!value)
		return;

	// Ensure index is within bounds
	if (index >= m_paramCount)
		return;

	// Calculate the length of the string
	Param_t &param = m_params[index];

	param.newlen = Q_strlen(value) + 1;

	// Transform buffer to accommodate the new string length
	setTxformBuffer(index, param.posBack, param.oldlen, param.newlen);

	// Copy the string value to the buffer
	Q_memcpy(m_Storage[BACK].buf.data + param.posBack, value, param.newlen);

	// Mark message as modified
	param.modified = true;

	// Mark the overall status as modified
	setModifiedDataBit(DataType::Param);
}

// Sets the destination of the message
void MessageImpl::setDest(Dest dest)
{
	m_Storage[BACK].dest = dest;
	setModifiedDataBit(DataType::Dest);
}

// Sets the type of the message
void MessageImpl::setId(int msg_id)
{
	m_Storage[BACK].msgid = msg_id;
	setModifiedDataBit(DataType::Index);
}

// Sets the origin of the message
void MessageImpl::setOrigin(const float *origin)
{
	// Copy origin vector if provided
	if (origin)
		VectorCopy(origin, m_Storage[BACK].origin);
	else
		VectorClear(m_Storage[BACK].origin);

	setModifiedDataBit(DataType::Origin);
}

//Sets the edict associated with the message
void MessageImpl::setEdict(edict_t *pEdict)
{
	m_Storage[BACK].edict = pEdict;
	setModifiedDataBit(DataType::Edict);
}

bool MessageImpl::isDataModified(DataType type, size_t index) const
{
	if (!isDataTypeModified(type))
		return false;

	if (type == DataType::Param && index != -1)
	{
		// Ensure index is within bounds
		if (index >= m_paramCount)
			return false;

		const Param_t &param = m_params[index];
		return param.modified;
	}

	return true;
}

void MessageImpl::resetParam(size_t index)
{
	Param_t &param = m_params[index];

	void *pbackbuf = m_Storage[BACK].buf.data + param.posBack;
	const void *pfrontbuf = m_Storage[FRONT].buf.data + param.posFront;

	// Set value based on parameter type
	switch (param.type)
	{
	case IMessage::ParamType::Byte:
		*(uint8 *)pbackbuf = *(uint8 *)pfrontbuf;
		break;
	case IMessage::ParamType::Char:
		*(int8 *)pbackbuf = *(int8 *)pfrontbuf;
		break;
	case IMessage::ParamType::Short:
	case IMessage::ParamType::Entity:
		*(int16 *)pbackbuf = *(int16 *)pfrontbuf;
		break;
	case IMessage::ParamType::Long:
		*(uint32 *)pbackbuf = *(uint32 *)pfrontbuf;
		break;
	case IMessage::ParamType::Angle:
		*(uint8 *)pbackbuf = *(uint8 *)pfrontbuf;
		break;
	case IMessage::ParamType::Coord:
		*(int16 *)pbackbuf = *(int16 *)pfrontbuf;
		break;
	case IMessage::ParamType::String:
		// Return the original string value from the front buffer
		setTxformBuffer(index, param.posBack, param.newlen, param.oldlen);
		Q_memcpy(pbackbuf, pfrontbuf, param.oldlen);
		param.newlen = param.oldlen;
		break;
	default:
		return; // bad type
	}

	// Unmark message as modified
	param.modified = false;
}

// Resets a specific message parameter to its original value
bool MessageImpl::resetModifiedData(DataType type, size_t index)
{
	Storage_t &storageBack = m_Storage[BACK];
	const Storage_t &storageFront = m_Storage[FRONT];

	unsetModifiedDataBit(type);

	switch (type)
	{
	// Resets all message parameters and storage data to their original values
	case DataType::Any:
	{
		// Update the position of all subsequent parameters
		for (size_t i = 0; i < m_paramCount; i++)
		{
			Param_t &param = m_params[i];
			param.posBack  = param.posFront;
			param.newlen   = param.oldlen;
			param.modified = false; // Unmark message as modified
		}

		// Copy front storage data to back buffer data
		Q_memcpy(storageBack.buf.data, storageFront.buf.data, storageFront.buf.maxsize);

		storageBack.dest  = storageFront.dest;
		storageBack.msgid = storageFront.msgid;
		storageBack.edict = storageFront.edict;

		VectorCopy(storageFront.origin, storageBack.origin);

		m_modifiedDataBits = 0;

		break;
	}
	case DataType::Dest:
		storageBack.dest = storageFront.dest;
		break;
	case DataType::Index:
		storageBack.msgid = storageFront.msgid;
		break;
	case DataType::Origin:
		VectorCopy(storageFront.origin, storageBack.origin);
		break;
	case DataType::Edict:
		storageBack.edict = storageFront.edict;
		break;
	case DataType::Param:
	{
		// Reset a specific parameter
		if (index != -1)
		{
			// Ensure index is within bounds
			if (index < m_paramCount)
				resetParam(index);
		}
		else
		{
			for (size_t i = 0; i < m_paramCount; i++)
				resetParam(i);
		}

		// Recalc modified data bits
		for (size_t i = 0; i < m_paramCount; i++)
		{
			const Param_t &param = m_params[i];
			if (param.modified)
			{
				setModifiedDataBit(DataType::Param);
				break;
			}
		}

		break;
	}
	default:
		return false;
	}

	// If there was any other modified data, mark Any as overall modified data
	if (m_modifiedDataBits != 0)
		setModifiedDataBit(DataType::Any);

	return true;
}

MessageManagerImpl::MessageManagerImpl() : m_stack(m_pool)
{
	m_inblock = false;
	m_inhook  = false;
}

// Register hook function for the game message type
void MessageManagerImpl::registerHook(int msg_id, hookfunc_t handler, int priority)
{
	if (!m_hooks[msg_id])
		m_hooks[msg_id] = new HookRegistry_t;

	if (m_hooks[msg_id]->findHook((void *)handler))
		return; // already registered

	m_hooks[msg_id]->registerHook(handler, priority);
}

// Unregister hook function for the game message type
void MessageManagerImpl::unregisterHook(int msg_id, hookfunc_t handler)
{
	if (!m_hooks[msg_id])
		return;

	m_hooks[msg_id]->unregisterHook(handler);

	if (m_hooks[msg_id]->getCount() == 0)
	{
		delete m_hooks[msg_id];
		m_hooks[msg_id] = nullptr;
		m_pool.clear();
	}
}

// Get the block type for the game message type
IMessage::BlockType MessageManagerImpl::getMessageBlock(int msg_id) const
{
	return m_blocks[msg_id];
}

// Set the block type for the game message type
void MessageManagerImpl::setMessageBlock(int msg_id, IMessage::BlockType blockType)
{
	m_blocks[msg_id] = blockType;
}

bool MessageManagerImpl::MessageBegin(int msg_dest, int msg_id, const float *pOrigin, edict_t *ed)
{
	// Check if the message type is blocked
	if (m_blocks[msg_id] != IMessage::BlockType::Not)
	{
		m_inblock = true;
		return false;
	}

	// Check if there are hooks registered for the message type
	m_inhook = (m_hooks[msg_id] && m_hooks[msg_id]->getCount() > 0);

	if (m_inhook)
	{
		// Check for stack overflow
		if (m_stack.size() >= m_stack.max_size() - 1)
			Sys_Error("%s: stack overflow in #%i user message.\nIndicate potential recursive calls...\n", __func__, msg_id);

		// Push a new game message onto the stack
		MessageImpl &msg = m_stack.push();

		// Initialize the message
		msg.setActive(msg_dest, msg_id, pOrigin, ed);
	}

	return true;
}

static void EXT_FUNC SendUserMessageData(IMessage *msg)
{
	// Set global variables with message data
	gMsgType   = msg->getId();
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
	MessageImpl &msg = m_stack.top();

	// Set buffer from global buffer and call hookchain
	msg.setBuffer(&gMsgBuffer);
	m_hooks[msg.getId()]->callChain(SendUserMessageData, &msg);
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
		MessageImpl &msg = m_stack.top();
		msg.addParam(type, length);
	}

	return true;
}

//
// Functions intercept to handle messages
//

void EXT_FUNC PF_MessageBegin_Intercept(int msg_dest, int msg_id, const float *pOrigin, edict_t *ed)
{
	// Set global message type
	gMsgType = msg_id;

	// Begin message manager
	if (MessageManager().MessageBegin(msg_dest, msg_id, pOrigin, ed))
		PF_MessageBegin_I(msg_dest, msg_id, pOrigin, ed);
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
