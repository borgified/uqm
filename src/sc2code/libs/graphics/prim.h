/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _PRIM_H
#define _PRIM_H

enum gfx_object
{
	POINT_PRIM = 0,
	STAMP_PRIM,
	STAMPFILL_PRIM,
	LINE_PRIM,
	TEXT_PRIM,
	RECT_PRIM,
	RECTFILL_PRIM,

	NUM_PRIMS
};
typedef BYTE GRAPHICS_PRIM;

typedef union
{
   POINT Point;
   STAMP Stamp;
   LINE Line;
   TEXT Text;
   RECT Rect;
} PRIM_DESC;
typedef PRIM_DESC *PPRIM_DESC;

typedef DWORD PRIM_LINKS;

typedef struct
{
	PRIM_LINKS Links;
	DWORD TypeAndColor;
	PRIM_DESC Object;
} PRIMITIVE;
typedef PRIMITIVE *PPRIMITIVE;

#define END_OF_LIST ((COUNT)0xFFFF)

#define PRIM_COLOR_SHIFT 8
#define PRIM_COLOR_MASK ((DWORD)~0L << PRIM_COLOR_SHIFT)
#define PRIM_TYPE_MASK (~PRIM_COLOR_MASK)

#define GetPredLink(l) LOWORD(l)
#define GetSuccLink(l) HIWORD(l)
#define MakeLinks MAKE_DWORD
#define SetPrimLinks(pPrim,p,s) ((pPrim)->Links = MakeLinks (p, s))
#define GetPrimLinks(pPrim) ((pPrim)->Links)
#define SetPrimType(pPrim,t) \
		((pPrim)->TypeAndColor = ((pPrim)->TypeAndColor & PRIM_COLOR_MASK) \
		| ((t) & PRIM_TYPE_MASK))
#define GetPrimType(pPrim) \
		((GRAPHICS_PRIM)LOBYTE ((pPrim)->TypeAndColor))
#define SetPrimColor(pPrim,c) \
		((pPrim)->TypeAndColor = ((pPrim)->TypeAndColor & PRIM_TYPE_MASK) \
		| ((c) << PRIM_COLOR_SHIFT))
#define GetPrimColor(pPrim) \
		((COLOR)((pPrim)->TypeAndColor >> PRIM_COLOR_SHIFT))

static inline void
SetPrimNextLink (PPRIMITIVE pPrim, COUNT Link)
{
	SetPrimLinks (pPrim, END_OF_LIST, Link);
}


static inline COUNT
GetPrimNextLink (PPRIMITIVE pPrim)
{
	return GetSuccLink (GetPrimLinks (pPrim));
}


#endif  /* PRIM_H */


