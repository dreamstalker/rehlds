/*
 * $Id: crc32.h,v 1.1.1.1 1996/02/18 21:38:11 ylo Exp $
 * $Log: crc32.h,v $
 * Revision 1.1.1.1  1996/02/18 21:38:11  ylo
 * 	Imported ssh-1.2.13.
 *
 * Revision 1.2  1995/07/13  01:21:45  ylo
 * 	Removed "Last modified" header.
 * 	Added cvs log.
 *
 * $Endlog$
 */

#pragma once
#include "archtypes.h"
/* This computes a 32 bit CRC of the data in the buffer, and returns the
   CRC.  The polynomial used is 0xedb88320. */

uint32 crc32(const uint8 *buf, unsigned int len);
uint32 crc32_t(uint32 iCRC, const uint8 *s, unsigned int len);

