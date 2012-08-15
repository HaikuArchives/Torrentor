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
//	File Name:		FileColumn.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include "ColumnTypes.h"
#include "FileColumn.h"


FileField::FileField(const BBitmap* icon, const char* fileName, const char* fileInfo)
	:	fIcon(icon),
		fName(fileName),
		fInfo(fileInfo)
{
	//.
}

const BBitmap* FileField::Icon() { return fIcon; }
const BString FileField::Name() { return fName; }
const BString FileField::Info() { return fInfo; }

FileColumn::FileColumn(const char* title, float width, float minWidth, float maxWidth, alignment align)
	:	BTitledColumn(title, width, minWidth, maxWidth, align)
{
/*
	font_height	fh;

	be_plain_font->GetHeight(&fh);
	fFontHeight = fh.descent + fh.leading;
*/
}

void FileColumn::DrawField(BField* field, BRect rect, BView* parent)
{
	FileField* fileField = static_cast<FileField*>(field);
	const BBitmap* 	icon = fileField->Icon();
	const BString	name = fileField->Name();
	const BString	info = fileField->Info();
	
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
}


int FileColumn::CompareFields(BField* /*field1*/, BField* /*field2*/)
{
	return 0;
}


bool FileColumn::AcceptsField(const BField* field) const
{
	return static_cast<bool>(dynamic_cast<const FileField*>(field));
}

