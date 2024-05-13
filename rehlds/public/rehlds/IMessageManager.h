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
		Not,	// Not a block
		Once,	// Block once
		Set		// Set block
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

	virtual ~IMessage() {};

	/**
	 * @brief Returns the number of parameters in the message
	 * @return The number of parameters
	 */
	virtual int				getParamCount() const = 0;

	/**
	 * @brief Returns the type of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The type of the parameter
	 */
	virtual ParamType		getParamType(size_t index) const = 0;

	/**
	 * @brief Returns the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The integer value of the parameter
	 */
	virtual int				getParamInt(size_t index) const = 0;

	/**
	 * @brief Returns the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The float value of the parameter
	 */
	virtual float			getParamFloat(size_t index) const = 0;

	/**
	 * @brief Returns the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @return The string value of the parameter
	 */
	virtual const char*		getParamString(size_t index) const = 0;

	/**
	 * @brief Sets the integer value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The integer value to set
	 */
	virtual void			setParamInt(size_t index, int value) = 0;

	/**
	 * @brief Sets the float value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param value The float value to set
	 */
	virtual void			setParamFloat(size_t index, float value) = 0;

	/**
	 * @brief Sets the vector value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param pos The vector value to set
	 */
	virtual void			setParamVec(size_t index, const float *pos) = 0;

	/**
	 * @brief Sets the string value of the parameter at the given index
	 * @param index The index of the parameter
	 * @param string The string value to set
	 */
	virtual void			setParamString(size_t index, const char *string) = 0;

	/**
	 * @brief Returns the destination of the message
	 * @return The destination of the message
	 */
	virtual Dest			getDest() const = 0;

    /**
     * @brief Returns the type of the message
     * @return The type of the message
     */
	virtual int				getType() const = 0;

    /**
     * @brief Returns the origin of the message
     * @return The origin of the message
     */
	virtual const float*	getOrigin() const = 0;

    /**
     * @brief Returns the edict associated with the message
     * @return The edict associated with the message
     */
	virtual struct edict_s*	getEdict() const = 0;

    /**
     * @brief Returns whether the message has been modified
     * @return True if the message has been modified, false otherwise
     */
	virtual bool			isModified() const = 0;

	// This must be the last virtual function in class
#ifdef REHLDS_SELF
	// Set the copyback buffer for the message
	virtual void setCopybackBuffer(struct sizebuf_s *pbuf) = 0;
#endif
};

#define MESSAGEMNGR_VERSION_MAJOR 1
#define MESSAGEMNGR_VERSION_MINOR 0

/**
 * @brief Interface manages hooks and blocking behavior game messages
 */
class IMessageManager
{
public:
	using hookfunc_t = void (*)(IVoidHookChain<IMessage *> *chain, IMessage *msg);

	virtual ~IMessageManager() {};

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
	 * @param msgType The message type
	 * @return The blocking behavior for the given message type
	 */
	virtual IMessage::BlockType	getMessageBlock(int msgType) const = 0;

	/**
	 * @brief Sets the blocking behavior for the given message type
	 * @param msgType The message type
	 * @param blockType The blocking behavior to set
	 */
	virtual void setMessageBlock(int msgType, IMessage::BlockType blockType) = 0;

	/**
	 * @brief Registers a hook function for the given message type
	 * @param msgType The message type to register the hook for
	 * @param handler The hook function to register
	 * @param priority The priority of the hook function (see enum HookChainPriority)
	 */
	virtual void registerHook(int msgType, hookfunc_t handler, int priority = HC_PRIORITY_DEFAULT) = 0;

	/**
	 * @brief Unregisters a hook function for the given message type
	 * @param msgType The message type to unregister the hook for
	 * @param handler The hook function to unregister
	 */
	virtual void unregisterHook(int msgType, hookfunc_t handler) = 0;
};
