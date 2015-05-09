#include "precompiled.h"
#include "cppunitlite/TestHarness.h"

TEST(BitsWritingReading, MSG, 5000)
{
	// TODO: Move to apropriate Init function
	int size = 4 * 1024 * 1024;
	Memory_Init(new char[size], size);
	SZ_Alloc("net_message", &net_message, 1024);

	sizebuf_t *buf = &net_message;

	uint32 ff1 = ((uint32)1 << 31) - 1;
	uint32 ff2 = ((uint32)1 << 9) - 1;

	SZ_Clear(buf);

	MSG_StartBitWriting(buf);
	MSG_WriteBits(ff1, 31);
	MSG_WriteBits(ff2, 9);
	MSG_EndBitWriting(buf);

	MSG_BeginReading();

	MSG_StartBitReading(buf);
	uint32 t1 = MSG_ReadBits(31);
	uint32 t2 = MSG_ReadBits(9);
	MSG_EndBitReading(buf);

	UINT32_EQUALS("31/9 Read failed (31)", ff1, t1);
	UINT32_EQUALS("31/9 Read failed (9)", ff2, t2);


	SZ_Clear(buf);

	MSG_StartBitWriting(buf);
	MSG_WriteBits(ff2, 9);
	MSG_WriteBits(ff1, 31);
	MSG_EndBitWriting(buf);

	MSG_BeginReading();

	MSG_StartBitReading(buf);
	t2 = MSG_ReadBits(9);
	t1 = MSG_ReadBits(31);
	MSG_EndBitReading(buf);

	UINT32_EQUALS("9/31 Read failed (9)", ff1, t1);
	UINT32_EQUALS("9/31 Read failed (31)", ff2, t2);


	uint32 a2, a1 = 5;
	uint32 b2, b1 = 0xEFEF;
	uint32 c2, c1 = 0x7AAEAEAE;

	SZ_Clear(buf);

	MSG_StartBitWriting(buf);
	MSG_WriteBits(c1, 31);
	MSG_EndBitWriting(buf);

	MSG_BeginReading();

	MSG_StartBitReading(buf);
	c2 = MSG_ReadBits(31);
	MSG_EndBitReading(buf);

	UINT32_EQUALS("31 Read failed", c1, c2);

	SZ_Clear(buf);

	MSG_StartBitWriting(buf);
	MSG_WriteBits(a1, 7);
	MSG_WriteBits(b1, 16);
	MSG_WriteBits(c1, 31);
	MSG_EndBitWriting(buf);

	MSG_BeginReading();

	MSG_StartBitReading(buf);
	a2 = MSG_ReadBits(7);
	b2 = MSG_ReadBits(16);
	c2 = MSG_ReadBits(31);
	MSG_EndBitReading(buf);

	UINT32_EQUALS("7/16/31 Read failed", a1, a2);
	UINT32_EQUALS("7/16/31 Read failed", b1, b2);
	UINT32_EQUALS("7/16/31 Read failed", c1, c2);
}
