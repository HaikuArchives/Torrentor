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

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PreferencesWindow"

static const BRect kDefaultFrame = BRect(120, 80, 800, 600);

enum
{
	MSG_APPLY	= 'appl',
	MSG_START_WHEN_ADDED_BEHAVIOR_CHANGED			= 'MSWA',
	MSG_INCOMPLETE_FOLDER_BEHAVIOR_CHANGED			= 'MINF',
	MSG_INCOMPLETE_FOLDER_PATH_BEHAVIOR_CHANGED		= 'MIFP',
	MSG_INCOMPLETE_FILENAMING_BEHAVIOR_CHANGED 		= 'MIFC',
	MSG_DOWNLOAD_FOLDER_BEHAVIOR_CHANGED			= 'MDFC',
};

PreferencesWindow::PreferencesWindow()
	:	BWindow(kDefaultFrame, "Settings", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS | 
				B_ASYNCHRONOUS_CONTROLS | 
				B_NOT_ZOOMABLE)

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
			.SetInsets(spacing, -1, -1, -1)
			.Add(fShowOptionsDialog, 0, 0)
			.Add(fStartWhenAdded, 0, 1)
		)
		.Add(downloadingLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, -1, -1, -1)
			.Add(fIncompleteFileNaming, 0, 0)
			.Add(fTorrentSaveLocation, 0, 1)
			.Add(fTorrentSaveLocationPath, 1, 1)
			.Add(fIncompleteDirEnabled, 0, 2)
			.Add(fIncompleteDirPath, 1, 2)
		)
		//.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Torrents");
	return view;

}


#include <cstdio>
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
			OnDownloadFolderChanged(message);
			break;
		case MSG_INCOMPLETE_FOLDER_BEHAVIOR_CHANGED:
			fTorrentPreferences->IncompleteFolderToggle();
			fIncompleteDirPath->SetEnabled(fTorrentPreferences->IncompleteFolderEnabled());
			break;
		case MSG_INCOMPLETE_FOLDER_PATH_BEHAVIOR_CHANGED:
			OnIncompleteFolderChanged(message);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

void PreferencesWindow::OnDownloadFolderChanged(BMessage* message)
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

void PreferencesWindow::OnIncompleteFolderChanged(BMessage* message)
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

bool PreferencesWindow::QuitRequested()
{
	if( !IsHidden() )
		Hide();
	
	return false;
}


