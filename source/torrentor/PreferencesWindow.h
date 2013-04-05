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
//	File Name:		PreferencesWindow.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_PREFERENCES_WINDOW_H
#define TORRENTOR_PREFERENCES_WINDOW_H

class BButton;
class BCheckBox;
class BTextControl;
class BView;
class FolderSelect;
class TorrentPreferences;

struct tr_session;

class PreferencesWindow : public BWindow
{
public:
	PreferencesWindow();
	
	void MessageReceived(BMessage* message);
	bool QuitRequested();
protected:
	BView*		_CreateTorrentsPage(float spacing);
	BView*		_CreateConnectionPage(float spacing);
	BMessage*	_CreateEncryptionMenuMessage(int32 index);
	
	//
	void 		_DownloadFolderChanged(BMessage* message);
	void		_IncompleteFolderChanged(BMessage* message);
	void		_IncomingPortChanged(BMessage* message);
	void		_IncomingPortRandomChanged(BMessage* message);
	void		_PortForwardingChanged(BMessage* message);
	void		_PeerLimitChanged(BMessage* message);
	void		_PeerLimitPerTorrentChanged(BMessage* message);
	void		_DistributedHashTableChanged(BMessage* message);
	void		_TorrentPeerExchangeChanged(BMessage* message);
	void		_MicroTransportProtocolChanged(BMessage* message);
	void		_LocalPeerDiscoveryChanged(BMessage* message);
	void		_EncryptionModeChanged(BMessage* message);
private:
	TorrentPreferences*	fTorrentPreferences;
	BButton*			fApplyButton;
	BCheckBox* 			fIncompleteFileNaming;
	FolderSelect* 		fTorrentSaveLocationPath;
	BCheckBox* 			fIncompleteDirEnabled;
	FolderSelect* 		fIncompleteDirPath;
	BTextControl* 		fListeningPort;
	BButton* 			fRandomPort;
	BButton*			fApplyPort;
	BCheckBox* 			fEnableForwardingPort;
	BTextControl* 		fMaxConnection;
	BButton*			fApplyMaxConnection;
	BTextControl* 		fTorrentMaxConnection;
	BButton*			fApplyTorrentMaxConnection;
	BCheckBox* 			fEnableDHTValue;
	BCheckBox* 			fEnablePEXValue;
	BCheckBox* 			fEnableUTPValue;
	BCheckBox* 			fEnableLPDValue;
	BMenuField* 		fEncryptionMenu;
	BMenuItem* 			fEncryptionMenuItem[3];

};


#endif // TORRENTOR_SETTINGS_WINDOW_H
