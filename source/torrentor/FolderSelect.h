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
//	File Name:		FolderSelect.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_FOLDER_SELECT_H
#define TORRENTOR_FOLDER_SELECT_H

#include <Bitmap.h>
#include <Button.h>

class BPath;
class BSize;
class BFilePanel;

class FolderSelect : public BButton
{
public:
	FolderSelect(const char* name,
				 const char* path,
				 BMessage* message = NULL,
				 uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
				 
	virtual ~FolderSelect();
	
	virtual void AttachedToWindow();
	virtual void MessageReceived(BMessage* message);
	virtual void Draw(BRect updateFrame);
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* message);

	virtual	void	GetPreferredSize(float* _width, 
						float* _height);
	virtual BSize MinSize();
	virtual BSize MaxSize();
	virtual BSize PreferredSize();
	
	
	virtual status_t Invoke(BMessage* message = NULL);
	
	void OnRefReceived(BMessage* message);
protected:
	virtual	void				LayoutInvalidated(bool descendants = false);
private:
	void	_UpdateIconBitmap();
	BSize 	_ValidatePreferredSize();
private:
	bool	fActive;
	BPath	fFolderPath;
	BBitmap	fIconBitmap;
	BSize	fPreferredSize;
	BFilePanel* fFilePanel;
};


#endif // TORRENTOR_FOLDER_SELECT_H
