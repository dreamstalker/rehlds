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

#ifndef IPRATELIMIT_H
#define IPRATELIMIT_H
#ifdef _WIN32
#pragma once
#endif

#include "maintypes.h"
#include "net.h"
#include "utlrbtree.h"


/* <e009b> ../engine/ipratelimit.h:5 */
class CIPRateLimit
{
public:
	/* <e014f> ../engine/ipratelimit.h:8 */
	CIPRateLimit() { }

	/* <e0167> ../engine/ipratelimit.h:9 */
	~CIPRateLimit() { }

	/* <e0185> ../engine/ipratelimit.h:12 */
	bool CheckIP(netadr_t adr); /* linkage=_ZN12CIPRateLimit7CheckIPE8netadr_s */

private:

	/* <e00a7> ../engine/ipratelimit.h:15 */
	enum
	{
		MAX_TREE_SIZE = 512,
		START_TREE_SIZE = 256,
		FLUSH_TIMEOUT = 120,
	};

	/* <e00c6> ../engine/ipratelimit.h:18 */
	typedef struct iprate_s
	{
		/* <e00d3> ../engine/ipratelimit.h:17 */
		typedef int ip_t;

		ip_t ip;/*     0     4 */
		long int lastTime;/*     4     4 */
		int count;/*     8     4 */
	} iprate_t;/* size: 12 */

private:

	class CUtlRBTree<CIPRateLimit::iprate_s, int> m_IPTree;/*     0    32 */
	int m_iGlobalCount;/*    32     4 */
	long int m_lLastTime;/*    36     4 */

	/* <e01aa> ../engine/ipratelimit.h:25 */
	bool LessIP(const struct iprate_s  &, const struct iprate_s  &); /* linkage=_ZN12CIPRateLimit6LessIPERKNS_8iprate_sES2_ */
};/* size: 40 */


//extern bool (__fastcall *pCIPRateLimit__CheckIP)(CIPRateLimit *obj, int none, netadr_t adr);

#endif // IPRATELIMIT_H
