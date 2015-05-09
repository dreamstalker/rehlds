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
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"


static const uint32 pulCRCTable[256] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


/* <193b9> ../engine/crc.c:86 */
void CRC32_Init(CRC32_t *pulCRC)
{
	*pulCRC = -1;
}

/* <193d9> ../engine/crc.c:91 */
CRC32_t CRC32_Final(CRC32_t pulCRC)
{
	return ~pulCRC;
}

/* <19453> ../engine/crc.c:96 */
void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
	CRC32_t ulCrc = *pulCRC;
	ulCrc = pulCRCTable[(ulCrc & 0xFF) ^ ch] ^ ((ulCrc ^ ch) >> 8);
	*pulCRC = ulCrc;
}

/* <19496> ../engine/crc.c:106 */
void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *pBuffer, int nBuffer)
{
	CRC32_t ulCrc = *pulCRC;
	uint8* pb = (uint8*)pBuffer;

	for (int i = 0; i < nBuffer; i++)
	{
		ulCrc = pulCRCTable[(ulCrc & 0xFF) ^ pb[i]] ^ (ulCrc >> 8);
	}

	*pulCRC = ulCrc;
}

/* <19527> ../engine/crc.c:247 */
byte COM_BlockSequenceCRCByte(byte *base, int length, int sequence)
{
	byte *p;
	char chkb[64];
	CRC32_t crc;

	if (sequence < 0)
		Sys_Error("sequence < 0, in COM_BlockSequenceCRCByte\n");
	p = (byte *)pulCRCTable + sequence % 0x3FC;
	if (length > 60)
		length = 60;

	Q_memcpy(chkb, base, length);

	chkb[length] = p[0];
	chkb[length + 1] = p[1];
	chkb[length + 2] = p[2];
	chkb[length + 3] = p[3];

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, chkb, length + 4);
	return CRC32_Final(crc);
}

/* <195e1> ../engine/crc.c:292 */
NOXREF BOOL CRC_File(CRC32_t *crcvalue, char *pszFileName)
{
	FileHandle_t fp;
	byte chunk[1024];
	int nBytesRead;
	int nSize;

	fp = FS_Open(pszFileName, "rb");

	if (!fp)
		return 0;

	nSize = FS_Size(fp);
	if (nSize == -1)
		return 0;

	while (nSize > 0)
	{
		if (nSize > 1024)
			nBytesRead = FS_Read(chunk, 1024, 1, fp);
		else
			nBytesRead = FS_Read(chunk, nSize, 1, fp);

		if (nBytesRead > 0)
		{
			nSize -= nBytesRead;
			CRC32_ProcessBuffer(crcvalue, chunk, nBytesRead);
		}

		if (FS_EndOfFile(fp))
			break;

		if (!FS_IsOk(fp))
		{
			FS_Close(fp);
			return 0;
		}
	}
	FS_Close(fp);
	return 1;
}

/* <1966e> ../engine/crc.c:354 */
int CRC_MapFile(CRC32_t *crcvalue, char *pszFileName)
{
	FileHandle_t fp;
	byte chunk[1024];
	int i;
	int l;
	int nBytesRead;
	dheader_t header;
	int nSize;
	lump_t *curLump;
	int32 startOfs;
	int nLumpID; 

	nLumpID = 0;
	if (Q_stricmp(com_gamedir, "bshift") == 0)
		nLumpID = 1;

	fp = FS_Open(pszFileName, "rb");

	if (!fp)
		return 0;

	nSize = FS_Size(fp);
	if (nSize == -1)
		return 0;

	startOfs = FS_Tell(fp);
	if (FS_Read(&header, sizeof(dheader_t), 1, fp) != sizeof(dheader_t))
	{
		Con_Printf("Could not read BSP header for map [%s].\n", pszFileName);
		FS_Close(fp);
		return 0;
	}
	i = LittleLong(header.version);
	if (i != BSPVERSION)
	{
		Con_Printf("Map [%s] has incorrect BSP version (%i should be %i).\n", pszFileName, i, BSPVERSION);
		FS_Close(fp);
		return 0;
	}
	for (l = 0; l < HEADER_LUMPS; l++)
	{
		if (l == nLumpID)
			continue;

		curLump = &(header.lumps[l]);
		nSize = curLump->filelen;
		FS_Seek(fp, startOfs + curLump->fileofs, FILESYSTEM_SEEK_HEAD);

		while(nSize > 0)
		{
			if (nSize > 1024)
				nBytesRead = FS_Read(chunk, 1024, 1, fp);
			else
				nBytesRead = FS_Read(chunk, nSize, 1, fp);

			if (nBytesRead > 0)
			{
				nSize -= nBytesRead;
				CRC32_ProcessBuffer(crcvalue, chunk, nBytesRead);
			}
			if (!FS_IsOk(fp))
			{
				FS_Close(fp);
				return 0;
			}
		}
	}
	FS_Close(fp);
	return 1;
}


static unsigned char PADDING[64] =
{
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) \
	{ \
		(a) += F ((b), (c), (d)) + (x) + (uint32)(ac); \
		(a) = ROTATE_LEFT ((a), (s)); \
		(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) \
	{ \
		(a) += G ((b), (c), (d)) + (x) + (uint32)(ac); \
		(a) = ROTATE_LEFT ((a), (s)); \
		(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) \
	{ \
		(a) += H ((b), (c), (d)) + (x) + (uint32)(ac); \
		(a) = ROTATE_LEFT ((a), (s)); \
		(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) \
	{ \
		(a) += I ((b), (c), (d)) + (x) + (uint32)(ac); \
		(a) = ROTATE_LEFT ((a), (s)); \
		(a) += (b); \
	}


/* <1974c> ../engine/crc.c:455 */
void MD5Init(MD5Context_t *ctx)
{
	ctx->bits[0] = ctx->bits[1] = 0;

	// Load magic initialization constants.
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;
}

/* <19841> ../engine/crc.c:473 */
void MD5Update(MD5Context_t *ctx, const unsigned char *buf, unsigned int len)
{
	uint32 in[16];
	int mdi;
	unsigned int i, ii;

	// Compute number of bytes mod 64
	mdi = (int)((ctx->bits[0] >> 3) & 0x3F);

	// Update number of bits
	if ((ctx->bits[0] + ((uint32)len << 3)) < ctx->bits[0])
	{
		ctx->bits[1]++;
	}

	ctx->bits[0] += ((uint32)len << 3);
	ctx->bits[1] += ((uint32)len >> 29);

	while (len--)
	{
		// Add new character to buffer, increment mdi
		ctx->in[mdi++] = *buf++;

		// Transform if necessary
		if (mdi == 0x40)
		{
			for (i = 0, ii = 0; i < 16; i++, ii += 4)
				in[i] = (((uint32)ctx->in[ii + 3]) << 24) |
						(((uint32)ctx->in[ii + 2]) << 16) |
						(((uint32)ctx->in[ii + 1]) << 8) |
						((uint32)ctx->in[ii]);
			MD5Transform(ctx->buf, in);
			mdi = 0;
		}
	}
}

/* <197ea> ../engine/crc.c:528 */
void MD5Final(unsigned char digest[16], MD5Context_t *ctx)
{
	uint32 in[16];
	int mdi;
	unsigned int i, ii;
	unsigned int padLen;

	// save number of bits
	in[14] = ctx->bits[0];
	in[15] = ctx->bits[1];

	// Compute number of bytes mod 64
	mdi = (int)((ctx->bits[0] >> 3) & 0x3F);

	// Pad out to 56 mod 64
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
	MD5Update(ctx, PADDING, padLen);

	// Append length in bits and transform
	for (i = 0, ii = 0; i < 14; i++, ii += 4)
		in[i] = (((uint32)ctx->in[ii + 3]) << 24) |
				(((uint32)ctx->in[ii + 2]) << 16) |
				(((uint32)ctx->in[ii + 1]) << 8) |
				((uint32)ctx->in[ii]);
	MD5Transform(ctx->buf, in);

	// Store buffer in digest
	for (i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		digest[ii] = (unsigned char)(ctx->buf[i] & 0xFF);
		digest[ii + 1] = (unsigned char)((ctx->buf[i] >> 8) & 0xFF);
		digest[ii + 2] = (unsigned char)((ctx->buf[i] >> 16) & 0xFF);
		digest[ii + 3] = (unsigned char)((ctx->buf[i] >> 24) & 0xFF);
	}
}

/* <19769> ../engine/crc.c:592 */
void MD5Transform(unsigned int buf[4], const unsigned int in[16])
{
	uint32 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

	// Round 1
#define S11 7
#define S12 12
#define S13 17
#define S14 22
	FF(a, b, c, d, in[0], S11, 3614090360); /* 1 */
	FF(d, a, b, c, in[1], S12, 3905402710); /* 2 */
	FF(c, d, a, b, in[2], S13, 606105819); /* 3 */
	FF(b, c, d, a, in[3], S14, 3250441966); /* 4 */
	FF(a, b, c, d, in[4], S11, 4118548399); /* 5 */
	FF(d, a, b, c, in[5], S12, 1200080426); /* 6 */
	FF(c, d, a, b, in[6], S13, 2821735955); /* 7 */
	FF(b, c, d, a, in[7], S14, 4249261313); /* 8 */
	FF(a, b, c, d, in[8], S11, 1770035416); /* 9 */
	FF(d, a, b, c, in[9], S12, 2336552879); /* 10 */
	FF(c, d, a, b, in[10], S13, 4294925233); /* 11 */
	FF(b, c, d, a, in[11], S14, 2304563134); /* 12 */
	FF(a, b, c, d, in[12], S11, 1804603682); /* 13 */
	FF(d, a, b, c, in[13], S12, 4254626195); /* 14 */
	FF(c, d, a, b, in[14], S13, 2792965006); /* 15 */
	FF(b, c, d, a, in[15], S14, 1236535329); /* 16 */

	// Round 2
#define S21 5
#define S22 9
#define S23 14
#define S24 20
	GG(a, b, c, d, in[1], S21, 4129170786); /* 17 */
	GG(d, a, b, c, in[6], S22, 3225465664); /* 18 */
	GG(c, d, a, b, in[11], S23, 643717713); /* 19 */
	GG(b, c, d, a, in[0], S24, 3921069994); /* 20 */
	GG(a, b, c, d, in[5], S21, 3593408605); /* 21 */
	GG(d, a, b, c, in[10], S22, 38016083); /* 22 */
	GG(c, d, a, b, in[15], S23, 3634488961); /* 23 */
	GG(b, c, d, a, in[4], S24, 3889429448); /* 24 */
	GG(a, b, c, d, in[9], S21, 568446438); /* 25 */
	GG(d, a, b, c, in[14], S22, 3275163606); /* 26 */
	GG(c, d, a, b, in[3], S23, 4107603335); /* 27 */
	GG(b, c, d, a, in[8], S24, 1163531501); /* 28 */
	GG(a, b, c, d, in[13], S21, 2850285829); /* 29 */
	GG(d, a, b, c, in[2], S22, 4243563512); /* 30 */
	GG(c, d, a, b, in[7], S23, 1735328473); /* 31 */
	GG(b, c, d, a, in[12], S24, 2368359562); /* 32 */

	// Round 3
#define S31 4
#define S32 11
#define S33 16
#define S34 23
	HH(a, b, c, d, in[5], S31, 4294588738); /* 33 */
	HH(d, a, b, c, in[8], S32, 2272392833); /* 34 */
	HH(c, d, a, b, in[11], S33, 1839030562); /* 35 */
	HH(b, c, d, a, in[14], S34, 4259657740); /* 36 */
	HH(a, b, c, d, in[1], S31, 2763975236); /* 37 */
	HH(d, a, b, c, in[4], S32, 1272893353); /* 38 */
	HH(c, d, a, b, in[7], S33, 4139469664); /* 39 */
	HH(b, c, d, a, in[10], S34, 3200236656); /* 40 */
	HH(a, b, c, d, in[13], S31, 681279174); /* 41 */
	HH(d, a, b, c, in[0], S32, 3936430074); /* 42 */
	HH(c, d, a, b, in[3], S33, 3572445317); /* 43 */
	HH(b, c, d, a, in[6], S34, 76029189); /* 44 */
	HH(a, b, c, d, in[9], S31, 3654602809); /* 45 */
	HH(d, a, b, c, in[12], S32, 3873151461); /* 46 */
	HH(c, d, a, b, in[15], S33, 530742520); /* 47 */
	HH(b, c, d, a, in[2], S34, 3299628645); /* 48 */

	// Round 4
#define S41 6
#define S42 10
#define S43 15
#define S44 21
	II(a, b, c, d, in[0], S41, 4096336452); /* 49 */
	II(d, a, b, c, in[7], S42, 1126891415); /* 50 */
	II(c, d, a, b, in[14], S43, 2878612391); /* 51 */
	II(b, c, d, a, in[5], S44, 4237533241); /* 52 */
	II(a, b, c, d, in[12], S41, 1700485571); /* 53 */
	II(d, a, b, c, in[3], S42, 2399980690); /* 54 */
	II(c, d, a, b, in[10], S43, 4293915773); /* 55 */
	II(b, c, d, a, in[1], S44, 2240044497); /* 56 */
	II(a, b, c, d, in[8], S41, 1873313359); /* 57 */
	II(d, a, b, c, in[15], S42, 4264355552); /* 58 */
	II(c, d, a, b, in[6], S43, 2734768916); /* 59 */
	II(b, c, d, a, in[13], S44, 1309151649); /* 60 */
	II(a, b, c, d, in[4], S41, 4149444226); /* 61 */
	II(d, a, b, c, in[11], S42, 3174756917); /* 62 */
	II(c, d, a, b, in[2], S43, 718787259); /* 63 */
	II(b, c, d, a, in[9], S44, 3951481745); /* 64 */

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

/* <198b8> ../engine/crc.c:686 */
BOOL MD5_Hash_File(unsigned char digest[16], char *pszFileName, BOOL bUsefopen, BOOL bSeed, unsigned int seed[4])
{
	FileHandle_t fp = (FileHandle_t)FS_OpenPathID(pszFileName, "rb", "GAMECONFIG");
	if (!fp)
		fp = FS_Open(pszFileName, "rb");
	
	if (!fp)
		return 0;

	int nSize = FS_Size(fp);
	if (nSize <= 0)
	{
		FS_Close(fp);
		return 0;

	}
	
	MD5Context_t ctx;
	Q_memset(&ctx, 0, sizeof(ctx));
	MD5Init(&ctx);
	if (bSeed)
		MD5Update(&ctx, (unsigned char*)seed, 16);

	while (1)
	{
		byte chunk[1024];
		int nBytesRead = FS_Read(chunk, (nSize <= sizeof(chunk)) ? nSize : sizeof(chunk), 1, fp);
		if (nBytesRead > 0)
		{
			nSize -= nBytesRead;
			MD5Update(&ctx, chunk, nBytesRead);
		}

		if (FS_EndOfFile(fp))
		{
			FS_Close(fp);
			MD5Final(digest, &ctx);
			return 1;
		}

		if (!FS_IsOk(fp))
			break;

		if (nSize <= 0)
		{
			FS_Close(fp);
			MD5Final(digest, &ctx);
			return 1;
		}
	}

	FS_Close(fp);
	return 0;
}

/* <1998b> ../engine/crc.c:762 */
char *MD5_Print(unsigned char hash[16])
{
	static char szReturn[64];
	char szChunk[10];
	int i;

	Q_memset(szReturn, 0, sizeof(szReturn));

	for (i = 0; i < 16; i++)
	{
		Q_snprintf(szChunk, sizeof(szChunk), "%02x", hash[i]);
		Q_strncat(szReturn, szChunk, sizeof(szReturn) - Q_strlen(szReturn) - 1);
	}

	return szReturn;
}
