//------------------------------------------------------------------------------
//	Copyright (c) 2010, Stephan Aßmus <superstippi@gmx.de>.
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
//	File Name:		DocumentView.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include "Torrentor.h"
#include "IconView.h"


IconView::IconView(const char* MimeType)
	:	BView("IconView", B_WILL_DRAW),
		fIconBitmap(BRect(0, 0, 31, 31), 0, B_RGBA32),
		fDimmedIcon(false)
{
	SetDrawingMode(B_OP_OVER);
	memset(fIconBitmap.Bits(), 0, fIconBitmap.BitsLength());
	
	//
	//
	//
	if( MimeType == NULL )
		MimeType = B_FILE_MIME_TYPE;
		
	
	BMimeType mime(MimeType);
	
	mime.GetIcon(&fIconBitmap, B_LARGE_ICON);
}




void IconView::SetTo(const BEntry& entry)
{
	BNode node(&entry);
	BNodeInfo info(&node);
	info.GetTrackerIcon(&fIconBitmap, B_LARGE_ICON);
	Invalidate();
}

void IconView::SetIconDimmed(bool iconDimmed)
{
	//
	//
	//
	if( fDimmedIcon == iconDimmed )
		return;
	
	fDimmedIcon = iconDimmed;
	Invalidate();
}

bool IconView::IsIconDimmed() const
{
	return fDimmedIcon;
}

status_t IconView::SaveSettings(BMessage* archive)
{
	return fIconBitmap.Archive(archive);
}

void IconView::AttachedToWindow()
{
	SetViewColor(Parent()->ViewColor());
}

void IconView::Draw(BRect updateRect)
{
	if (fDimmedIcon) {
		SetDrawingMode(B_OP_ALPHA);
		SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
		SetHighColor(0, 0, 0, 100);
	}
	DrawBitmapAsync(&fIconBitmap);
}

BSize IconView::MinSize()
{
	return BSize(fIconBitmap.Bounds().Width(), fIconBitmap.Bounds().Height());
}

BSize IconView::PreferredSize()
{
	return MinSize();
}

BSize IconView::MaxSize()
{
	return MinSize();
}
