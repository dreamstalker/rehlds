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

// header
#define Q1BSP_VERSION		29		// quake1 regular version (beta is 28)
#define HLBSP_VERSION		30		// half-life regular version

#define MAX_MAP_HULLS		4

#define CONTENTS_ORIGIN		-7		// removed at csg time
#define CONTENTS_CLIP		-8		// changed to contents_solid
#define CONTENTS_CURRENT_0	-9
#define CONTENTS_CURRENT_90	-10
#define CONTENTS_CURRENT_180	-11
#define CONTENTS_CURRENT_270	-12
#define CONTENTS_CURRENT_UP	-13
#define CONTENTS_CURRENT_DOWN	-14

#define CONTENTS_TRANSLUCENT	-15

#define	LUMP_ENTITIES		0
#define	LUMP_PLANES		1
#define	LUMP_TEXTURES		2
#define	LUMP_VERTEXES		3
#define	LUMP_VISIBILITY		4
#define	LUMP_NODES		5
#define	LUMP_TEXINFO		6
#define	LUMP_FACES		7
#define	LUMP_LIGHTING		8
#define	LUMP_CLIPNODES		9
#define	LUMP_LEAFS		10
#define	LUMP_MARKSURFACES	11
#define	LUMP_EDGES		12
#define	LUMP_SURFEDGES		13
#define	LUMP_MODELS		14

#define	HEADER_LUMPS		15

typedef struct lump_s
{
	int				fileofs;
	int				filelen;
} lump_t;

typedef struct dmodel_s
{
	float			mins[3], maxs[3];
	float			origin[3];
	int				headnode[MAX_MAP_HULLS];
	int				visleafs;		// not including the solid leaf 0
	int				firstface, numfaces;
} dmodel_t;

typedef struct dheader_s
{
	int				version;
	lump_t			lumps[15];
} dheader_t;

typedef struct dmiptexlump_s
{
	int				_nummiptex;
	int				dataofs[4];
} dmiptexlump_t;

typedef struct miptex_s
{
	char			name[16];
	unsigned		width;
	unsigned		height;
	unsigned		offsets[4];
} miptex_t;

typedef struct dvertex_s
{
	float			point[3];
} dvertex_t;

typedef struct dplane_s
{
	float			normal[3];
	float			dist;
	int				type;
} dplane_t;

typedef struct dnode_s
{
	int				planenum;
	short			children[2];
	short			mins[3];
	short			maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;
} dnode_t;

typedef struct dclipnode_s
{
	int				planenum;
	short			children[2];	// negative numbers are contents
} dclipnode_t;

typedef struct texinfo_s
{
	float			vecs[2][4];
	int				_miptex;
	int				flags;
} texinfo_t;

typedef struct dedge_s
{
	unsigned short	v[2];
} dedge_t;

typedef struct dface_s
{
	short			planenum;
	short			side;
	int				firstedge;
	short			numedges;
	short			texinfo;
	byte			styles[4];
	int				lightofs;
} dface_t;

typedef struct dleaf_s
{
	int				contents;
	int				visofs;
	short			mins[3];
	short			maxs[3];
	unsigned short	firstmarksurface;
	unsigned short	nummarksurfaces;
	byte			ambient_level[4];
} dleaf_t;
