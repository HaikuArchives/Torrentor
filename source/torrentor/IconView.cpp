//------------------------------------------------------------------------------
//	Copyright (c) 2010, Stephan Aßmus <superstippi@gmx.de>.
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
//	File Name:		IconView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include "Torrentor.h"
#include "IconUtil.h"
#include "IconView.h"


IconView::IconView(const char* MimeType)
	:	BView("IconView", B_WILL_DRAW),
		fIconMime(MimeType)
{
	SetDrawingMode(B_OP_OVER);
}




void IconView::AttachedToWindow()
{
	SetViewColor(Parent()->ViewColor());
}

void IconView::Draw(BRect updateRect)
{
	DrawBitmapAsync(GetIconFromMime(fIconMime));
}

BSize IconView::MinSize()
{
	return BSize(B_LARGE_ICON, B_LARGE_ICON);
}

BSize IconView::PreferredSize()
{
	return MinSize();
}

BSize IconView::MaxSize()
{
	return MinSize();
}
