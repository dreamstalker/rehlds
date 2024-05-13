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

#include "IMessageManager.h"
#include "hookchains.h"

/**
 * @brief Implementation interface for defining message parameters and behavior for a game message object
 */
class MessageImpl: public IMessage
{
public:
	MessageImpl();
	~MessageImpl() = default;

	/**
	 * @brief Returns the number of parameters in the message
	 * @return The number of parameters
	 */
	int getParamCount() const { return m_paramCount; }

	/**
	 * @brief Returns the type of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The type of the parameter
	 */
	ParamType getParamType(size_t index) const { return m_params[index].type; }

	/**
	 * @brief Returns the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The integer value of the parameter
	 */
	int getParamInt(size_t index) const;

	/**
	 * @brief Returns the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The float value of the parameter
	 */
	float getParamFloat(size_t index) const;

	/**
	 * @brief Returns the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The string value of the parameter
	 */
	const char *getParamString(size_t index) const;

	/**
	 * @brief Sets the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The integer value to set
	 */
	void setParamInt(size_t index, int value);

	/**
	 * @brief Sets the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The float value to set
	 */
	void setParamFloat(size_t index, float value);

	/**
	 * @brief Sets the vector value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param pos The vector value to set
	 */
	void setParamVec(size_t index, const float *pos);

	/**
	 * @brief Sets the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param string The string value to set
	 */
	void setParamString(size_t index, const char *string);

	/**
	 * @brief Returns the destination of the message
	 * @return The destination of the message
	 */
	Dest getDest() const { return m_dest; }

	/**
	 * @brief Returns the type of the message
	 * @return The type of the message
	 */
	int getType() const { return m_type; }

	/**
	 * @brief Returns the origin of the message
	 * @return The origin of the message
	 */
	const float *getOrigin() const { return m_origin; }

	/**
	 * @brief Returns the edict associated with the message
	 * @return The edict associated with the message
	 */
	edict_t *getEdict() const { return m_edict; }

	/**
	 * @brief Returns whether the message has been modified
	 * @return True if the message has been modified, false otherwise
	 */
	bool isModified() const { return m_modified; }

private:

	friend class MessageManagerImpl;

	// Sets the active state of the message with the given parameters
	void setActive(int dest, int type, const float *origin, edict_t *edict);

	// Sets the buffer for the message
	void setBuffer(sizebuf_t *pbuf);

	// Set the copyback buffer for the message
	void setCopybackBuffer(sizebuf_t *pbuf);

	// Adds a parameter to the message
	void addParam(IMessage::ParamType type, size_t length);

	// Clears the message after execution
	void clear();

	template <typename T>
	void setParamPrimitive(size_t index, T value);

	// Transforms buffer after sets string for a parameter at the given index
	void setTxformBuffer(size_t index, size_t startPos, size_t oldLength, size_t newLength);

	bool		m_modified;  // Flag indicating whether the message has been modified
	Dest		m_dest;      // The destination of the message
	int			m_type;      // The type of the message
	float		m_origin[3]; // The origin of the message
	edict_t*	m_edict;     // The edict associated with the message

	uint8		m_bufferData[512]; // The buffer data for the message 'm_buffer'
	sizebuf_t	m_buffer;          // The buffer for the message

	struct Param_t
	{
		ParamType	type; // The type of the parameter
		size_t		pos;  // The position of the parameter in the buffer
		size_t		len;  // The length of the parameter in the buffer
	};

	static const size_t MAX_PARAMS = 16; // The maximum number of parameters allowed in the message
	Param_t m_params[MAX_PARAMS];        // The array of parameters in the message
	size_t  m_paramCount;                // The number of parameters in the message
};

/**
 * @brief Implementation interface manages hooks and blocking behavior game messages
 */
class MessageManagerImpl: public IMessageManager
{
public:

	void Init();

	MessageManagerImpl();
	~MessageManagerImpl() = default;

	/**
	 * @brief Returns the major version of the MessageManager
	 * @return The major version
	 */
	int getMajorVersion() const { return MESSAGEMNGR_VERSION_MAJOR; }

	/**
	 * @brief Returns the minor version of the MessageManager
	 * @return The minor version
	 */
	int getMinorVersion() const { return MESSAGEMNGR_VERSION_MINOR; }

	/**
	 * @brief Returns the blocking behavior for the given message type
	 * @param msgType The message type
	 * @return The blocking behavior for the given message type
	 */
	IMessage::BlockType getMessageBlock(int msgType) const;

	/**
	 * @brief Sets the blocking behavior for the given message type
	 * @param msgType The message type
	 * @param blockType The blocking behavior to set
	 */
	void setMessageBlock(int msgType, IMessage::BlockType blockType);

	/**
	 * @brief Registers a hook function for the given message type
	 * @param msgType The message type to register the hook for
	 * @param handler The hook function to register
	 * @param priority The priority of the hook function (see enum HookChainPriority)
	 */
	void registerHook(int msgType, hookfunc_t handler, int priority = HC_PRIORITY_DEFAULT);

	/**
	 * @brief Unregisters a hook function for the given message type
	 * @param msgType The message type to unregister the hook for
	 * @param handler The hook function to unregister
	 */
	void unregisterHook(int msgType, hookfunc_t handler);

private:
	friend void PF_MessageBegin_Intercept(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
	friend void PF_MessageEnd_Intercept();
	friend void PF_WriteByte_Intercept(int iValue);
	friend void PF_WriteChar_Intercept(int iValue);
	friend void PF_WriteShort_Intercept(int iValue);
	friend void PF_WriteLong_Intercept(int iValue);
	friend void PF_WriteAngle_Intercept(float flValue);
	friend void PF_WriteCoord_Intercept(float flValue);
	friend void PF_WriteString_Intercept(const char *sz);
	friend void PF_WriteEntity_Intercept(int iValue);

	bool MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
	bool MessageEnd();

private:
	bool WriteParam(IMessage::ParamType type, size_t length = -1);

	bool m_inblock; // Flag indicating whether a message block is currently active
	bool m_inhook;  // Flag indicating whether a message hook is currently active

	/**
	 * @brief Helper a templated Stack class to manage a stack of Message objects
	 * @tparam T The type of objects stored in the stack
	 * @tparam MAX The maximum size of the stack
	 */
	template <typename T, size_t MAX>
	class Stack
	{
	public:
		void push() { _size++; }
		void pop () { _size--; }

		size_t size()     const { return _size; }
		size_t max_size() const { return MAX;   }

		const T &back()   const { return _data[_size - 1]; }
		      T &back()         { return _data[_size - 1]; }
	private:
		size_t _size = 0u;
		T _data[MAX]{};
	};

	static const size_t MAX_MSGSTACK = 16; // The maximum size of the message stack, 16 it should be enough
	Stack<MessageImpl, MAX_MSGSTACK> m_stack;

	IVoidHookChainRegistryImpl<IMessage *> m_hooks [MAX_USERMESSAGES]{}; // The array of hook chain registries for each message type
	IMessage::BlockType m_blocks[MAX_USERMESSAGES]{}; // The array of blocking behaviors for each message type
};

/**
 * @brief The singleton instance of the MessageManager
 */
extern MessageManagerImpl &MessageManager();
