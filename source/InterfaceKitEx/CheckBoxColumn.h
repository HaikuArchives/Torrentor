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
//	File Name:		CheckBoxColumn.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef INTERFACE_CHECK_BOX_COLUMN_H
#define INTERFACE_CHECK_BOX_COLUMN_H

#include "ColumnTypes.h"

class BBitmap;

class CheckBoxField : public BField
{
public:
	CheckBoxField(bool selected = false);
	
	void Toggle();
	bool Selected() const;
private:
	bool	fSelected;
};

class CheckBoxColumn : public BTitledColumn
{
public:
	CheckBoxColumn(const char* title, float width, float minWidth, float maxWidth, alignment align = B_ALIGN_LEFT);
	
	void DrawField(BField* field, BRect rect, BView* parent);
	int CompareFields(BField*, BField*);
	bool AcceptsField(const BField* field) const;


	void MouseDown(BColumnListView* /*parent*/, BRow* /*row*/,
					BField* /*field*/, BRect /*field_rect*/, BPoint /*point*/,
					uint32 /*buttons*/);
};

#endif // INTERFACE_CHECK_BOX_COLUMN_H
