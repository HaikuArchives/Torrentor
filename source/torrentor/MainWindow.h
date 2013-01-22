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
//	File Name:		MainWindow.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_MAIN_WINDOW_H
#define TORRENTOR_MAIN_WINDOW_H


#include <File.h>
#include <FilePanel.h>
#include <Window.h>

//
//
//
class DownloadView;
class PreferencesWindow;

class MainWindow : public BWindow 
{
public:
					MainWindow(BRect frame); 
					~MainWindow();
	virtual	bool	QuitRequested();
	
	void MessageReceived(BMessage* message);
	void Pulse();
	
	//
	//
	//
	void AddTorrent(TorrentObject* torrentObject);
	
	//void LoadTorrentFromFile(entry_ref* ref);
	
	void Update();
protected:
	void InitWindow();
	void CreateMenuBar();
	
	
	void OpenPreferencesWindow();
	void OpenTorrentDownloadFolder();
	void OnTorrentInspect();
	void OnMenuTorrentRemove();
	void OnMenuStartAllTorrents();
	void OnMenuStopAllTorrents();
private:
	//BListView*		mlistView;
	//BFilePanel*		mFilePanel;	
	BFilePanel*			fOpenPanel;
	BMenuBar* 			fMenuBar;
	DownloadView*		fDownloadView;
	PreferencesWindow*	fPreferencesWindow;
};

#endif // TORRENTOR_MAIN_WINDOW_H
