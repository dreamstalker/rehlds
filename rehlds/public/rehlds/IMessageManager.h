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

/**
 * @brief Interface for defining message parameters and behavior for a individual message object
 */
class IMessage
{
public:
	/**
	 * @brief The parameter types for a message
	 */
	enum class ParamType : uint8
	{
		Byte,
		Char,
		Short,
		Long,
		Angle,
		Coord,
		String,
		Entity,
	};

	/**
	 * @brief Blocking behavior types for messages
	 */
	enum class BlockType : uint8
	{
		Not,    // Not a block
		Once,   // Block once
		Set     // Set block
	};

	/**
	 * @brief Message destinations
	 */
	enum class Dest : uint8
	{
		BROADCAST,      // Unreliable to all
		ONE,            // Reliable to one (msg_entity)
		ALL,            // Reliable to all
		INIT,           // Write to the init string
		PVS,            // Ents in PVS of org
		PAS,            // Ents in PAS of org
		PVS_R,          // Reliable to PVS
		PAS_R,          // Reliable to PAS
		ONE_UNRELIABLE, // Send to one client, but don't put in reliable stream, put in unreliable datagram
		SPEC,           // Sends to all spectator proxies
	};

	/**
	 * @brief Data types for message data
	 */
	enum class DataType : uint8_t
	{
		Any,    // Any part of the message
		Dest,   // Destination of the message
		Index,  // Index of the message
		Origin, // Origin of the message
		Edict,  // Pointer to the edict of the recipient client
		Param,  // Parameter of the message
		Max
	};

	virtual ~IMessage() = default;

	/**
	 * @brief Returns the number of parameters in the message
	 * @return The number of parameters
	 */
	virtual int getParamCount() const = 0;

	/**
	 * @brief Returns the type of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The type of the parameter
	 */
	virtual ParamType getParamType(size_t index) const = 0;

	/**
	 * @brief Returns the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The integer value of the parameter
	 */
	virtual int getParamInt(size_t index) const = 0;

	/**
	 * @brief Returns the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The float value of the parameter
	 */
	virtual float getParamFloat(size_t index) const = 0;

	/**
	 * @brief Returns the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The string value of the parameter
	 */
	virtual const char* getParamString(size_t index) const = 0;

	/**
	 * @brief Sets the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The integer value to set
	 */
	virtual void setParamInt(size_t index, int value) = 0;

	/**
	 * @brief Sets the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The float value to set
	 */
	virtual void setParamFloat(size_t index, float value) = 0;

	/**
	 * @brief Sets the vector value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param pos The vector value to set
	 */
	virtual void setParamVec(size_t index, const float *pos) = 0;

	/**
	 * @brief Sets the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param string The string value to set
	 */
	virtual void setParamString(size_t index, const char *string) = 0;

	/**
	 * @brief Returns the destination of the message
	 * @return The destination of the message
	 */
	virtual Dest getDest() const = 0;

	/**
	 * @brief Returns the index of the message
	 * @return The index of the message
	 */
	virtual int getId() const = 0;

	/**
	 * @brief Returns the origin of the message
	 * @return The origin of the message
	 */
	virtual const float* getOrigin() const = 0;

	/**
	 * @brief Returns the edict associated with the message
	 * @return The edict associated with the message
	 */
	virtual struct edict_s*	getEdict() const = 0;

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
	virtual bool isDataModified(DataType type = DataType::Any, size_t index = -1) const = 0;

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
	virtual bool resetModifiedData(DataType type = DataType::Any, size_t index = -1) = 0;

	/**
	 * @brief Sets the destination of the message
	 */
	virtual void setDest(Dest dest) = 0;

	/**
	 * @brief Sets the index of the message
	 */
	virtual void setId(int msg_id) = 0;

	/**
	 * @brief Sets the origin of the message
	 */
	virtual void setOrigin(const float *origin) = 0;

	/**
	 * @brief Sets the edict associated with the message
	 */
	virtual void setEdict(struct edict_s *pEdict) = 0;

	/**
	 * @brief Returns the original destination of the message before any modifications
	 * @return The original destination of the message
	 */
	virtual Dest getOriginalDest() const = 0;

	/**
	 * @brief Returns the original type of the message before any modifications
	 * @return The original type of the message
	 */
	virtual int getOriginalId() const = 0;

	/**
	 * @brief Returns the original origin of the message before any modifications
	 * @return The original origin of the message
	 */
	virtual const float* getOriginalOrigin() const = 0;

	/**
	 * @brief Returns the original edict associated with the message before any modifications
	 * @return The original edict associated with the message
	 */
	virtual struct edict_s*	getOriginalEdict() const = 0;

	/**
	 * @brief Returns the original integer value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original integer value of the parameter
	 */
	virtual int getOriginalParamInt(size_t index) const = 0;

	/**
	 * @brief Returns the original float value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original float value of the parameter
	 */
	virtual float getOriginalParamFloat(size_t index) const = 0;

	/**
	 * @brief Returns the original string value of the parameter at the given index before any modifications
	 * @param index The index of the parameter
	 * @return The original string value of the parameter
	 */
	virtual const char* getOriginalParamString(size_t index) const = 0;

	// This must be the last virtual function in class
#ifdef REHLDS_SELF
	// Set the copyback buffer for the message
	virtual void setCopybackBuffer(struct sizebuf_s *pbuf) = 0;
#endif
};

#define MESSAGEMNGR_VERSION_MAJOR 2
#define MESSAGEMNGR_VERSION_MINOR 0

/**
 * @brief Interface manages hooks and blocking behavior game messages
 */
class IMessageManager
{
public:
	using hookfunc_t = void (*)(IVoidHookChain<IMessage *> *chain, IMessage *msg);

	virtual ~IMessageManager() = default;

	/**
	 * @brief Returns the major version of the MessageManager
	 * @return The major version
	 */
	virtual int getMajorVersion() const = 0;

	/**
	 * @brief Returns the minor version of the MessageManager
	 * @return The minor version
	 */
	virtual int getMinorVersion() const = 0;

	/**
	 * @brief Returns the blocking behavior for the given message type
	 * @param msg_id The message type
	 * @return The blocking behavior for the given message type
	 */
	virtual IMessage::BlockType	getMessageBlock(int msg_id) const = 0;

	/**
	 * @brief Sets the blocking behavior for the given message type
	 * @param msg_id The message type
	 * @param blockType The blocking behavior to set
	 */
	virtual void setMessageBlock(int msg_id, IMessage::BlockType blockType) = 0;

	/**
	 * @brief Registers a hook function for the given message type
	 * @param msg_id The message type to register the hook for
	 * @param handler The hook function to register
	 * @param priority The priority of the hook function (see enum HookChainPriority)
	 */
	virtual void registerHook(int msg_id, hookfunc_t handler, int priority = HC_PRIORITY_DEFAULT) = 0;

	/**
	 * @brief Unregisters a hook function for the given message type
	 * @param msg_id The message type to unregister the hook for
	 * @param handler The hook function to unregister
	 */
	virtual void unregisterHook(int msg_id, hookfunc_t handler) = 0;
};
