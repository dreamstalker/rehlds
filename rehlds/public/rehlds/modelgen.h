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

#ifndef MODELGEN_H
#define MODELGEN_H
#ifdef _WIN32
#pragma once
#endif

typedef enum synctype_e
{
	ST_SYNC = 0,
	ST_RAND = 1,
} synctype_t;

typedef enum aliasframetype_s
{
	ALIAS_SINGLE = 0,
	ALIAS_GROUP = 1,
} aliasframetype_t;

typedef enum aliasskintype_s
{
	ALIAS_SKIN_SINGLE = 0,
	ALIAS_SKIN_GROUP = 1,
} aliasskintype_t;

typedef struct mdl_s
{
	int				ident;
	int				version;
	vec3_t			scale;
	vec3_t			scale_origin;
	float			boundingradius;
	vec3_t			eyeposition;
	int				numskins;
	int				skinwidth;
	int				skinheight;
	int				numverts;
	int				numtris;
	int				numframes;
	synctype_t		synctype;
	int				flags;
	float			size;
} mdl_t;

typedef struct stvert_s
{
	int				onseam;
	int				s;
	int				t;
} stvert_t;

typedef struct dtriangle_s
{
	int				facesfront;
	int				vertindex[3];
} dtriangle_t;

typedef struct trivertx_s
{
	byte			v[3];
	byte			lightnormalindex;
} trivertx_t;

typedef struct daliasframe_s
{
	trivertx_t		bboxmin, bboxmax;
	char			name[16];
} daliasframe_t;

typedef struct daliasgroup_s
{
	int				numframes;
	trivertx_t		bboxmin, bboxmax;
} daliasgroup_t;

typedef struct daliasskingroup_s
{
	int				numskins;
} daliasskingroup_t;

typedef struct daliasinterval_s
{
	float			interval;
} daliasinterval_t;

typedef struct daliasskininterval_s
{
	float			interval;
} daliasskininterval_t;

typedef struct daliasframetype_s
{
	aliasframetype_t type;
} daliasframetype_t;

typedef struct daliasskintype_s
{
	aliasskintype_t type;
} daliasskintype_t;

#endif // MODELGEN_H
