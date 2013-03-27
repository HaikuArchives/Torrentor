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
//	File Name:		FolderSelect.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <string.h>

#include <Bitmap.h>
#include <ControlLook.h>
#include <MenuField.h>
#include <MimeType.h>
#include "FolderSelect.h"

#include <stdio.h>

FolderSelect::FolderSelect(const char* name, const char* label, const char* path, uint32 flags)
	:	BView(name, flags | B_FRAME_EVENTS),
		fIconBitmap(BRect(0, 0, 15, 15), 0, B_RGBA32)
{
	memset(fIconBitmap.Bits(), 0, fIconBitmap.BitsLength());
	
	//
	//
	//
	if( path != NULL )
	{
		BMimeType mime;
		
		//
		//
		//
		if( BMimeType::GuessMimeType(path, &mime) < B_NO_ERROR )
			return;
			
		//
		// We've got the mime, so get the icon ;)
		//
		mime.GetIcon(&fIconBitmap, B_MINI_ICON);
	}
}

void FolderSelect::AttachedToWindow()
{

	BView* parent = Parent();
	if (parent != NULL) {
		// inherit the color from parent
		rgb_color color = parent->ViewColor();
		if (color == B_TRANSPARENT_COLOR)
			color = ui_color(B_PANEL_BACKGROUND_COLOR);

		SetViewColor(color);
		SetLowColor(color);
	}
	
}


void FolderSelect::Draw(BRect updateFrame)
{	
		BRect frame(Bounds());
		rgb_color base = LowColor();
		rgb_color background = LowColor();
		uint32 flags = 0;
		
		

		//
		// Draw the frame.
		//
		be_control_look->DrawMenuFieldFrame(this, frame, updateFrame, base,
											background, flags);

		//
		// Draw the background
		//
		base = ui_color(B_PANEL_BACKGROUND_COLOR);
		/*uint32*/ flags = 0;
//		if (!IsEnabled())
//			flags |= BControlLook::B_DISABLED;
//		if (IsFocus())
//			flags |= BControlLook::B_FOCUSED;
		be_control_look->DrawMenuFieldBackground(this, frame,
			updateFrame, base, true/*fShowPopUpMarker*/, flags);

	//	_DrawItems(updateRect);
	
	//
	// @TODO: DrawIconAndLabel(..)
	//
	SetDrawingMode(B_OP_OVER);
	
	float _icon_y = Bounds().top
				+ (Bounds().Height() + 1 - fIconBitmap.Bounds().Height()) / 2;
	
	//
	// Draw the icon.
	//
	DrawBitmapAsync(&fIconBitmap, BPoint(8, _icon_y));
	
	//
	// Draw the label.
	//
	SetLowColor(ViewColor());
	
	BFont font;
	GetFont(&font);


	
	font_height fh;
	
	font.GetHeight(&fh);

	
	float x = 8 + fIconBitmap.Bounds().Width() + 8;
	float y = Bounds().top
				+ (Bounds().Height() + 1 - fh.ascent - fh.descent) / 2
				+ fh.ascent;
	y = floor(y + 0.5);


	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
			/*IsEnabled() ?*/ B_DARKEN_MAX_TINT /* : B_DISABLED_LABEL_TINT*/));
		DrawString("/home/test", BPoint(x, y));


/*
	BRect frame(Bounds());
	
	
	//frame.InsetBy(-kVMargin, -kVMargin);
	rgb_color base = LowColor();
	rgb_color background = LowColor();
	
	uint32 flags = 0;
	//if (!fMenuBar->IsEnabled())
			flags |= BControlLook::B_DISABLED;
	//	if (active)
	//		flags |= BControlLook::B_FOCUSED;
		be_control_look->DrawMenuFieldFrame(this, frame, updateFrame, base,
			background, flags);
*/

}


BSize FolderSelect::MinSize()
{
	return BSize(42, 28);
}

BSize FolderSelect::MaxSize()
{
	return BSize(B_SIZE_UNLIMITED, 28);
}


