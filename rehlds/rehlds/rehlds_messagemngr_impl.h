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

#include <memory> // std::unique_ptr
#include <algorithm> // std::move

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
	 * @brief Returns the original integer value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original integer value of the parameter
	 */
	int getOriginalParamInt(size_t index) const;

	/**
	 * @brief Returns the original float value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original float value of the parameter
	 */
	float getOriginalParamFloat(size_t index) const;

	/**
	 * @brief Returns the original string value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original string value of the parameter
	 */
	const char* getOriginalParamString(size_t index) const;

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
	 * @brief Sets the destination of the message
	 */
	void setDest(Dest dest);

	/**
	 * @brief Returns the destination of the message
	 * @return The destination of the message
	 */
	Dest getDest() const { return m_Storage[BACK].dest; }
	Dest getOriginalDest() const { return m_Storage[FRONT].dest; }

	/**
	 * @brief Sets the index of the message
	 */
	void setId(int msg_id);

	/**
	 * @brief Returns the index of the message
	 * @return The index of the message
	 */
	int getId() const { return m_Storage[BACK].msgid;  }
	int getOriginalId() const { return m_Storage[FRONT].msgid; }

	/**
	 * @brief Sets the origin of the message
	 */
	void setOrigin(const float *origin);

	/**
	 * @brief Returns the origin of the message
	 * @return The origin of the message
	 */
	const float *getOrigin() const { return m_Storage[BACK].origin;  }
	const float *getOriginalOrigin() const { return m_Storage[FRONT].origin; }

	/**
	 * @brief Sets the edict associated with the message
	 */
	void setEdict(edict_t *pEdict);

	/**
	 * @brief Returns the edict associated with the message
	 * @return The edict associated with the message
	 */
	edict_t *getEdict() const { return m_Storage[BACK].edict; }
	edict_t *getOriginalEdict() const { return m_Storage[FRONT].edict; }

	/**
	 * @brief Checks if the specified type of message data has been modified
	 *
	 * This function allows you to check if any part of the message data, such as its
	 * destination, type, origin, edict, or any specific parameter, has been modified
	 *
	 * @param type The type of the data to check for modification
	 * This can be one of the following:
	 *             - DataType::Any:      Check if any part of the message has been modified
	 *             - DataType::Dest:     Check if the destination has been modified
	 *             - DataType::Index:    Check if the message ID has been modified
	 *             - DataType::Origin:   Check if the origin has been modified
	 *             - DataType::Edict:    Check if the edict pointer has been modified
	 *             - DataType::Param:    Check if a specific parameter has been modified
	 *
	 * @param index The index of the parameter to check for modification (used only when type is DataType::Param)
	 *              Default value is -1, which means the parameter index is not applicable
	 *
	 * @return True if the specified data type has been modified, false otherwise
	 */
	bool isDataModified(DataType type, size_t index) const;

	/**
	 * @brief Resets a specific type of message data to its original value
	 *
	 * @param type The type of data to reset to its original value
	 *             This can be one of the following:
	 *             - DataType::Any:      Reset all modified message data to its original values
	 *             - DataType::Dest:     Reset the destination to its original value
	 *             - DataType::Index:    Reset the message ID to its original value
	 *             - DataType::Origin:   Reset the origin to its original value
	 *             - DataType::Edict:    Reset the edict pointer of the recipient client to its original value
	 *             - DataType::Param:    Reset a specific parameter to its original value
	 *
	 * @param index The index of the parameter to reset (used only when type is DataType::Param)
	 *              Default value is -1, which means the parameter index is not applicable
	 *
	 * @return True if the modified data type was reset, false otherwise
	 */
	bool resetModifiedData(DataType type, size_t index = -1);

private:

	friend class MessageManagerImpl;

	// Sets the active state of the message with the given parameters
	void setActive(int dest, int id, const float *origin, edict_t *edict);

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

	enum
	{
		FRONT, // Original buffer storage data
		BACK,  // Current modifiable buffer storage data
		MAX_STORAGE
	};

	struct Storage_t
	{
		// The buffer storage data for the message 'm_buffer' (variable-length message limited to one byte is 256)
		uint8_t bufData[256]{};

		// The buffer for the message
		sizebuf_t buf = { "MsgMngr/Begin/End", SIZEBUF_ALLOW_OVERFLOW, bufData, sizeof(bufData), 0 };

		// The destination of the message
		Dest dest{};

		// The index of the message
		int msgid{0};

		// The origin of the message
		float origin[3]{0,0,0};

		// The edict associated with the message
		edict_t *edict{nullptr};
	};

	Storage_t m_Storage[MAX_STORAGE];

#pragma pack(push, 1)
	struct Param_t
	{
		bool		modified : 1; // Flag indicating whether the message param has been modified
		ParamType	type     : 3; // The type of the parameter
		size_t		posBack  : 9; // The current position of the parameter in the buffer
		size_t		posFront : 9; // The stock position of the parameter in the buffer
		size_t		oldlen   : 9; // The length of the parameter in the buffer
		size_t		newlen   : 9; // The length of the parameter in the buffer
	};
#pragma pack(pop)

	static const size_t MAX_PARAMS = 16; // The maximum number of parameters allowed in the message
	Param_t m_params[MAX_PARAMS]{};      // The array of parameters in the message
	size_t  m_paramCount : 4;            // The number of parameters in the message

	void resetParam(size_t index);

	void setModifiedDataBit(DataType type);
	void unsetModifiedDataBit(DataType type);
	bool isDataTypeModified(DataType type) const;
	uint8 m_modifiedDataBits : DataType::Max;
};

/**
* Inline methods
*/
inline void MessageImpl::setModifiedDataBit(DataType type)
{
	m_modifiedDataBits |= ((1 << static_cast<int>(DataType::Any)) | (1 << static_cast<int>(type)));
}

inline void MessageImpl::unsetModifiedDataBit(DataType type)
{
	m_modifiedDataBits &= ~((1 << static_cast<int>(DataType::Any)) | (1 << static_cast<int>(type)));
}

inline bool MessageImpl::isDataTypeModified(DataType type) const
{
	return (m_modifiedDataBits & (1 << static_cast<int>(type))) != 0;
}

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
	 * @param msg_id The message type
	 * @return The blocking behavior for the given message type
	 */
	IMessage::BlockType getMessageBlock(int msg_id) const;

	/**
	 * @brief Sets the blocking behavior for the given message type
	 * @param msg_id The message type
	 * @param blockType The blocking behavior to set
	 */
	void setMessageBlock(int msg_id, IMessage::BlockType blockType);

	/**
	 * @brief Registers a hook function for the given message type
	 * @param msg_id The message type to register the hook for
	 * @param handler The hook function to register
	 * @param priority The priority of the hook function (see enum HookChainPriority)
	 */
	void registerHook(int msg_id, hookfunc_t handler, int priority = HC_PRIORITY_DEFAULT);

	/**
	 * @brief Unregisters a hook function for the given message type
	 * @param msg_id The message type to unregister the hook for
	 * @param handler The hook function to unregister
	 */
	void unregisterHook(int msg_id, hookfunc_t handler);

private:
	friend void PF_MessageBegin_Intercept(int msg_dest, int msg_id, const float *pOrigin, edict_t *ed);
	friend void PF_MessageEnd_Intercept();
	friend void PF_WriteByte_Intercept(int iValue);
	friend void PF_WriteChar_Intercept(int iValue);
	friend void PF_WriteShort_Intercept(int iValue);
	friend void PF_WriteLong_Intercept(int iValue);
	friend void PF_WriteAngle_Intercept(float flValue);
	friend void PF_WriteCoord_Intercept(float flValue);
	friend void PF_WriteString_Intercept(const char *sz);
	friend void PF_WriteEntity_Intercept(int iValue);

	bool MessageBegin(int msg_dest, int msg_id, const float *pOrigin, edict_t *ed);
	bool MessageEnd();

private:
	bool WriteParam(IMessage::ParamType type, size_t length = -1);

	bool m_inblock; // Flag indicating whether a message block is currently active
	bool m_inhook;  // Flag indicating whether a message hook is currently active

	/**
	 * @brief The fixed-size memory pool holds a list of free objects
	 * @tparam T The type of objects stored in the stack
	 * @tparam MAX The maximum size of the stack
	 */
	template <typename T, size_t MAX>
	class MessagePool
	{
	public:
		std::unique_ptr<T> acquire()
		{
			if (_size > 0)
				return std::move(_freeObjects[--_size]); // reusing
			return std::make_unique<T>(); // initialize constructor for new element
		}

		void release(std::unique_ptr<T> obj)
		{
			if (_size < MAX)
				_freeObjects[_size++] = std::move(obj);
		}

		void clear()
		{
			while (_size > 0)
				_freeObjects[--_size].reset();
		}

	private:
		size_t _size{0u};
		std::unique_ptr<T> _freeObjects[MAX]{};
	};

	/**
	 * @brief Helper a templated MessageStack class to manage a stack of Message objects with fixed size
	 * @tparam T The type of objects stored in the stack
	 * @tparam MAX The maximum size of the stack
	 */
	template <typename T, size_t MAX>
	class MessageStack
	{
	public:
		MessageStack(MessagePool<T, MAX> &pool) : _pool(pool) {}

		T &push()
		{
			std::unique_ptr<T> msg = _pool.acquire();
			_activeObjects[_size++] = std::move(msg);
			return *_activeObjects[_size - 1];
		}

		void pop()
		{
			if (_size > 0)
				_pool.release(std::move(_activeObjects[--_size]));
		}

		size_t size()     const { return _size; }
		size_t max_size() const { return MAX;   }

		const T &top() const { return *_activeObjects[_size - 1]; }
		      T &top()       { return *_activeObjects[_size - 1]; }

	private:
		size_t _size{0u};
		std::unique_ptr<T> _activeObjects[MAX]{};
		MessagePool<T, MAX> &_pool;
	};

	static const size_t MAX_MSGSTACK = 16; // The maximum size of the message stack, 16 it should be enough
	MessagePool<MessageImpl, MAX_MSGSTACK> m_pool; // A fixed-size memory pool stack for reusable
	MessageStack<MessageImpl, MAX_MSGSTACK> m_stack;

	using HookRegistry_t = IVoidHookChainRegistryImpl<IMessage *>;
	HookRegistry_t *m_hooks[MAX_USERMESSAGES]{};

	IMessage::BlockType m_blocks[MAX_USERMESSAGES]{}; // The array of blocking behaviors for each message type
};

/**
 * @brief The singleton instance of the MessageManager
 */
extern MessageManagerImpl &MessageManager();
