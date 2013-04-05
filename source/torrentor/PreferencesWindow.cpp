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
//	File Name:		PreferencesWindow.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <Button.h>
#include <CheckBox.h>
#include <ControlLook.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <SpaceLayoutItem.h>
#include <TabView.h>
#include <TextControl.h>
#include <Window.h>

#include <libtransmission/transmission.h>

#include "Torrentor.h"
#include "Application.h"
#include "FolderSelect.h"
#include "TorrentPreferences.h"
#include "PreferencesWindow.h"

#define UTF8_GREEK_MU_LETTER "\xCE\xBC"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PreferencesWindow"

static const BRect kDefaultFrame = BRect(120, 80, 800, 600);

enum
{
	//MSG_CONTROL_BEHAVIOR_CHANGED = 'mcbc',
	MSG_START_WHEN_ADDED_BEHAVIOR_CHANGED			= 'stwa',
	MSG_INCOMPLETE_FOLDER_BEHAVIOR_CHANGED			= 'info',
	MSG_INCOMPLETE_FOLDER_PATH_BEHAVIOR_CHANGED		= 'infp',
	MSG_INCOMPLETE_FILENAMING_BEHAVIOR_CHANGED		= 'infn',
	MSG_DOWNLOAD_FOLDER_BEHAVIOR_CHANGED			= 'dowf',
	MSG_INCOMING_PORT_BEHAVIOR_CHANGED				= 'inpo',
	MSG_INCOMING_PORT_RANDOM_BEHAVIOR_CHANGED		= 'inpr',
	MSG_PORT_FORWARDING_BEHAVIOR_CHANGED			= 'pofw',
	MSG_PEER_LIMIT_BEHAVIOR_CHANGED					= 'peli',
	MSG_PEER_LIMIT_PER_TORRENT_BEHAVIOR_CHANGED		= 'plpt',
	MSG_DISTRIBUTED_HASH_TABLE_BEHAVIOR_CHANGED 	= '_dht',
	MSG_TORRENT_PEER_EXCHANGE_BEHAVIOR_CHANGED		= '_pex',
	MSG_MICRO_TRANSPORT_PROTOCOL_BEHAVIOR_CHANGED	= '_utp',
	MSG_LOCAL_PEER_DISCOVERY_BEHAVIOR_CHANGED		= '_lpd',
	MSG_ENCRYPTION_BEHAVIOR_CHANGED					= 'encb',
};

PreferencesWindow::PreferencesWindow()
	:	BWindow(kDefaultFrame, "Preferences", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS | 
				B_ASYNCHRONOUS_CONTROLS | 
				B_NOT_ZOOMABLE),
		fTorrentPreferences(NULL),
		fApplyButton(NULL),
		fIncompleteFileNaming(NULL),
		fTorrentSaveLocationPath(NULL),
		fIncompleteDirEnabled(NULL),
		fIncompleteDirPath(NULL),
		fListeningPort(NULL),
		fRandomPort(NULL),
		fApplyPort(NULL),
		fEnableForwardingPort(NULL),
		fMaxConnection(NULL),
		fApplyMaxConnection(NULL),
		fTorrentMaxConnection(NULL),
		fApplyTorrentMaxConnection(NULL),
		fEnableDHTValue(NULL),
		fEnablePEXValue(NULL),
		fEnableUTPValue(NULL),
		fEnableLPDValue(NULL),
		fEncryptionMenu(NULL)

{
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	//
	// Get the torrent session.
	// @TODO: clean up this mess..
	//
	fTorrentPreferences = static_cast<TorrentorApp*>(be_app)->Preferences();
	float spacing = be_control_look->DefaultItemSpacing();

	BTabView* tabView = new BTabView("settings pages", B_WIDTH_FROM_LABEL);
	
	
	tabView->AddTab(_CreateTorrentsPage(spacing));
	tabView->AddTab(_CreateConnectionPage(spacing));

	AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.Add(tabView)
		.SetInsets(spacing, spacing, spacing, spacing)
	);

	CenterOnScreen();
	Run();
}


BView* PreferencesWindow::_CreateTorrentsPage(float spacing)
{
	BStringView* addingLabel 		= new BStringView("", B_TRANSLATE("Adding"));
	BStringView* downloadingLabel 	= new BStringView("", B_TRANSLATE("Downloading"));
	//BStringView* seedingLabel 	= new BStringView("", B_TRANSLATE("Seeding Limits"));
	
	addingLabel->SetFont(be_bold_font);
	downloadingLabel->SetFont(be_bold_font);
	
	//
	//
	//
	//fAutoAddTorrentsFrom = new BCheckBox("auto_add_torrents_from",
	//	B_TRANSLATE("Automatically add torrents from:"), 
	//	new BMessage(MSG_ADD_TORRENT_FROM_FOLDER_CHANGED));
	//fAutoAddTorrentsFrom->SetValue(B_CONTROL_OFF);
	//
	//
	//fAutoAddTorrentsFromFolder = new FolderSelect("auto_add_torrents_from_folder",
	//	NULL, "/test/path"/*new BMessage(MSG_DOWNLOAD_FOLDER_CHANGED)*/);
	//
	//fAutoAddTorrentsFromFolder->SetEnabled(false);


	BCheckBox* fShowOptionsDialog = new BCheckBox("Show options dialog",
		B_TRANSLATE("Show options dialog"),
		NULL /*new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED)*/);
	fShowOptionsDialog->SetValue(B_CONTROL_OFF);
	
	BCheckBox* fStartWhenAdded = new BCheckBox("start_when_added",
		B_TRANSLATE("Start when added"),
		new BMessage(MSG_START_WHEN_ADDED_BEHAVIOR_CHANGED));
	fStartWhenAdded->SetValue(fTorrentPreferences->StartWhenAddedEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	
	fIncompleteFileNaming = new BCheckBox("incomplete_file_naming",
		B_TRANSLATE("Append \".part\" to incomplete files' names"), 
		new BMessage(MSG_INCOMPLETE_FILENAMING_BEHAVIOR_CHANGED));
	fIncompleteFileNaming->SetValue(fTorrentPreferences->IncompleteFileNamingEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	
	BStringView* fTorrentSaveLocation = new BStringView("", B_TRANSLATE("Save to Location:"));
	fTorrentSaveLocationPath = new FolderSelect("download_folder_select", 
		fTorrentPreferences->DownloadFolder(), new BMessage(MSG_DOWNLOAD_FOLDER_BEHAVIOR_CHANGED));
		
	fIncompleteDirEnabled = new BCheckBox("incomplete_torrent_folder",
		B_TRANSLATE("Incomplete torrents folder"),
		new BMessage(MSG_INCOMPLETE_FOLDER_BEHAVIOR_CHANGED));
	fIncompleteDirEnabled->SetValue(fTorrentPreferences->IncompleteFolderEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	
	fIncompleteDirPath = new FolderSelect("incomplete_torrent_folder_path", 
		fTorrentPreferences->IncompleteFolder(), new BMessage(MSG_INCOMPLETE_FOLDER_PATH_BEHAVIOR_CHANGED));
	fIncompleteDirPath->SetEnabled(fTorrentPreferences->IncompleteFolderEnabled());
	
	//
	//
	//
	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(addingLabel)
		//.Add(BGridLayoutBuilder(spacing / 2, spacing / 2)
		//	.Add(fAutoAddTorrentsFrom, 0, 0)
		//	.Add(fAutoAddTorrentsFromFolder, 1, 0)
		//)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing / 2, -1, -1, -1)
			.Add(fShowOptionsDialog, 0, 0)
			.Add(fStartWhenAdded, 0, 1)
		)
		.Add(downloadingLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing / 2, -1, -1, -1)
			.Add(fIncompleteFileNaming, 0, 0, 2, 1)
			.Add(fTorrentSaveLocation, 0, 1)
			.Add(fTorrentSaveLocationPath, 1, 1)
			.Add(fIncompleteDirEnabled, 0, 2)
			.Add(fIncompleteDirPath, 1, 2)
		)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Torrents");
	return view;

}


BView* PreferencesWindow::_CreateConnectionPage(float spacing)
{
/*
	BStringView* addingLabel 		= new BStringView("", B_TRANSLATE("Adding"));
	BStringView* downloadingLabel 	= new BStringView("", B_TRANSLATE("Downloading"));
	//BStringView* seedingLabel 	= new BStringView("", B_TRANSLATE("Seeding Limits"));
	
	addingLabel->SetFont(be_bold_font);
	downloadingLabel->SetFont(be_bold_font);
*/
	BStringView* peerPortLabel = new BStringView("", B_TRANSLATE("Peer Port"));
	BStringView* limitsLabel = new BStringView("", B_TRANSLATE("Limits"));
	BStringView* otherLabel = new BStringView("", B_TRANSLATE("Other"));
	peerPortLabel->SetFont(be_bold_font);
	limitsLabel->SetFont(be_bold_font);
	otherLabel->SetFont(be_bold_font);
	

	BStringView* fListeningPortLabel = new BStringView("", B_TRANSLATE("Incoming port:"));
	BStringView* fMaxConnectionLabel = new BStringView("", B_TRANSLATE("Max connections:"));
	BStringView* fTorrentMaxConnectionLabel	= new BStringView("", B_TRANSLATE("Connected peers limit:"));
	//BStringView* fTorrentUploadSlotsLabel = new BStringView("", B_TRANSLATE("Connected peers per torrent limit:"));

	
	
	fListeningPort = new BTextControl("_name", NULL, "", NULL);
	fRandomPort = new BButton("", B_TRANSLATE("Random"), new BMessage(MSG_INCOMING_PORT_RANDOM_BEHAVIOR_CHANGED));
	fApplyPort = new BButton("", B_TRANSLATE("Apply"), new BMessage(MSG_INCOMING_PORT_BEHAVIOR_CHANGED));
	fEnableForwardingPort = new BCheckBox("", B_TRANSLATE("Enable UPnP / NAT-PMP port forwarding"), new BMessage(MSG_PORT_FORWARDING_BEHAVIOR_CHANGED));	
	fMaxConnection = new BTextControl("_name", "", "", NULL);
	fApplyMaxConnection = new BButton("", B_TRANSLATE("Apply"), new BMessage(MSG_PEER_LIMIT_BEHAVIOR_CHANGED));
	fTorrentMaxConnection = new BTextControl("_name", "", "", NULL);
	fApplyTorrentMaxConnection = new BButton("", B_TRANSLATE("Apply"), new BMessage(MSG_PEER_LIMIT_PER_TORRENT_BEHAVIOR_CHANGED));
	//BTextControl* fTorrentUploadSlots = new BTextControl("_name", "", "", NULL);
	BCheckBox* fEnableDHTValue = new BCheckBox("", B_TRANSLATE("Enable Distributed Hash Table (DHT)"), new BMessage(MSG_DISTRIBUTED_HASH_TABLE_BEHAVIOR_CHANGED));
	BCheckBox* fEnablePEXValue = new BCheckBox("", B_TRANSLATE("Enable Bit Torrent Peer EXchange (PEX)"), new BMessage(MSG_TORRENT_PEER_EXCHANGE_BEHAVIOR_CHANGED));
	BCheckBox* fEnableUTPValue = new BCheckBox("", B_TRANSLATE("Enable Micro Transport Protocol (" UTF8_GREEK_MU_LETTER "TP)"), new BMessage(MSG_MICRO_TRANSPORT_PROTOCOL_BEHAVIOR_CHANGED));
	BCheckBox* fEnableLPDValue = new BCheckBox("", B_TRANSLATE("Enable Local Peer Discovery (LPD)"), new BMessage(MSG_LOCAL_PEER_DISCOVERY_BEHAVIOR_CHANGED));
	
	//
	BPopUpMenu* menu = new BPopUpMenu("");
	
	
	fEncryptionMenuItem[0] = new BMenuItem(B_TRANSLATE("Off"), _CreateEncryptionMenuMessage(0));
	fEncryptionMenuItem[1] = new BMenuItem(B_TRANSLATE("Enabled"), _CreateEncryptionMenuMessage(1));
	fEncryptionMenuItem[2] = new BMenuItem(B_TRANSLATE("Required"), _CreateEncryptionMenuMessage(2));
	
	menu->AddItem(fEncryptionMenuItem[0]);
	menu->AddItem(fEncryptionMenuItem[1]);
	menu->AddItem(fEncryptionMenuItem[2]);
	
	fEncryptionMenu = new BMenuField("", B_TRANSLATE("Encryption:"), menu);
	
	//
	BString textBuffer;	
	
	textBuffer << (int32)fTorrentPreferences->IncomingPort();
	fListeningPort->SetText(textBuffer);
	
	textBuffer = B_EMPTY_STRING;
	textBuffer << (int32)fTorrentPreferences->PeerLimit();
	fMaxConnection->SetText(textBuffer);
	
	textBuffer = B_EMPTY_STRING;
	textBuffer << (int32)fTorrentPreferences->PeerLimitPerTorrent();
	fTorrentMaxConnection->SetText(textBuffer);
	
	//textBuffer << (int32)fTorrentPreferences->IncomingPort();
	//fTorrentUploadSlots->SetText(textBuffer);
	

	fEnableForwardingPort->SetValue(fTorrentPreferences->PortForwardingEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	fEnableDHTValue->SetValue(fTorrentPreferences->DistributedHashTableEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	fEnablePEXValue->SetValue(fTorrentPreferences->PeerExchangeEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	fEnableUTPValue->SetValue(fTorrentPreferences->MicroTransportProtocolEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	fEnableLPDValue->SetValue(fTorrentPreferences->LocalPeerDiscoveryEnabled() ? B_CONTROL_ON : B_CONTROL_OFF);
	fEncryptionMenuItem[fTorrentPreferences->EncryptionMode()]->SetMarked(true);
	
	//
	fListeningPort->SetExplicitMaxSize(BSize(60, 40));
	fMaxConnection->SetExplicitMaxSize(BSize(60, 40));
	fTorrentMaxConnection->SetExplicitMaxSize(BSize(60, 40));
	//fTorrentUploadSlots->SetExplicitMaxSize(BSize(60, 40));
	
	//
	//
	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(peerPortLabel)
		.Add(BGridLayoutBuilder(-1, spacing / 2)
			.SetInsets(spacing / 2, -1, -1, -1)
			.Add(fListeningPortLabel, 0, 0)
			//.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing), 1, 0)
			.Add(fListeningPort, 1, 0)
			.Add(fRandomPort, 2, 0)
			.Add(fApplyPort, 3, 0)
			
			//.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			//.Add(BSpaceLayoutItem::CreateGlue(), 3, 0)
			//.Add(BSpaceLayoutItem::CreateGlue(), 4, 0)
			
			//
			.Add(fEnableForwardingPort, 0, 1, 3, 1)
			
			//
		)
		.Add(limitsLabel)
		.Add(BGridLayoutBuilder(spacing / 2, spacing / 2)
			.SetInsets(spacing / 2, -1, -1, -1)
			.Add(fMaxConnectionLabel, 0, 0)
			.Add(fMaxConnection, 1, 0)
			.Add(fApplyMaxConnection, 2, 0)
			// padding
			//.Add(BSpaceLayoutItem::CreateGlue(), 3, 0)
			//.Add(BSpaceLayoutItem::CreateGlue(), 4, 0)
			
			//
			.Add(fTorrentMaxConnectionLabel, 0, 1)
			.Add(fTorrentMaxConnection, 1, 1)
			.Add(fApplyTorrentMaxConnection, 2, 1)
			//
			//.Add(fTorrentUploadSlotsLabel, 0, 2)
			//.Add(fTorrentUploadSlots, 1, 2)
		)
		.Add(otherLabel)
		.Add(BGridLayoutBuilder(spacing / 2, spacing / 2)
			.SetInsets(spacing / 2, -1, -1, -1)
			.Add(fEnableDHTValue, 0, 0, 3, 1)
			.Add(fEnablePEXValue, 0, 1, 3, 1)
			.Add(fEnableUTPValue, 0, 2, 3, 1)
			.Add(fEnableLPDValue, 0, 3, 3, 1)
			
			
			//
			.Add(fEncryptionMenu->CreateLabelLayoutItem(), 0, 4)
			.Add(fEncryptionMenu->CreateMenuBarLayoutItem(), 1, 4)
		)
		.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Connection");
	return view;
}

BMessage* PreferencesWindow::_CreateEncryptionMenuMessage(int32 index)
{
	BMessage* message = new BMessage(MSG_ENCRYPTION_BEHAVIOR_CHANGED);
	message->AddInt32("menuIndex", index);	
	
	return message;
}


#pragma mark - Message handle
void PreferencesWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case MSG_START_WHEN_ADDED_BEHAVIOR_CHANGED:
			fTorrentPreferences->StartWhenAddedToggle();
			break;
		case MSG_INCOMPLETE_FILENAMING_BEHAVIOR_CHANGED:
			fTorrentPreferences->IncompleteFileNamingToggle();
			break;
		case MSG_DOWNLOAD_FOLDER_BEHAVIOR_CHANGED:
			_DownloadFolderChanged(message);
			break;
		case MSG_INCOMPLETE_FOLDER_BEHAVIOR_CHANGED:
			fTorrentPreferences->IncompleteFolderToggle();
			fIncompleteDirPath->SetEnabled(fTorrentPreferences->IncompleteFolderEnabled());
			break;
		case MSG_INCOMPLETE_FOLDER_PATH_BEHAVIOR_CHANGED:
			_IncompleteFolderChanged(message);
			break;
		case MSG_INCOMING_PORT_BEHAVIOR_CHANGED:
			_IncomingPortChanged(message);
			break;
		case MSG_INCOMING_PORT_RANDOM_BEHAVIOR_CHANGED:
			_IncomingPortRandomChanged(message);
			break;
		case MSG_PORT_FORWARDING_BEHAVIOR_CHANGED:
			_PortForwardingChanged(message);
			break;
		case MSG_PEER_LIMIT_BEHAVIOR_CHANGED:
			_PeerLimitChanged(message);
			break;
		case MSG_PEER_LIMIT_PER_TORRENT_BEHAVIOR_CHANGED:
			_PeerLimitPerTorrentChanged(message);
			break;
		case MSG_DISTRIBUTED_HASH_TABLE_BEHAVIOR_CHANGED:
			_DistributedHashTableChanged(message);
			break;
		case MSG_TORRENT_PEER_EXCHANGE_BEHAVIOR_CHANGED:
			_TorrentPeerExchangeChanged(message);
			break;
		case MSG_MICRO_TRANSPORT_PROTOCOL_BEHAVIOR_CHANGED:
			_MicroTransportProtocolChanged(message);
			break;
		case MSG_LOCAL_PEER_DISCOVERY_BEHAVIOR_CHANGED:
			_LocalPeerDiscoveryChanged(message);
			break;
		case MSG_ENCRYPTION_BEHAVIOR_CHANGED:
			_EncryptionModeChanged(message);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}


void PreferencesWindow::_DownloadFolderChanged(BMessage* message)
{
	entry_ref ref;
	
	if (message->FindRef("ref", &ref) == B_OK) {
		BPath dir(&ref);
		
		//
		//
		//
		fTorrentPreferences->SetDownloadFolder(dir.Path());
	}
}

void PreferencesWindow::_IncompleteFolderChanged(BMessage* message)
{
	entry_ref ref;
	
	if (message->FindRef("ref", &ref) == B_OK) {
		BPath dir(&ref);
		
		//
		//
		//
		fTorrentPreferences->SetIncompleteFolder(dir.Path());
	}
}

void PreferencesWindow::_IncomingPortChanged(BMessage* message)
{
	uint16 value = static_cast<uint16>(atoi(fListeningPort->Text()));
	fTorrentPreferences->SetIncomingPort(value);
}

void PreferencesWindow::_IncomingPortRandomChanged(BMessage* message)
{
	BString textBuffer;
	uint16 value = fTorrentPreferences->SetIncomingRandomPort();
	textBuffer << value;
	fListeningPort->SetText(textBuffer);
	
}

void PreferencesWindow::_PortForwardingChanged(BMessage* message)
{
	fTorrentPreferences->PortForwardingToggle();
}

void PreferencesWindow::_PeerLimitChanged(BMessage* message)
{
	uint16 value = static_cast<uint16>(atoi(fMaxConnection->Text()));
	fTorrentPreferences->SetPeerLimit(value);
}

void PreferencesWindow::_PeerLimitPerTorrentChanged(BMessage* message)
{
	uint16 value = static_cast<uint16>(atoi(fTorrentMaxConnection->Text()));
	fTorrentPreferences->SetPeerLimitPerTorrent(value);
}

void PreferencesWindow::_DistributedHashTableChanged(BMessage* message)
{
	fTorrentPreferences->DistributedHashTableToggle();
}

void PreferencesWindow::_TorrentPeerExchangeChanged(BMessage* message)
{
	fTorrentPreferences->PeerExchangeToggle();	
}

void PreferencesWindow::_MicroTransportProtocolChanged(BMessage* message)
{
	fTorrentPreferences->MicroTransportProtocolToggle();
}

void PreferencesWindow::_LocalPeerDiscoveryChanged(BMessage* message)
{
	fTorrentPreferences->LocalPeerDiscoveryToggle();
}

void PreferencesWindow::_EncryptionModeChanged(BMessage* message)
{
	int32 selectedIndex;
	
	if (message->FindInt32("menuIndex", &selectedIndex) == B_OK)
		fTorrentPreferences->SetEncryptionMode(static_cast<tr_encryption_mode>(selectedIndex));
}

bool PreferencesWindow::QuitRequested()
{
	if( !IsHidden() )
		Hide();
		
	//
	//
	//
	fTorrentPreferences->Save();
	
	return false;
}


