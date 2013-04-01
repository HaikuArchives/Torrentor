//------------------------------------------------------------------------------
//	Copyright (c) 2012-2013, Guido Pola.
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
//					- Replace the default icons bitmap objects 
//					  [fDefaultIconLargeBitmap, fDefaultIconMiniBitmap] with
//					  IconCache_t object.
//------------------------------------------------------------------------------
#include <cstring>

#include <Bitmap.h>
#include <Mime.h>
#include <StringList.h>

#include "Torrentor.h"
#include "IconUtil.h"

const char* B_DIRECTORY_MIME_TYPE = "application/x-vnd.Be-directory";


bool FindIconFromMime(BString& MimeType, BBitmap* IconLarge, BBitmap* IconMini);
bool MimeSupportFileExtension(BString& MimeType, BString& FileExtension);


struct IconCache_t
{
	BBitmap*	fIconLarge;
	BBitmap*	fIconMini;
	BString		fIconMime;
	BStringList	fExtensionList;
};

//
// @TODO: Cleanup ObjectList.
//
BObjectList<IconCache_t> 	fIconObjectList;
bool						fDefaultIconInitialized = false;
BBitmap*				 	fDefaultIconLargeBitmap = NULL;
BBitmap*				 	fDefaultIconMiniBitmap = NULL;

const BRect					ICON_LARGE_RECT(0, 0, B_LARGE_ICON - 1, B_LARGE_ICON - 1);
const BRect					ICON_MINI_RECT(0, 0, B_MINI_ICON - 1, B_MINI_ICON - 1);

const BBitmap* DefaultIcon(icon_size size)
{
	BString mime = B_FILE_MIME_TYPE;
	
	if( !fDefaultIconInitialized )
	{
		fDefaultIconLargeBitmap = new BBitmap(ICON_LARGE_RECT, 0, B_RGBA32);
		fDefaultIconMiniBitmap = new BBitmap(ICON_MINI_RECT, 0, B_RGBA32);
		FindIconFromMime(mime, fDefaultIconLargeBitmap, fDefaultIconMiniBitmap);
		
		fDefaultIconInitialized = true;
	}
	return (size == B_LARGE_ICON) ? fDefaultIconLargeBitmap : fDefaultIconMiniBitmap;
}

bool FindIconFromMime(BString& MimeType, BBitmap* IconLarge, BBitmap* IconMini)
{
	char 		preferred[B_MIME_TYPE_LENGTH];
	BMimeType 	mime(MimeType);

	//
	//
	//	
	if (IconLarge == NULL || IconMini == NULL)
		return false;
	
	//
	// Get the icon
	//
	if (mime.GetIcon(IconLarge, B_LARGE_ICON) == B_OK &&
		mime.GetIcon(IconMini, B_MINI_ICON) == B_OK)
		return true;
	
	//
	// Try to get the icon from preferred app
	//		
	if (mime.GetPreferredApp(preferred) == B_OK) 
	{
		BMimeType preferredApp(preferred);

		if (preferredApp.GetIconForType(MimeType, IconLarge, B_LARGE_ICON) == B_OK &&
			preferredApp.GetIconForType(MimeType, IconMini, B_MINI_ICON) == B_OK)
			return true;
	}
	
	//
	// check super type for an icon
	//
	BMimeType superType;
		
	if (mime.GetSupertype(&superType) == B_OK ) 
	{
		if (superType.GetIcon(IconLarge, B_LARGE_ICON) == B_OK &&
			superType.GetIcon(IconMini, B_MINI_ICON) == B_OK)
				return true;

		// check the super type's preferred app
		if (superType.GetPreferredApp(preferred) == B_OK)
		{
			BMimeType preferredApp(preferred);

			if (preferredApp.GetIconForType(superType.Type(), IconLarge, B_LARGE_ICON) == B_OK &&
				preferredApp.GetIconForType(superType.Type(), IconMini, B_MINI_ICON) == B_OK)
				return true;
		}
	}
	return false;
}


//
//
//
const BBitmap* CreateIconCacheFromMime(BString& MimeType, icon_size size)
{
	BMimeType 	 mime(MimeType);
	IconCache_t* IconCache = new IconCache_t;
	
	//
	//
	//
	IconCache->fIconMime = MimeType;
	IconCache->fIconLarge= new BBitmap(ICON_LARGE_RECT, 0, B_RGBA32);
	IconCache->fIconMini = new BBitmap(ICON_MINI_RECT, 0, B_RGBA32);
	
	
	//
	// If we don't get any icon, put the default.
	//
	if( !FindIconFromMime(MimeType, IconCache->fIconLarge, IconCache->fIconMini) )
	{
		delete IconCache;
		return DefaultIcon(size);
	}
	
	//
	// Build file extension from file
	//
	{
		
		BMessage  msg;
		BString	  MimeExtension;
		uint32	  i = 0;
	
		//
		//
		//
		if( mime.GetFileExtensions(&msg) == B_OK )
		{
			//
			//
			//
			while( msg.FindString("extensions", i++, &MimeExtension) == B_OK )
				IconCache->fExtensionList.Add( MimeExtension );
		}
	}
	
	//
	//
	//
	fIconObjectList.AddItem(IconCache);
	
	return (size == B_LARGE_ICON) ? IconCache->fIconLarge : IconCache->fIconMini;
}

//
//
//
const BBitmap* CreateIconCacheFromExtension(BString& FileExtension, icon_size size)
{
	BMessage 	 types;
	BString 	 MimeType = B_EMPTY_STRING;
	bool		 MimeFound = false;
	uint32		 i = 0;
	
	
	if( BMimeType::GetInstalledTypes(&types) != B_OK )
		return DefaultIcon(size);

	
	//
	//
	//
	while( types.FindString("types", i++, &MimeType) == B_OK )
	{
		//
		//
		//
		if( MimeSupportFileExtension(MimeType, FileExtension) )
		{
			MimeFound = true;
			break;
		}
	}
	
	if( !MimeFound )
		return DefaultIcon(size);


	IconCache_t* IconCache = new IconCache_t;
	
	//
	//
	//
	IconCache->fIconMime = MimeType;
	IconCache->fIconLarge= new BBitmap(ICON_LARGE_RECT, 0, B_RGBA32);
	IconCache->fIconMini = new BBitmap(ICON_MINI_RECT, 0, B_RGBA32);
	
	
	//
	// If we don't get any icon, put the default.
	//
	if( !FindIconFromMime(MimeType, IconCache->fIconLarge, IconCache->fIconMini) )
	{
		delete IconCache;
		return DefaultIcon(size);
	}
	
	//
	// Build file extension from file
	//
	{
		
		BMessage  msg;
		BString	  MimeExtension;
		BMimeType mime(MimeType);
		uint32	  i = 0;
	
		//
		//
		//
		if( mime.GetFileExtensions(&msg) == B_OK )
		{
			//
			//
			//
			while( msg.FindString("extensions", i++, &MimeExtension) == B_OK )
				IconCache->fExtensionList.Add( MimeExtension );
		}
	}
	
	//
	//
	//
	fIconObjectList.AddItem(IconCache);
	
	return (size == B_LARGE_ICON) ? IconCache->fIconLarge : IconCache->fIconMini;
}

//
//
//
const BBitmap* GetIconFromMimeCache(BString& MimeType, icon_size size)
{
	//
	//
	//
	for( int32 i = 0; i < fIconObjectList.CountItems(); ++i ) 
	{	
		//
		//
		//
		if( const IconCache_t* IconCache = fIconObjectList.ItemAt(i) )
		{
			if( IconCache->fIconMime == MimeType )
				return (size == B_LARGE_ICON) ? IconCache->fIconLarge : IconCache->fIconMini;
		}
	}
	return NULL;
}

//
//
//
const BBitmap* GetIconFromExtensionCache(BString FileExtension, icon_size size)
{
	//
	//
	//
	for( int32 i = 0; i < fIconObjectList.CountItems(); ++i ) 
	{	
		//
		//
		//
		if( const IconCache_t* IconCache = fIconObjectList.ItemAt(i) )
		{
			if( IconCache->fExtensionList.HasString(FileExtension, true) ) 
				return (size == B_LARGE_ICON) ? IconCache->fIconLarge : IconCache->fIconMini;
		}
	}
	return NULL;
}

//
//
//
const BBitmap* GetIconFromMime(BString MimeType, icon_size size)
{
	if( const BBitmap* Icon = GetIconFromMimeCache(MimeType, size) )
		return Icon;
	
	//
	//
	//
	return CreateIconCacheFromMime(MimeType, size);
}

const BBitmap* GetIconFromExtension(BString FileExtension, icon_size size)
{
	if( const BBitmap* Icon = GetIconFromExtensionCache(FileExtension, size) )
		return Icon;
	
	//
	//
	//
	return CreateIconCacheFromExtension(FileExtension, size);
}


//
//
//
bool MimeSupportFileExtension(BString& MimeType, BString& FileExtension)
{
	BMessage  msg;
	BMimeType mime(MimeType);
	BString	  MimeExtension;
	uint32	  i = 0;
	
	
	// Make sure the file extension are lower-case.
	FileExtension.ToLower();
	
	//
	//
	//
	if( mime.GetFileExtensions(&msg) != B_OK )
		return false;
	
	//
	//
	//
	while( msg.FindString("extensions", i++, &MimeExtension) == B_OK )
	{
		if( FileExtension == MimeExtension.ToLower() )
			return true;
	}
	return false;
}


