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

#pragma once

class BitBuffer {
public:
	BitBuffer();
	BitBuffer(unsigned int size);
	BitBuffer(void *newData, unsigned int size);
	virtual ~BitBuffer();

	unsigned int PeekBits(int numbits);
	int CurrentBit();
	int CurrentSize();
	unsigned char *CurrentByte();

	int GetMaxSize() const { return m_MaxSize; }
	unsigned int GetCurSize() const { return m_CurSize; }
	unsigned char *GetData() const { return m_Data; }
	bool IsOverflowed() const { return m_Overflowed; }

	void SetBuffer(void *buffer, int size);
	void Free();
	void Reset();
	void Clear();
	void FastClear();
	int SpaceLeft();
	void EndBitMode();
	void StartBitMode();
	bool Resize(unsigned int size);
	void ConcatBuffer(BitBuffer *buffer);
	int SkipString();

	void SkipBits(int numbits);
	void SkipBytes(int numbits);

	void AlignByte();

	// function's read stuff
	unsigned int ReadBits(int numbits);
	int ReadBit();
	int ReadChar();
	int ReadByte();
	int ReadShort();
	int ReadWord();

	unsigned int ReadLong();
	float ReadFloat();
	char *ReadString();
	char *ReadStringLine();
	char *ReadBitString();
	int ReadSBits(int numbits);
	float ReadBitAngle(int numbits);
	int ReadBitData(void *dest, int length);
	bool ReadBuf(int iSize, void *pbuf);
	float ReadAngle();
	float ReadHiresAngle();
	float ReadCoord();
	float ReadBitCoord();
	void ReadBitVec3Coord(float *fa);

	// function's write stuff
	void WriteBits(unsigned int data, int numbits);
	void WriteBit(int c);
	void WriteChar(int c);
	void WriteByte(int c);
	void WriteShort(int c);
	void WriteWord(int c);
	void WriteLong(unsigned int c);
	void WriteFloat(float f);
	void WriteString(const char *p);
	void WriteBitString(const char *p);
	void WriteSBits(int data, int numbits);
	void WriteBitAngle(float fAngle, int numbits);
	void WriteBitData(void *src, int length);
	void WriteBuf(const void *buf, int iSize);
	void WriteBuf(BitBuffer *buf, int length);
	void WriteAngle(float f);
	void WriteHiresAngle(float f);
	void WriteCoord(float f);

	bool m_Overflowed;
	unsigned char *m_Data;
	unsigned char *m_CurByte;
	int m_CurSize;
	int m_MaxSize;

protected:
	bool m_LittleEndian;
	bool m_OwnData;
};
