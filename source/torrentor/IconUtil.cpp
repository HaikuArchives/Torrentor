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
//------------------------------------------------------------------------------
#include <cstring>

#include <Bitmap.h>
#include <StringList.h>

#include "Torrentor.h"
#include "IconUtil.h"




bool FindIconFromMime(BString& MimeType, BBitmap* Icon);
bool MimeSupportFileExtension(BString& MimeType, BString& FileExtension);


struct IconCache_t
{
	BBitmap*	fIcon;
	BString		fIconMime;
	BStringList	fExtensionList;
};

//
// @TODO: Cleanup ObjectList.
//
BObjectList<IconCache_t> IconObjectList;
BBitmap*				 DefaultIconBitmap = NULL;


const BBitmap* DefaultIcon()
{
	if( DefaultIconBitmap == NULL )
	{
		BString mime = B_FILE_MIME_TYPE;
		DefaultIconBitmap = new BBitmap(BRect(0, 0, 31, 31), 0, B_RGBA32);
		FindIconFromMime(mime, DefaultIconBitmap);
	}
	return DefaultIconBitmap;
}

bool FindIconFromMime(BString& MimeType, BBitmap* Icon)
{
	char 		preferred[B_MIME_TYPE_LENGTH];
	BMimeType 	mime(MimeType);

	//
	//
	//	
	if( Icon == NULL )
		return false;
	
	//
	// Get the icon
	//
	if( mime.GetIcon(Icon, B_LARGE_ICON) == B_OK )
		return true;
	
	//
	// Try to get the icon from preferred app
	//		
	if( mime.GetPreferredApp(preferred) == B_OK) 
	{
		BMimeType preferredApp(preferred);

		if( preferredApp.GetIconForType(MimeType, Icon, B_LARGE_ICON) == B_OK )
			return true;
	}
	
	//
	// check super type for an icon
	//
	BMimeType superType;
		
	if( mime.GetSupertype(&superType) == B_OK ) 
	{
		if (superType.GetIcon(Icon, B_LARGE_ICON) == B_OK)
				return true;

		// check the super type's preferred app
		if( superType.GetPreferredApp(preferred) == B_OK ) 
		{
			BMimeType preferredApp(preferred);

			if( preferredApp.GetIconForType(superType.Type(), Icon, B_LARGE_ICON) == B_OK)
				return true;
		}
	}
	return false;
}


//
//
//
const BBitmap* CreateIconCacheFromMime(BString& MimeType)
{
	BMimeType 	 mime(MimeType);
	IconCache_t* IconCache = new IconCache_t;
	
	//
	//
	//
	IconCache->fIconMime = MimeType;
	IconCache->fIcon 	 = new BBitmap(BRect(0, 0, 31, 31), 0, B_RGBA32);
	
	
	//
	// If we don't get any icon, put the default.
	//
	if( !FindIconFromMime(MimeType, IconCache->fIcon) )
	{
		delete IconCache;
		return DefaultIcon();
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
	IconObjectList.AddItem(IconCache);
	
	return IconCache->fIcon;
}

//
//
//
const BBitmap* CreateIconCacheFromExtension(BString& FileExtension)
{
	BMessage 	 types;
	BString 	 MimeType = B_EMPTY_STRING;
	bool		 MimeFound = false;
	uint32		 i = 0;
	
	
	if( BMimeType::GetInstalledTypes(&types) != B_OK )
		return DefaultIcon();

	
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
		return DefaultIcon();


	IconCache_t* IconCache = new IconCache_t;
	
	//
	//
	//
	IconCache->fIconMime = MimeType;
	IconCache->fIcon 	 = new BBitmap(BRect(0, 0, 31, 31), 0, B_RGBA32);
	
	
	//
	// If we don't get any icon, put the default.
	//
	if( !FindIconFromMime(MimeType, IconCache->fIcon) )
	{
		delete IconCache;
		return DefaultIcon();
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
	IconObjectList.AddItem(IconCache);
	
	return IconCache->fIcon;
}

//
//
//
const BBitmap* GetIconFromMimeCache(BString& MimeType)
{
	//
	//
	//
	for( int32 i = 0; i < IconObjectList.CountItems(); ++i ) 
	{	
		//
		//
		//
		if( const IconCache_t* IconCache = IconObjectList.ItemAt(i) )
		{
			if( IconCache->fIconMime == MimeType )
				return IconCache->fIcon;
		}
	}
	return NULL;
}

//
//
//
const BBitmap* GetIconFromExtensionCache(BString FileExtension)
{
	//
	//
	//
	for( int32 i = 0; i < IconObjectList.CountItems(); ++i ) 
	{	
		//
		//
		//
		if( const IconCache_t* IconCache = IconObjectList.ItemAt(i) )
		{
			if( IconCache->fExtensionList.HasString(FileExtension, true) ) 
				return IconCache->fIcon;
		}
	}
	return NULL;
}

//
//
//
const BBitmap* GetIconFromMime(BString MimeType)
{
	if( const BBitmap* Icon = GetIconFromMimeCache(MimeType) )
		return Icon;
	
	//
	//
	//
	return CreateIconCacheFromMime(MimeType);
}

const BBitmap* GetIconFromExtension(BString FileExtension)
{
	if( const BBitmap* Icon = GetIconFromExtensionCache(FileExtension) )
		return Icon;
	
	//
	//
	//
	return CreateIconCacheFromExtension(FileExtension);
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


