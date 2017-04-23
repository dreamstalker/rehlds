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

void MD5_Init(MD5Context_t *ctx)
{
	ctx->bits[0] = ctx->bits[1] = 0;

	// Load magic initialization constants.
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;
}

void MD5_Transform(unsigned int *buf, const unsigned int *in)
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

void MD5_Update(MD5Context_t *ctx, const unsigned char *buf, unsigned int len)
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
			MD5_Transform(ctx->buf, in);
			mdi = 0;
		}
	}
}

void MD5_Final(unsigned char *digest, MD5Context_t *ctx)
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
	MD5_Update(ctx, PADDING, padLen);

	// Append length in bits and transform
	for (i = 0, ii = 0; i < 14; i++, ii += 4)
		in[i] = (((uint32)ctx->in[ii + 3]) << 24) |
				(((uint32)ctx->in[ii + 2]) << 16) |
				(((uint32)ctx->in[ii + 1]) << 8) |
				((uint32)ctx->in[ii]);
	MD5_Transform(ctx->buf, in);

	// Store buffer in digest
	for (i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		digest[ii] = (unsigned char)(ctx->buf[i] & 0xFF);
		digest[ii + 1] = (unsigned char)((ctx->buf[i] >> 8) & 0xFF);
		digest[ii + 2] = (unsigned char)((ctx->buf[i] >> 16) & 0xFF);
		digest[ii + 3] = (unsigned char)((ctx->buf[i] >> 24) & 0xFF);
	}
}

char *MD5_Print(unsigned char *hash)
{
	static char szReturn[64];
	char szChunk[10];
	int i;

	memset(szReturn, 0, sizeof(szReturn));

	for (i = 0; i < 16; i++)
	{
		_snprintf(szChunk, sizeof(szChunk), "%02x", hash[i]);
		strncat(szReturn, szChunk, sizeof(szReturn) - strlen(szReturn) - 1);
	}

	return szReturn;
}

char *MD5_GetCDKeyHash(const char *key)
{
	static char szHashedKeyBuffer[256];

	char szKeyBuffer[256];
	int nKeyLength = strlen(key);
	if (nKeyLength <= 0 || nKeyLength > 255) {
		return nullptr;
	}

	strncpy(szKeyBuffer, key, sizeof(szKeyBuffer) - 1);
	szKeyBuffer[sizeof(szKeyBuffer) - 1] = '\0';
	szKeyBuffer[nKeyLength] = '\0';

	MD5Context_t ctx;
	unsigned char digest[16];

	memset(&ctx, 0, sizeof(ctx));
	memset(digest, 0, sizeof(digest));
	memset(szHashedKeyBuffer, 0, sizeof(szHashedKeyBuffer));

	MD5_Init(&ctx);
	MD5_Update(&ctx, (unsigned char *)szKeyBuffer, nKeyLength);
	MD5_Final(digest, &ctx);

	strncpy(szHashedKeyBuffer, MD5_Print(digest), sizeof(szHashedKeyBuffer) - 1);
	szHashedKeyBuffer[sizeof(szHashedKeyBuffer) - 1] = '\0';

	return szHashedKeyBuffer;
}

void MD5_Hash_Mem(unsigned char *digest, unsigned char *mem, int size)
{
	MD5Context_t ctx;
	memset(&ctx, 0, sizeof(ctx));

	MD5_Init(&ctx);
	MD5_Update(&ctx, mem, size);
	MD5_Final(digest, &ctx);
}
