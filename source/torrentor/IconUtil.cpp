//------------------------------------------------------------------------------
//	Copyright (c) 2012, Guido Pola.
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the "Software"),
//	to deal in the Software without restriction, including without limitation
//	the rights to use, copy, modify, merge, publish, distribute, sublicense,
//	and/or sell copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//	DEALINGS IN THE SOFTWARE.
//
//	File Name:		IconUtil.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	Provides icon cache support.
//	TODO:			- Load default icon for unknow mimes.
//					- Cleanup, move to a class [IconCache]?.
//------------------------------------------------------------------------------
#include "Torrentor.h"
#include "IconUtil.h"

#include <interface/Bitmap.h>

#include <string.h>
#define stricmp strcasecmp




struct IconCache_t
{
	BBitmap*	fIcon;
	char		fIconMime[B_MIME_TYPE_LENGTH];
};

//
// @TODO: Cleanup ObjectList.
//
BObjectList<IconCache_t> IconObjectList;


//
//
//
const BBitmap* LoadIconFromMime(const char* MimeType)
{
	BMimeType mime(MimeType);
	
	//
	//
	//
	IconCache_t* IconCache = new IconCache_t;
	
	//
	//
	//
	IconCache->fIcon = new BBitmap(BRect(0, 0, 31, 31), 0, B_RGBA32);
	
	
	mime.GetIcon(IconCache->fIcon, B_LARGE_ICON);
	
	strncpy(IconCache->fIconMime, MimeType, B_MIME_TYPE_LENGTH);
	
	//
	//
	//
	IconObjectList.AddItem(IconCache);
	
	return IconCache->fIcon;
}

//
//
//
const BBitmap* FindIconFromMime(const char* MimeType)
{
	//
	//
	//
	for( int32 i = 0; i < IconObjectList.CountItems(); ++i ) 
	{
		const IconCache_t* IconCache = IconObjectList.ItemAt(i);
		
		//
		//
		//
		if( IconCache == NULL )
			continue;
			
		//
		//
		//
		if( stricmp(IconCache->fIconMime, MimeType) == 0 )
			return IconCache->fIcon;
	}
	
	return NULL;
}

//
//
//
const BBitmap* GetIconBitmap(const char* MimeType)
{
	const BBitmap* Icon = FindIconFromMime(MimeType);
	
	//
	//
	//
	if( Icon == NULL )
	{
		return LoadIconFromMime(MimeType);
	}
	return Icon;
}

