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
#include <cstring>
#include <cstdio>

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <CheckBoxColumn.h>
#include <ColumnListView.h>
#include <ColumnTypes.h>
#include <ControlLook.h>
#include <FileColumn.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <StatusBar.h>
#include <StringView.h>
#include <Window.h>



#include <libtransmission/transmission.h>
#include <libtransmission/utils.h>




#include "IconUtil.h"
#include "InfoHeaderView.h"
#include "AddTorrentWindow.h"

#include "TorrentorMessages.h"
#include "TorrentObject.h"

enum
{
	MSG_BUTTON_CANCEL	= 100,
	MSG_BUTTON_ADD,
};

enum
{
	COLUMN_FILE_NAME	= 0,
	COLUMN_FILE_DOWNLOAD,
};

AddTorrentWindow::AddTorrentWindow(TorrentObject* torrent)
	:	BWindow(BRect(),
				"Add torrent", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS |
				B_ASYNCHRONOUS_CONTROLS |
				B_NOT_ZOOMABLE |
				B_NOT_RESIZABLE |
				B_PULSE_NEEDED ),
		fTorrent(torrent),
		fCancelAdd(true)
{
	SetPulseRate(1000000);
	SetLayout(new BGroupLayout(B_VERTICAL));	
	
	float spacing = be_control_look->DefaultItemSpacing();
	
	//
	//
	//
	fInfoHeaderView = new InfoHeaderView(fTorrent);
	fFileList		= new BColumnListView("FileList", 0, B_PLAIN_BORDER, true);
	fStartCheckBox 	= new BCheckBox("", "Start when added", NULL);
	fCancelButton	= new BButton("Cancel", new BMessage(B_QUIT_REQUESTED));
	fAddButton		= new BButton("Add", new BMessage(MSG_BUTTON_ADD));
	fLoadingView	= new BStatusBar("", "Downloading Metadata");
	fLoadingView->SetBarHeight(12);
	fLoadingView->SetMaxValue(1.0);
	
	if( !fTorrent->IsMagnet() )
		fLoadingView->Hide();
	
	fStartCheckBox->SetValue(B_CONTROL_ON);
	
	//
	//
	//
	fFileList->SetColumnFlags(B_ALLOW_COLUMN_RESIZE);
	fFileList->SetSortingEnabled(false);
	fFileList->SetExplicitMinSize(BSize(550, FILE_COLUMN_HEIGHT * 5));
	
	fFileList->AddColumn(new FileColumn("Name", 400, 400, 500), COLUMN_FILE_NAME);
	fFileList->AddColumn(new CheckBoxColumn("DL", 40, 40, 40), COLUMN_FILE_DOWNLOAD);
	
	//
	// We're a magnet or a complete torrent file?
	//
	if( fTorrent->IsMagnet() )
	{
		fTorrent->SetMetadataCallbackHandler(this);
		//const_cast<TorrentObject*>(fTorrent)->StartTransfer();
	}
	else
		UpdateFileList();

	
	AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.Add(fInfoHeaderView)
		.AddGlue()
		.Add(fFileList)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
			.SetInsets(spacing, spacing, spacing, spacing)
			.Add(fStartCheckBox)
			.AddGlue()
			.Add(fCancelButton)
			.Add(fAddButton)
			.Add(fLoadingView)
		)
		.SetInsets(spacing, spacing, spacing, spacing)
	);
	
	CenterOnScreen();
	Run();
}

void AddTorrentWindow::MessageReceived(BMessage* message)
{
	switch( message->what ) 
	{
	case B_PULSE:
		BWindow::MessageReceived(message);
		break;
	case MSG_BUTTON_ADD:
		OnButtonAddClick();
		break;
	case MSG_TRANSMISSION_METADATA_CALLBACK:
		OnMetadataComplete();
		break;
	default:
		BWindow::MessageReceived(message);
		break;
	}
}



void AddTorrentWindow::DispatchMessage(BMessage* msg, BHandler* target)
{
	if (!msg)
		return;
	
	//
	// this doesn't work.
	//
	if (msg->what == B_PULSE && fTorrent->IsMagnet()) 
		fLoadingView->SetTo(fTorrent->Statistics()->metadataPercentComplete);
		
	//
	//
	//
	BWindow::DispatchMessage(msg, target);
}

void AddTorrentWindow::UpdateFileList()
{	
	//
	//
	int 	 fileCount = fTorrent->Info()->fileCount;
	tr_file* fileList  = fTorrent->Info()->files;
	
	for( int i = 0; i < fileCount; i++ )
	{
		char FormatBuffer[128] = { 0 };
		BRow* row = new BRow(FILE_COLUMN_HEIGHT);
		
		const char* name = fTorrent->IsFolder() ? (strchr(fileList[i].name, '/') + 1) : fileList[i].name;
		
		//
		//
		//
		BString FileExtension = B_EMPTY_STRING;
		BString FilePath = fileList[i].name;
		FilePath.CopyInto(FileExtension, FilePath.FindLast('.') + 1, FilePath.CountChars());
		
		
		const char* info = tr_formatter_mem_B(FormatBuffer, fileList[i].length, sizeof(FormatBuffer));;
		const BBitmap* icon = GetIconFromExtension(FileExtension);
		
		row->SetField(new FileField(icon, name, info), COLUMN_FILE_NAME);
		row->SetField(new CheckBoxField(true), COLUMN_FILE_DOWNLOAD);
		////row->SetField(new BIntegerField(PeerStatus[i].progress * 100.0), COLUMN_PEER_PROGRESS);
		
		fFileList->AddRow(row, i);

	} 	
}

void AddTorrentWindow::OnMetadataComplete()
{
	//
	// Pause the torrent to prevent files to be downloaded.
	// See [TorrentObject::LoadFromMagnet] for more info.
	//
	fTorrent->StopTransfer();
	
	//
	//
	//
	fInfoHeaderView->Update();
	
	//
	//
	//
	UpdateFileList();
}

void AddTorrentWindow::OnButtonAddClick()
{
	tr_file_index_t* files = new tr_file_index_t[ fTorrent->Info()->fileCount ];
	uint32 fileCount = 0;
	
	//
	// Just for test.
	//
	int32 RowCount = fFileList->CountRows();
	
	for( int32 i = 0; i < RowCount; i++ )
	{
		BRow* row = fFileList->RowAt( i );

		CheckBoxField* checkField = static_cast<CheckBoxField*>(row->GetField(COLUMN_FILE_DOWNLOAD));
		
		if( !checkField->Selected() )
			files[fileCount++] = i;
	}
	tr_torrentSetFileDLs(const_cast<tr_torrent*>(fTorrent->Handle()), files, fileCount, false);
	
	//
	//
	//
	BMessage* message = new BMessage(MSG_OPEN_TORRENT_RESULT);
	
	message->AddBool("add", true);
	message->AddBool("start", fStartCheckBox->Value() == B_CONTROL_ON);
	message->AddPointer("torrent", fTorrent);
	
	be_app->PostMessage(message);
	
	delete[] files;
	
	//
	// 
	//
	fCancelAdd = false;
	Quit();
}

bool AddTorrentWindow::QuitRequested()
{
	if( fCancelAdd )
	{
		BMessage* message = new BMessage(MSG_OPEN_TORRENT_RESULT);
	
		message->AddBool("add", false);
		message->AddPointer("torrent", fTorrent);
	
		be_app->PostMessage(message);
	}
	return true;
}

