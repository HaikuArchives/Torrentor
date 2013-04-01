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
#include <Catalog.h>
#include <ControlLook.h>
#include <Cursor.h>
#include <FilePanel.h>
#include <LayoutUtils.h>
#include <MenuField.h>
#include <Mime.h>
#include <MimeType.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <String.h>
#include <Window.h>

#include "FolderSelect.h"
#include "IconUtil.h"

#include <stdio.h>
#include <assert.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FolderSelect"

const int kIconPadding = 8;

enum {
	MSG_FOLDER_SELECT_REFS_RECEIVED	= 'FSRR',
};

FolderSelect::FolderSelect(const char* name, const char* path, BMessage* message, uint32 flags)
	:	BButton(name, "", message, flags | B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
		fFolderPath(path),
		fIconBitmap(BRect(0, 0, 15, 15), 0, B_RGBA32),
		fFilePanel(NULL)
{
	memset(fIconBitmap.Bits(), 0, fIconBitmap.BitsLength());
	
	_UpdateIconBitmap();
}

FolderSelect::~FolderSelect()
{
	delete fFilePanel;
}

void FolderSelect::AttachedToWindow()
{
	BButton::AttachedToWindow();
	if (BView* parent = Parent()) {
		// inherit the color from parent
		rgb_color color = parent->ViewColor();
		if (color == B_TRANSPARENT_COLOR)
			color = ui_color(B_PANEL_BACKGROUND_COLOR);

		SetViewColor(color);
		SetLowColor(color);
	}
}


void FolderSelect::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_FOLDER_SELECT_REFS_RECEIVED:
			OnRefReceived(message);
			break;
		default:
			BControl::MessageReceived(message);
			break;
	}
}

void FolderSelect::OnRefReceived(BMessage* message)
{
	void* folderSelectObject = NULL;
	
	if (message->FindPointer("folderSelectObject", &folderSelectObject) != B_OK)
		return;
	
	//
	// Be sure we're the owner.
	//
	if (folderSelectObject != this)
		return;
	
	//
	entry_ref ref;
	if (message->FindRef("refs", 0, &ref) == B_OK) {
		BEntry entry(&ref, true);
		entry.GetRef(&ref);
		
		if (fFolderPath.SetTo(&ref) == B_OK) {
			_UpdateIconBitmap();
			InvalidateLayout();
			Invalidate();
			
			//
			BMessage copy(*Message());
			copy.AddRef("ref", &ref);
			BButton::Invoke(&copy);
		}
	}
}


void FolderSelect::MouseMoved(BPoint where, uint32 transit, const BMessage* message)
{
	BButton::MouseMoved(where, transit, message);
	
	if (transit == B_ENTERED_VIEW) {
		BCursor cursor(IsEnabled() ? B_CURSOR_ID_FOLLOW_LINK : B_CURSOR_ID_SYSTEM_DEFAULT);
		SetViewCursor(&cursor, true);
	}
}

void FolderSelect::_UpdateIconBitmap()
{
	//
	//
	//
	if (fFolderPath.InitCheck() == B_NO_INIT) {
		fIconBitmap = GetIconFromMime(B_DIRECTORY_MIME_TYPE, B_MINI_ICON);
		return;
	}
	
	BNode node(fFolderPath.Path());
	BNodeInfo nodeInfo(&node);
	
	nodeInfo.GetTrackerIcon(&fIconBitmap, B_MINI_ICON);
}


void FolderSelect::Draw(BRect updateFrame)
{
	BRect frame(Bounds());
	rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color background = LowColor();
	rgb_color labelColor = tint_color(base, IsEnabled() ? B_DARKEN_MAX_TINT : B_DARKEN_3_TINT);
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
	flags = 0;
	if (!IsEnabled())
		flags |= BControlLook::B_DISABLED;

	if (IsFocus())
		flags |= BControlLook::B_FOCUSED;

	be_control_look->DrawMenuFieldBackground(this, frame,
			updateFrame, base, true, flags);

	//
	// @TODO: DrawIconAndLabel(..)
	//
	SetDrawingMode(B_OP_OVER);
	
	float _icon_y = Bounds().top
				+ (Bounds().Height() + 1 - fIconBitmap.Bounds().Height()) / 2;
	
	//
	// Draw the icon.
	//
	if (!IsEnabled()) {
		SetDrawingMode(B_OP_ALPHA);
    	SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
    	SetHighColor(0, 0, 0, 150);
	}
	DrawBitmapAsync(&fIconBitmap, BPoint(8, _icon_y));
	
	//
	// Draw the label.
	//
	SetLowColor(ViewColor());
	
	BFont font;
	GetFont(&font);

	font_height fh;	
	font.GetHeight(&fh);

	
	float x = kIconPadding + fIconBitmap.Bounds().Width() + kIconPadding;
	float y = Bounds().top
				+ (Bounds().Height() + 1 - fh.ascent - fh.descent) / 2
				+ fh.ascent;
	y = floor(y + 0.5);


	SetHighColor(labelColor);
	DrawString(fFolderPath.Leaf(), BPoint(x, y));
}

#pragma mark -
status_t FolderSelect::Invoke(BMessage *message)
{
	Sync();
	snooze(50000);
	
	//
	//
	//
	entry_ref ref;
	get_ref_for_path(fFolderPath.Path(), &ref);
	
	//
	//
	//
	if (fFilePanel == NULL) {
		BMessenger messenger(this);
		BMessage* msg = new BMessage(MSG_FOLDER_SELECT_REFS_RECEIVED);
		msg->AddPointer("folderSelectObject", this);

		fFilePanel = new BFilePanel(B_OPEN_PANEL, &messenger, &ref, 
			B_DIRECTORY_NODE, false, msg, NULL, true);
	}
	fFilePanel->SetPanelDirectory(&ref);
	fFilePanel->Show();
	fFilePanel->SetButtonLabel(B_DEFAULT_BUTTON, B_TRANSLATE("Select"));
	
	//
	//
	//
	//status_t err = BControl::Invoke(message);

	SetValue(B_CONTROL_OFF);

	return B_OK;
}

#pragma mark -

BSize FolderSelect::_ValidatePreferredSize()
{
	if (!fPreferredSize.IsWidthSet()) 
	{
		font_height fontHeight;
		GetFontHeight(&fontHeight);

		float width = kIconPadding + fontHeight.ascent + B_MINI_ICON + 
			kIconPadding + StringWidth(fFolderPath.Leaf()) + kIconPadding;

		//if (Label())
			//width += StringWidth(Label());

		fPreferredSize.width = (float)ceil(width);
		fPreferredSize.height = (float)ceil(B_MINI_ICON + 12.0f);

		ResetLayoutInvalidation();
	}

	return fPreferredSize;
}

void FolderSelect::LayoutInvalidated(bool descendants)
{
	// invalidate cached preferred size
	fPreferredSize.Set(B_SIZE_UNSET, B_SIZE_UNSET);
}

void FolderSelect::GetPreferredSize(float* _width, float* _height)
{
	_ValidatePreferredSize();

	if (_width)
		*_width = fPreferredSize.width;

	if (_height)
		*_height = fPreferredSize.height;
}


BSize FolderSelect::MinSize()
{
	return BLayoutUtils::ComposeSize(ExplicitMinSize(),
		_ValidatePreferredSize());
}

BSize FolderSelect::MaxSize()
{
	return BLayoutUtils::ComposeSize(ExplicitMaxSize(),
		BSize(B_SIZE_UNLIMITED, _ValidatePreferredSize().height));
}

BSize FolderSelect::PreferredSize()
{
	return BLayoutUtils::ComposeSize(ExplicitPreferredSize(),
		_ValidatePreferredSize());
}




