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
//	File Name:		InfoTransferView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <cstdio>
#include <libtransmission/transmission.h>

#include <ControlLook.h>
#include <GroupView.h>
#include <GridLayoutBuilder.h>
#include <GroupLayoutBuilder.h>
#include <SpaceLayoutItem.h>
#include <SeparatorView.h>
#include <StringView.h>
#include <String.h>
#include <TextView.h>

#include "TorrentObject.h"
#include "PiecesView.h"
#include "InfoTransferView.h"
#include "FormatText.h"

InfoTransferView::InfoTransferView(const TorrentObject* torrent)
	:	BGroupView(B_HORIZONTAL, 1),
		fTorrent(torrent)
{
	SetFlags(Flags() | B_PULSE_NEEDED);
	
	const float spacing = be_control_look->DefaultItemSpacing();
	
	fPiecesView = new PiecesView(fTorrent);
	
	
	fPiecesView->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));

	
	BStringView* transferLabel 	= new BStringView("", "Transfer");
	BStringView* datesLabel 	= new BStringView("", "Dates");
	BStringView* elapsedLabel 	= new BStringView("", "Time Elapsed");
	transferLabel->SetFont(be_bold_font);
	datesLabel->SetFont(be_bold_font);
	elapsedLabel->SetFont(be_bold_font);
	
	
	BStringView* stateLabel 	= new BStringView("", "State:");	
	BStringView* progressLabel 	= new BStringView("", "Progress:");	
	BStringView* haveLabel 		= new BStringView("", "Have:");
	BStringView* downloadedLabel= new BStringView("", "Downloaded:");
	BStringView* uploadedLabel 	= new BStringView("", "Uploaded:");
	BStringView* failedLabel 	= new BStringView("", "Failed:");
	BStringView* ratioLabel 	= new BStringView("", "Ratio:");
	BStringView* errorLabel 	= new BStringView("", "Error:");
	BStringView* addedLabel 	= new BStringView("", "Added:");
	BStringView* completedLabel	= new BStringView("", "Completed:");
	BStringView* lastActivityLabel= new BStringView("", "Last Activity:");
	BStringView* downloadTimeLabel= new BStringView("", "Downloading:");
	BStringView* seedTimeLabel	= new BStringView("", "Seeding:");
	
	
	stateLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	progressLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	haveLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	downloadedLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	uploadedLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	failedLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	ratioLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	errorLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	addedLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	completedLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	lastActivityLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	downloadTimeLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	seedTimeLabel->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP));
	
	fStateView 		= new BStringView("", "");
	fProgressView 	= new BStringView("", "57.15% (57.15% selected)");
	fHaveView 		= new BStringView("", "695.7 MB (680.3 MB verified)");
	fDownloadedView	= new BStringView("", "701.1 MB");
	fUploadedView	= new BStringView("", "2.8 KB");
	fFailedView		= new BStringView("", "0 KB");
	fRatioView		= new BStringView("", "0.00");
	
	char FormatBuffer[256];
	
	strftime(FormatBuffer, sizeof(FormatBuffer), "%A, %B %d, %Y %X", localtime(&fTorrent->Statistics()->addedDate));
	
	fAddedView 		= new BStringView("", FormatBuffer);
	fCompletedView 	= new BStringView("", "N/A");
	fLastActivityView = new BStringView("", "N/A");
	fDownloadTimeView = new BStringView("", "20 minutes, 35 seconds.");
	fSeedTimeView 	= new BStringView("", "0 seconds.");
	
		
	
	
	
	fStateView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fProgressView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fHaveView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fDownloadedView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fUploadedView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fFailedView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fRatioView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fDownloadTimeView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	fSeedTimeView->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	
	fErrorText = new BTextView("");
	
	fErrorText->MakeEditable(false);
	//fErrorText->MakeSelectable(false);
	
	fErrorText->SetExplicitMinSize(BSize(400, 70));
	fErrorText->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_TOP));
	
	GroupLayout()->AddView(BGroupLayoutBuilder(B_VERTICAL, 0)
		.Add(transferLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(stateLabel, 0, 0)
			.Add(fStateView, 1, 0)
			.Add(fPiecesView, 2, 0, 1, 7)
			.Add(progressLabel, 0, 1)
			.Add(fProgressView, 1, 1)
			.Add(haveLabel, 0, 2)
			.Add(fHaveView, 1, 2)
			.Add(downloadedLabel, 0, 3)
			.Add(fDownloadedView, 1, 3)
			.Add(uploadedLabel, 0, 4)
			.Add(fUploadedView, 1, 4)
			.Add(failedLabel, 0, 5)
			.Add(fFailedView, 1, 5)
			.Add(ratioLabel, 0, 6)
			.Add(fRatioView, 1, 6)
			.Add(errorLabel, 0, 7)
			.Add(fErrorText, 1, 7, 2, 1)
		)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(datesLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(addedLabel, 0, 0)
			.Add(fAddedView, 1, 0)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			.Add(completedLabel, 0, 1)
			.Add(fCompletedView, 1, 1)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 1)
			.Add(lastActivityLabel, 0, 2)
			.Add(fLastActivityView, 1, 2)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 2)
		)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(elapsedLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(downloadTimeLabel, 0, 0)
			.Add(fDownloadTimeView, 1, 0)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			.Add(seedTimeLabel, 0, 1)
			.Add(fSeedTimeView, 1, 1)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 1)
		)
		.TopView()
	);

}

	
void InfoTransferView::Pulse()
{
	BString TextBuffer = B_EMPTY_STRING;
		
	//
	// If the current active tab it's the InfoTransferView
	// update the graph.
	//
	fPiecesView->Invalidate();
	
	//
	//
	//
	FormatPercentText(TextBuffer, fTorrent->Statistics()->percentComplete  * 100.f);
	
	//
	// If the user don't want to download the entire torrent
	//
	if( fTorrent->Statistics()->sizeWhenDone < fTorrent->Info()->totalSize )
	{
		// (57.15% selected)
		TextBuffer << " (";
		FormatPercentText(TextBuffer, fTorrent->Statistics()->percentDone * 100.f, false);	
		TextBuffer << " selected)";
	}
	fProgressView->SetText( TextBuffer );
	
	FormatStateString(TextBuffer, fTorrent);
	fStateView->SetText( TextBuffer );
	
	FormatRatioText(TextBuffer, fTorrent->Statistics()->ratio);
	fRatioView->SetText( TextBuffer );
	
	
	TextBuffer = fTorrent->ErrorMessage();
	
	if( fErrorText->Text() != TextBuffer )
		fErrorText->SetText( TextBuffer );
	
	//
	//
	//
	uint64_t haveTotal = fTorrent->Statistics()->haveValid + 
						 fTorrent->Statistics()->haveUnchecked;
	uint64_t haveValid = fTorrent->Statistics()->haveValid;
	
	//
	//
	//
	if( haveTotal == haveValid )
	{
		FormatSizeText(TextBuffer, haveValid);
		TextBuffer << " verified";
	}
	else
	{
		FormatSizeText(TextBuffer, haveTotal);
		
		TextBuffer << " ("; 
		FormatSizeText(TextBuffer, haveValid, false); 
		TextBuffer << ") verified";
	}
	
	fHaveView->SetText(TextBuffer);
	
	FormatTimeText(TextBuffer, fTorrent->SecondsDownloading());
	fDownloadTimeView->SetText( TextBuffer );
	
	FormatTimeText(TextBuffer, fTorrent->SecondsSeeding());
	fSeedTimeView->SetText( TextBuffer );
	
	FormatSizeText(TextBuffer, fTorrent->Statistics()->downloadedEver );
	fDownloadedView->SetText( TextBuffer );
	
	FormatSizeText(TextBuffer, fTorrent->Statistics()->uploadedEver );
	fUploadedView->SetText( TextBuffer );

	FormatSizeText(TextBuffer, fTorrent->Statistics()->corruptEver );
	fFailedView->SetText( TextBuffer );

	
/*
	fProgressView 	= new BStringView("", "57.15% (57.15% selected)");
	fHaveView 		= new BStringView("", "695.7 MB (680.3 MB verified)");
	fDownloadedView	= new BStringView("", "701.1 MB");
	fUploadedView	= new BStringView("", "2.8 KB");
	fFailedView		= new BStringView("", "0 KB");
	fRatioView		= new BStringView("", "0.00");
	*/
}


