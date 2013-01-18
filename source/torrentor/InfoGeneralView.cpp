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
//	File Name:		InfoGeneralView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <libtransmission/transmission.h>
#include <libtransmission/utils.h>

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

#include "InfoGeneralView.h"

InfoGeneralView::InfoGeneralView(const TorrentObject* torrent)
	:	BGroupView(B_HORIZONTAL, 1),
		fTorrent(torrent),
		fFirstUpdate(true)
{
	SetFlags(Flags() | B_PULSE_NEEDED);
	
	const float spacing = be_control_look->DefaultItemSpacing();
	const BAlignment leftAlignment(B_ALIGN_LEFT, B_ALIGN_TOP);
	const BAlignment rightAlignment(B_ALIGN_RIGHT, B_ALIGN_TOP);
	
	//
	// Build Labels
	//
	BStringView* infoLabel 	= new BStringView("", "Torrent Information");
	BStringView* createdLabel 	= new BStringView("", "Torrent Information");
	BStringView* locationLabel 	= new BStringView("", "Location");
	
	infoLabel->SetFont(be_bold_font);
	createdLabel->SetFont(be_bold_font);
	locationLabel->SetFont(be_bold_font);

	BStringView* piecesLabel 	= new BStringView("", "Pieces:");
	BStringView* hashLabel 		= new BStringView("", "Hash:");
	BStringView* secureLabel	= new BStringView("", "Secure:");
	BStringView* commentLabel	= new BStringView("", "Comment:");
	BStringView* creatorLabel	= new BStringView("", "Creator:");
	BStringView* dateLabel		= new BStringView("", "Date:");
	BStringView* dataLabel		= new BStringView("", "Data file:");
	BStringView* torrentLabel	= new BStringView("", "Torrent file:");

	//
	//
	//
	fPiecesView 	= new BStringView("", "");
	fHashView 		= new BStringView("", "");
	fSecureView		= new BStringView("", "");
	fCommentView	= new BTextView("");
	fCreatorView 	= new BStringView("", "");
	fDateView 		= new BStringView("", "");
	fDataView 		= new BStringView("", "");
	fTorrentView 	= new BStringView("", "");

	//
	// align all
	//
	piecesLabel->SetExplicitAlignment(rightAlignment);
	hashLabel->SetExplicitAlignment(rightAlignment);
	secureLabel->SetExplicitAlignment(rightAlignment);
	commentLabel->SetExplicitAlignment(rightAlignment);
	creatorLabel->SetExplicitAlignment(rightAlignment);
	dateLabel->SetExplicitAlignment(rightAlignment);
	dataLabel->SetExplicitAlignment(rightAlignment);
	torrentLabel->SetExplicitAlignment(rightAlignment);


	fPiecesView->SetExplicitAlignment(leftAlignment);
	fHashView->SetExplicitAlignment(leftAlignment);
	fSecureView->SetExplicitAlignment(leftAlignment);
	fCommentView->SetExplicitAlignment(leftAlignment);
	fCreatorView->SetExplicitAlignment(leftAlignment);
	fDateView->SetExplicitAlignment(leftAlignment);
	fDataView->SetExplicitAlignment(leftAlignment);
	fTorrentView->SetExplicitAlignment(leftAlignment);

	
	fCommentView->MakeEditable(false);
	fCommentView->SetExplicitMinSize(BSize(400, 70));
	
	GroupLayout()->AddView(BGroupLayoutBuilder(B_VERTICAL, 0)
		.Add(infoLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(piecesLabel, 0, 0)
			.Add(fPiecesView, 1, 0)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			.Add(hashLabel, 0, 1)
			.Add(fHashView, 1, 1)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 1)
			.Add(secureLabel, 0, 2)
			.Add(fSecureView, 1, 2)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 2)
			.Add(commentLabel, 0, 3)
			.Add(fCommentView, 1, 3)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 3)
		)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(createdLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(creatorLabel, 0, 0)
			.Add(fCreatorView, 1, 0)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			.Add(dateLabel, 0, 1)
			.Add(fDateView, 1, 1)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 1)
		)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.Add(locationLabel)
		.Add(BGridLayoutBuilder(spacing, spacing / 2)
			.SetInsets(spacing, spacing, 0, spacing)
			.Add(dataLabel, 0, 0)
			.Add(fDataView, 1, 0)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 0)
			.Add(torrentLabel, 0, 1)
			.Add(fTorrentView, 1, 1)
			.Add(BSpaceLayoutItem::CreateGlue(), 2, 1)
		)
		.TopView()
	);
}


void InfoGeneralView::Pulse()
{
	UpdateInfo();
}
	
void InfoGeneralView::UpdateInfo()
{
	BString formatBuffer;
	char tempBuffer[1024] = {0};

	
	//
	//
	//
	if( fTorrent->IsMagnet() || !fFirstUpdate )
		return;
	
	//
	//
	//
	formatBuffer << fTorrent->Info()->pieceCount 
				 << ", " 
				 << tr_formatter_mem_B(tempBuffer, fTorrent->Info()->pieceSize, sizeof(tempBuffer));
	
	
	fPiecesView->SetText( formatBuffer );
	fHashView->SetText( fTorrent->Info()->hashString );
	fSecureView->SetText( fTorrent->Info()->isPrivate ? "Private Torrent" : "Public Torrent" );
	fCommentView->SetText( fTorrent->Info()->comment );
	
	fCreatorView->SetText( strlen(fTorrent->Info()->creator) > 0 ? fTorrent->Info()->creator : "N/A" ); // TODO: N/A if empty.
	
	strftime(tempBuffer, sizeof(tempBuffer), "%A, %B %d, %Y %X", localtime(&fTorrent->Info()->dateCreated));

	
	fDateView->SetText( fTorrent->Info()->dateCreated ? tempBuffer : "N/A" );
	fDataView->SetText( fTorrent->DownloadFolder() );
	
	
	BString out_string(fTorrent->Info()->torrent);

	TruncateString(&out_string, B_TRUNCATE_END, 400);

	
	fTorrentView->SetText( out_string );
	fTorrentView->SetToolTip( fTorrent->Info()->torrent );
	
	fFirstUpdate = false;
}
