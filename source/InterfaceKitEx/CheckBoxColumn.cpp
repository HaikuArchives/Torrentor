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
//	File Name:		FileColumn.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <ControlLook.h>
#include "ColumnTypes.h"
#include "CheckBoxColumn.h"


BRect _CheckBoxFrame(BView* parent, BRect rect)
{
	font_height fontHeight;
	parent->GetFontHeight(&fontHeight);
	
	BRect BoxFrameRect(0.0f, 2.0f, ceilf(3.0f + fontHeight.ascent),
			ceilf(5.0f + fontHeight.ascent));
			
			
	float top = rect.top + ((rect.Height() - BoxFrameRect.Height()) / 2);
	float left = rect.left + ((rect.Width() - BoxFrameRect.Width()) / 2);

	//
	//
	//
	return BRect(BoxFrameRect.left + left, BoxFrameRect.top + top,
				 BoxFrameRect.right + left, BoxFrameRect.bottom + top);
}


CheckBoxField::CheckBoxField(bool selected)
	:	fSelected(selected)
{
	//.
}

void CheckBoxField::Toggle() { fSelected = !fSelected; }
bool CheckBoxField::Selected() const { return fSelected; }

CheckBoxColumn::CheckBoxColumn(const char* title, float width, float minWidth, float maxWidth, alignment align)
	:	BTitledColumn(title, width, minWidth, maxWidth, align)
{
	SetWantsEvents(true);
}

void CheckBoxColumn::MouseDown(BColumnListView* parent, BRow* row,
		BField* field, BRect field_rect, BPoint point, uint32 buttons)
{
	CheckBoxField* checkField = static_cast<CheckBoxField*>(field);
	BRect checkFrame = _CheckBoxFrame(parent, field_rect);
	
	//
	// check if we're inside the checkbox frame.
	//
	if( checkFrame.Contains(point) )
		checkField->Toggle();
}




void CheckBoxColumn::DrawField(BField* field, BRect rect, BView* parent)
{
	CheckBoxField* checkField = static_cast<CheckBoxField*>(field);

	if( be_control_look == NULL )
		return;
		
	//
	//
	//
	rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);
	uint32 flags = BControlLook::B_BLEND_FRAME;
	
/*
uint32
BControlLook::Flags(BControl* control) const
{
	uint32 flags = 0;

	if (!control->IsEnabled())
		flags |= B_DISABLED;

	if (control->IsFocus())
		flags |= B_FOCUSED;

	if (control->Value() == B_CONTROL_ON)
		flags |= B_ACTIVATED;

	if (control->Parent() != NULL
		&& (control->Parent()->Flags() & B_DRAW_ON_CHILDREN) != 0) {
		// In this constellation, assume we want to render the control
		// against the already existing view contents of the parent view.
		flags |= B_BLEND_FRAME;
	}

	return flags;
}
*/
	
	
	if( checkField->Selected() )
		flags |= BControlLook::B_ACTIVATED;
		
	
	BRect checkBoxRect(_CheckBoxFrame(parent, rect));
	//BRect rect(checkBoxRect);
	
	be_control_look->DrawCheckBox(parent, checkBoxRect, rect, base, flags);

/*
	BRect labelRect(rect);
	labelRect.left = checkBoxRect.right
			+ be_control_look->DefaultLabelSpacing();

	be_control_look->DrawLabel(this, Label(), labelRect, updateRect,
			base, flags);*/


/*
	
	float x = 2.0f;
	
	//
	//
	//
	if( icon != NULL )
	{
		BRect r = icon->Bounds();
		float y = rect.top + ((rect.Height() - r.Height()) / 2);
		
		drawing_mode oldMode = parent->DrawingMode();
		if( icon->ColorSpace() == B_RGBA32 || icon->ColorSpace() == B_RGBA32_BIG) 
		{
			parent->SetDrawingMode(B_OP_ALPHA);
			parent->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
		} 
		else 
		{
			parent->SetDrawingMode(B_OP_OVER);
		}

		parent->DrawBitmap(icon, BPoint(x + rect.left, y));
		parent->SetDrawingMode(oldMode);
		
		x += r.Width() + 6.0f;
	}
	

	BFont font;
	font_height	finfo;

	parent->GetFont(&font);
	
	//parent->SetFont(be_bold_font);
	font.GetHeight(&finfo);
	
	float top = rect.top + 2.0f + finfo.ascent + finfo.descent;

	//
	//
	//
	parent->DrawString(name, BPoint(x + rect.left, top));
	
	
	BFont smallFont;
	parent->GetFont(&smallFont);
	float fontSize = smallFont.Size() * 0.8f;
	smallFont.SetSize(max_c(8.0f, fontSize));
	parent->SetFont(&smallFont, B_FONT_SIZE);
	
	
	smallFont.GetHeight(&finfo);
	
	parent->DrawString(info, BPoint(x + rect.left, top + 2.0f + finfo.ascent + finfo.descent));
	parent->SetFont(&font);
*/
}


int CheckBoxColumn::CompareFields(BField* /*field1*/, BField* /*field2*/)
{
	return 0;
}


bool CheckBoxColumn::AcceptsField(const BField* field) const
{
	return static_cast<bool>(dynamic_cast<const CheckBoxField*>(field));
}

