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
//	File Name:		AddTorrentWindow.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_ADD_TORRENT_WINDOW_H
#define TORRENTOR_ADD_TORRENT_WINDOW_H

class BButton;
class BCheckBox;
class BColumnListView;
class BStatusBar;
class InfoHeaderView;
class TorrentObject;


class AddTorrentWindow : public BWindow
{
public:
	AddTorrentWindow(TorrentObject* torrent);
	
	virtual void DispatchMessage(BMessage* msg, BHandler* target);
	void MessageReceived(BMessage* message);
	bool QuitRequested();

protected:
	void UpdateFileList();
	void OnMetadataComplete();
	void OnButtonAddClick();
private:
	TorrentObject*		fTorrent;
	bool				fCancelAdd;
	InfoHeaderView*		fInfoHeaderView;
	BCheckBox*			fStartCheckBox;
	BButton*			fCancelButton;
	BButton*			fAddButton;
	BStatusBar*			fLoadingView;
	BColumnListView*	fFileList;
};

#endif // TORRENTOR_ADD_TORRENT_WINDOW_H
