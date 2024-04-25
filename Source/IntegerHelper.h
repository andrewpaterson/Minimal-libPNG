/** ---------------- COPYRIGHT NOTICE, DISCLAIMER, and LICENSE ------------- **

Copyright (c) 2022 Andrew Paterson

This file is part of The Codaphela Project: Codaphela BaseLib

Codaphela BaseLib is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Codaphela BaseLib is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Codaphela BaseLib.  If not, see <http://www.gnu.org/licenses/>.

Microsoft Windows is Copyright Microsoft Corporation

** ------------------------------------------------------------------------ **/
#ifndef __INTEGER_HELPER_H__
#define __INTEGER_HELPER_H__
#include "PrimitiveTypes.h"


uint32 	TruncateLowBits(uint32 i);
bool 	IsPowerOfTwo(uint32 i);
int32 	GetBestHighBit(int32 i);
int32 	GetLogBase2(int32 i);  //0x1 -> 0, 0x2 -> 1, 0x4 -> 2, 0x8 -> 3, 0x10 -> 4, 0x20 -> 5...
int32 	GetByte(uint32 iInt, uint8 iPos);
int32 	SetByte(uint32 c, uint8 iPos);
bool 	GetBit(uint32 iBit, void* pvBitArray);  //return 1 or 0 in char array of bits.
bool 	GetBitReverseHiLo(int32 iBit, void* pvBitArray);  //return 1 or 0 in char array of bits indexed in reverse.
int32	CountBits(void* pvBitArray, int32 iBitLength); 
int32	CountBitsSingly(void* pvBitArray, int32 iBitLength);  //Slow, for testing.
void 	SetBit(uint32 iBit, void* pvBitArray, bool bBit);
void 	SetBits(uint32 iStart, void* pvBitArray, bool bBit, uint32 iLength);
void 	SetFlagInt(uint32* piDest, uint32 iFlag, bool iFlagValue);
void 	SetFlagShort(uint16* psiDest, uint16 iFlag, bool iFlagValue);
void 	SetFlagByte(uint8* psiDest, uint8 iFlag, bool iFlagValue);
int32 	FindFirstClearBit(void* pvArray, uint32 iArraySize);
int32 	FindFirstSetBit(void* pvArray, uint32 iArraySize);
int32 	FindLastClearBit(void* pvArray, uint32 iArraySize);
int32 	FindLastSetBit(void* pvArray, uint32 iArraySize);
int32	FindNextSetBit(void* pvArray, uint32 iArraySize, int32 iStartBit);
int32	FindNextClearBit(void* pvArray, uint32 iArraySize, int32 iStartBit);
bool 	FixBoolInt(uint32 i);
bool	FixBoolPtr(void* pv);
void 	SwapInt(uint32* pi1, uint32* pi2);
void 	SwapByte(uint8* pc1, uint8* pi2);
int32 	FindFirstInt(uint32* piIntArray, uint32 iSearch, uint32 iMaxLength);
int32	FindFirstByte(uint8* pcCharArray, uint8 uiSearch, uint32 iMaxLength);
int32	GetHighNybble(char c);
int32	GetLowNybble(char c);
int32 	CompareInt(const void* arg1, const void* arg2);
int32	CompareIntReverse(const void* arg1, const void* arg2);
char 	GetCrumb(uint32 iCrumb, void* pvArray);  //return 11, 10, 01 or 00.
char 	GetNybble(uint32 iNybble, void* pvArray);
void 	CopyBits(void* pvDest, int32 iDestOffset, void* pvSource, int32 iSourceOffset, int32 iNumBits, int32 bClear);  //Setting clear will make all unused bits zero.
void 	CopyBitsReverseHiLo(void* pvDest, int32 iDestOffset, void* pvSource, int32 iSourceOffset, int32 iNumBits, int32 bClear);  //Setting clear will make all unused bits zero.
int16	ReverseShortEndianness(int16 s);
int32	ReverseIntEndianness(int32 i);
void	ReverseEndianness(void* pv, int32 iSize);
void	ReverseBytes(void* pv, int32 iSize);
int32 	CalculateStride(int32 iElementSize, int32 iAlignment);
int32 	CalculateOffset(char iOffset, int32 iAlignment);
int32	IntAbs(int32 i);
int32	GetPowerOf2SizeDown(int32 iInt);
int32	GetPowerOf2SizeUp(int32 iInt);
int32	GetPowerOf2SizeAuto(int32 iInt);
int32	LargestInt(int32* pInts, int32 iCount);


#endif // !__INTEGER_HELPER_H__

