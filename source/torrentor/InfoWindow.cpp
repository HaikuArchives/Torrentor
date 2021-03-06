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
//	File Name:		InfoWindow.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <Button.h>
#include <ControlLook.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <ListView.h>
#include <ScrollView.h>
#include <SpaceLayoutItem.h>
#include <String.h>
#include <StringView.h>
#include <TabView.h>
#include <Window.h>

#include <libtransmission/transmission.h>



#include "InfoFileView.h"
#include "InfoGeneralView.h"
#include "InfoHeaderView.h"
#include "InfoPeerView.h"
#include "InfoTrackerView.h"
#include "InfoTransferView.h"
#include "InfoWindow.h"

#include "TorrentObject.h"

#include <stdio.h>



static const BRect kDefaultFrame = BRect(120, 80, 800, 600);


InfoWindow::InfoWindow(TorrentObject* torrent)
	:	BWindow(kDefaultFrame, "Torrent Info", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS |
				B_ASYNCHRONOUS_CONTROLS |
				B_NOT_ZOOMABLE |
				B_NOT_RESIZABLE |
				B_PULSE_NEEDED ),
		fTorrent(torrent)
{
	SetPulseRate(1000000);
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	float spacing = be_control_look->DefaultItemSpacing();

	fTabView = new BTabView("InfoPages", B_WIDTH_FROM_LABEL);
	
	//
	//
	//
	fTabView->AddTab(_CreateInfoPage(spacing));
	fTabView->AddTab(_CreateTransferPage(spacing));
	fTabView->AddTab(_CreateTrackerPage(spacing));
	
	fTabView->AddTab(_CreatePeerPage(spacing));
	fTabView->AddTab(_CreateFilesPage(spacing));
		
	
	//
	//
	//
	AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.Add(new InfoHeaderView(fTorrent))
		.Add(fTabView)
		.SetInsets(spacing, spacing, spacing, spacing)
	);
	
	CenterOnScreen();
	Run();
}


BView* InfoWindow::_CreateInfoPage(float spacing)
{
	
	InfoGeneralView* fGeneralView = new InfoGeneralView(fTorrent);
	fGeneralView->UpdateInfo();
	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(fGeneralView)
		.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("General");
	return view;
}

BView* InfoWindow::_CreateTransferPage(float spacing)
{
	fTransferView = new InfoTransferView(fTorrent);

	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(fTransferView)
		.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Transfer");
	return view;
}

#include <LayoutUtils.h>
class FixedListView : public BListView
{
public:
	FixedListView(const char* name) 
		: 	BListView(name, B_MULTIPLE_SELECTION_LIST) 
	{}

	BSize MinSize()
	{
		float w, h;
		GetPreferredSize(&w, &h);
		return BSize(w, h);
	}

};


BView* InfoWindow::_CreateTrackerPage(float spacing)
{	
	InfoTrackerView* fTrackerView = new InfoTrackerView(fTorrent);


    BButton* fAddButton = new BButton("Add", NULL);

	
	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(fTrackerView)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
			.SetInsets(spacing, spacing, spacing, spacing)
			.Add(fAddButton)
			.AddGlue()
			//.Add(fCancelButton)
			//.Add(fApplyButton)
		)

		//.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Trackers");

	return view;

}

BView* InfoWindow::_CreatePeerPage(float spacing)
{
	
	InfoPeerView* fInfoPeerView = new InfoPeerView(fTorrent);

	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(fInfoPeerView)
		//.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Peers");
	return view;
}


BView* InfoWindow::_CreateFilesPage(float spacing)
{
	
	InfoFileView* fInfoFileView = new InfoFileView(fTorrent);

	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(fInfoFileView)
		//.SetInsets(spacing, spacing, spacing, spacing)
		.TopView()
	;
	view->SetName("Files");
	return view;
}

//BView* (float spacing);
//BView* _CreateOptionPage(float spacing);
