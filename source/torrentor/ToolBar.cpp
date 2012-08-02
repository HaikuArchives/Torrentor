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
//	File Name:		ToolBar.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------

#include "ToolBar.h"



ToolBar::ToolBar(BRect frame) : BView(frame, "ToolBar", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

ToolBar::~ToolBar()
{
	
}

void ToolBar::Draw(BRect update)
{
	
	rgb_color bgColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color shineColor = ui_color(B_SHINE_COLOR);
	rgb_color halfDarkColor = tint_color(bgColor, B_DARKEN_1_TINT);
	rgb_color darkColor = tint_color(bgColor, B_DARKEN_2_TINT);
	// Draws a beveled smooth border
	BeginLineArray(4);
	AddLine(Bounds().LeftTop(), Bounds().RightTop(), shineColor);
	AddLine(Bounds().LeftTop(), Bounds().LeftBottom() - BPoint(0, 1), shineColor);
	AddLine(Bounds().LeftBottom() - BPoint(-1, 1), Bounds().RightBottom() - BPoint(0, 1), halfDarkColor);
	AddLine(Bounds().LeftBottom(), Bounds().RightBottom(), darkColor);
	EndLineArray();
}


float ToolBar::CalculateHeight(void)
{
	// Empiric formula from how much space the textview
	// will take once it is attached (using be_plain_font):
	return  ceilf(11.0f + be_plain_font->Size()*(1.0f + 7.0f / 30.0f));
}
