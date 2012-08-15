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
//	File Name:		InfoHeaderView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <Bitmap.h>
#include <GroupView.h>
#include <GroupLayoutBuilder.h>
#include <String.h>
#include <StringView.h>
#include <MimeType.h>

#include <libtransmission/transmission.h>
#include <libtransmission/utils.h>
#include "TorrentObject.h"
#include "IconView.h"
#include "InfoHeaderView.h"

static const char* B_DIRECTORY_MIME_TYPE = "application/x-vnd.Be-directory";

InfoHeaderView::InfoHeaderView(const TorrentObject* fTorrent)
	:	BGroupView(B_HORIZONTAL, 0)
{
	SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);
	
	char FormatBuffer[1024] = {0};
	BMimeType mime(B_DIRECTORY_MIME_TYPE);
	
	//
	// If the torrent is not a folder, search the mime.
	//	
	if( !fTorrent->IsFolder() && !fTorrent->IsMagnet() )
		BMimeType::GuessMimeType(fTorrent->Info()->files[0].name, &mime);
		
	//
	//
	//
	BString infoText;
	infoText.SetToFormat("%d files, %s", fTorrent->Info()->fileCount,
		tr_formatter_mem_B(FormatBuffer, fTorrent->Statistics()->sizeWhenDone, sizeof(FormatBuffer)));

	//
	//
	//
	fIconView 		= new IconView(mime.Type());
	fNameView 		= new BStringView("NameView", fTorrent->Name());
	fInfoView 		= new BStringView("InfoView", infoText);
	
	
	//GroupLayout()->SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING);
	GroupLayout()->AddView(fIconView);

	//
	BView* verticalGroup = BGroupLayoutBuilder(B_VERTICAL, 0)
		.SetInsets(8, 0, 0, 0)
		.Add(fNameView)
		.Add(fInfoView)
		.TopView()
	;
	
	GroupLayout()->AddView(verticalGroup);
	
	//
	//
	//
	BFont font;
	fNameView->GetFont(&font);
	font.SetFace(B_BOLD_FACE);	
	fNameView->SetFont(&font, B_FONT_FACE);

	
	fNameView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	fInfoView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

}
