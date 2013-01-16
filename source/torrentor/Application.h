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
//	File Name:		Application.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_APPLICATION_H
#define TORRENTOR_APPLICATION_H


#include <Application.h>




struct tr_session;
struct tr_benc;

class TorrentObject;
class MainWindow;
class BStringList;


class TorrentorApp : public BApplication 
{
public:
	TorrentorApp();
	~TorrentorApp();
	
	//
	//
	//
	const tr_session* Session() const;
	
	//
	//
	//
	void MessageReceived(BMessage* message);
	void RefsReceived(BMessage* message);
	void Pulse();
	void ReadyToRun();
	
protected:
	void CreateSession();
	void LoadTorrentList();
	void LoadTorrentFromFiles(const BStringList& TorrentPathList);
	void LoadTorrentFromMagnet(BMessage* message);
	void OpenTorrentResult(BMessage* message);
	void OpenMagnetLinkWindow();
	void CheckForUpdates();
private:
	tr_session*	fTorrentSession;
	
	// @TODO: Move it to a class.
	//		  i.e [TorrentPreferences]
	tr_benc		fTorrentPreferences;

	MainWindow*	fMainWindow;
	
	BObjectList<TorrentObject>	fTorrentList;
};

#endif // TORRENTOR_APPLICATION_H
