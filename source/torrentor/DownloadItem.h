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
//	File Name:		DownloadItem.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_DOWNLOAD_ITEM_H
#define TORRENTOR_DOWNLOAD_ITEM_H

#include <String.h>

class DownloadView;
class BStringView;
class BStatusBar;
class ControlButton;
class IconView;
class TorrentObject;

const uint32 MSG_UPDATE_DOWNLOAD= 0x249142;


class DownloadItem : public BGroupView 
{
public:	
	DownloadItem(DownloadView* parentView, TorrentObject* torrentObject);
	~DownloadItem();
	
	void AttachedToWindow();
	
	void Draw(BRect updateRect);
	void AllAttached();
	void MessageReceived(BMessage* message);
	void MouseDown(BPoint point);
	
	//
	//
	//
	void UpdateInfo();
	
	
	bool IsSelected() const;


	void Select();
	void Deselect();
	
	void ToggleSelection();
	
	const TorrentObject* GetTorrentObject() const;
	
protected:
	void OnUpdateDownload(BMessage* Msg);
	
	void OnSelectionChange();
	
	void UpdateStatusText();
	void UpdateProgressText();
private:
	bool			fSelected;
	BString			fStatusText;
	BString			fProgressText;
	IconView*		fIconView;
	BStringView*	fInfoView;
	BStringView*	fPeerView;
	BStringView*	fNameView;
	BStatusBar*		fProgressBar;
	ControlButton* 	fControlButton;
	DownloadView*	fDownloadView;
	TorrentObject* 	fTorrentObject;

};

#endif // TORRENTOR_DOWNLOAD_ITEM_H
