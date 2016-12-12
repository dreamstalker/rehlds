/*
Copyright (C) 2010 by Ronnie Sahlberg <ronniesahlberg@gmail.com>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "archtypes.h"

extern uint32 crc32c_t8_nosse(uint32 iCRC, uint8 u8);
extern uint32 crc32c_t8_sse(uint32 iCRC, uint8 u8);
extern uint32 crc32c_t_nosse(uint32 iCRC, const uint8 *buf, int len);
extern uint32 crc32c_t_sse(uint32 iCRC, const uint8 *buf, unsigned int len);
extern uint32 crc32c_t(uint32 iCRC, const uint8 *buf, unsigned int len);
extern uint32 crc32c(const uint8 *buf, int len);
