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
//	File Name:		FileColumn.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef INTERFACE_FILE_COLUMN_H
#define INTERFACE_FILE_COLUMN_H

#include "ColumnTypes.h"

class BBitmap;

const float FILE_COLUMN_HEIGHT = 42.0f;

class FileField : public BField
{
public:
	FileField(const BBitmap* icon, const char* fileName, const char* fileInfo);
	
	
	//
	//
	//
	const BBitmap* 	Icon();
	const BString  	Name();
	const BString	Info();
private:
	const  BBitmap*	fIcon;
	BString		fName;
	BString		fInfo;

};

class FileColumn : public BTitledColumn
{
public:
	FileColumn(const char* title, float width, float minWidth, float maxWidth, alignment align = B_ALIGN_LEFT);
	
	void DrawField(BField* field, BRect rect, BView* parent);
	int CompareFields(BField*, BField*);
	bool AcceptsField(const BField* field) const;


};

#endif // INTERFACE_FILE_COLUMN_H
