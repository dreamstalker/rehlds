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

//bool (__fastcall *pCIPRateLimit__CheckIP)(CIPRateLimit *obj, int none, netadr_t adr);

NOBODY bool CIPRateLimit::CheckIP(netadr_t adr)
{
	// TODO: Reverse me
	//{
	//	long curTime;                                     //    29
	//	ip_t clientIP;                                        //    32
	//	class iprate_s findEntry;                             //    56
	//	ip_t entry;                                           //    57
	//	{
	//		ip_t tmp;                                     //    37
	//		int i;                                        //    38
	//		LastInorder(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this); /* size=169106148, low_pc=0 */ //    37
	//		IsValidIndex(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this,
	//					 int i); /* size=524288, low_pc=0 */ //    41
	//		operator[](class CUtlRBTree<CIPRateLimit::iprate_s, int> *const this,
	//				   int i); /* size=169107280, low_pc=0 */ //    42
	//		{
	//			ip_t removeIPT;                       //    45
	//			PrevInorder(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this,
	//						int i); /* size=0, low_pc=0 */ //    46
	//		}
	//		PrevInorder(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this,
	//					int i); /* size=2019913216, low_pc=0 */ //    51
	//	}
	//	Find(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this,
	//		 const class iprate_s  &const search); /* size=1967350639, low_pc=0 */ //    57
	//	IsValidIndex(const class CUtlRBTree<CIPRateLimit::iprate_s, int>  *const this,
	//				 int i); /* size=7626612, low_pc=0 */ //    59
	//	operator[](class CUtlRBTree<CIPRateLimit::iprate_s, int> *const this,
	//			   int i); /* size=1936875856, low_pc=0 */ //    61
	//	operator[](class CUtlRBTree<CIPRateLimit::iprate_s, int> *const this,
	//			   int i); /* size=1869898597, low_pc=0 */ //    63
	//	{
	//		float query_rate;                             //    70
	//	}
	//	{
	//		class iprate_s newEntry;                      //    80
	//		Insert(class CUtlRBTree<CIPRateLimit::iprate_s, int> *const this,
	//			   const class iprate_s  &const insert); /* size=1399744112, low_pc=0 */ //    84
	//	}
	//	{
	//		float query_rate;                             //    97
	//	}
	//}
	return true;
}
