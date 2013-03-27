//------------------------------------------------------------------------------
//	Copyright (c) 2010, Stephan Aßmus <superstippi@gmx.de>.
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
//	File Name:		DownloadItem.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include "Torrentor.h"
#include "TorrentorMessages.h"
#include <String.h>

#include "IconView.h"
#include "DownloadView.h"
#include "DownloadItem.h"

#include "SymbolButton.h"

#include <MessageFilter.h>
#include <Shape.h>

#include <ControlLook.h>
#include <GradientLinear.h>


#include "FormatText.h"


#define UTF8_UP_ARROW_CHARACTER "\xE2\x86\x91"
#define UTF8_DOWN_ARROW_CHARACTER "\xE2\x86\x93"


const uint32 MSG_CONTROL_BUTTON = 0x249141;






BShape* _CreatePlayShape(float height)
{
	BShape* shape = new BShape();

	float step = floorf(height / 8);

	shape->MoveTo(BPoint(height + step, height / 2));
	shape->LineTo(BPoint(-step, height + step));
	shape->LineTo(BPoint(-step, 0 - step));
	shape->Close();

	return shape;
}

BShape* _CreatePauseShape(float height)
{
	BShape* shape = new BShape();

	float stemWidth = floorf(height / 3);

	shape->MoveTo(BPoint(0, height));
	shape->LineTo(BPoint(stemWidth, height));
	shape->LineTo(BPoint(stemWidth, 0));
	shape->LineTo(BPoint(0, 0));
	shape->Close();

	shape->MoveTo(BPoint(height - stemWidth, height));
	shape->LineTo(BPoint(height, height));
	shape->LineTo(BPoint(height, 0));
	shape->LineTo(BPoint(height - stemWidth, 0));
	shape->Close();

	return shape;
}

class ControlButton : public SymbolButton
{
public:
	ControlButton(bool TorrentActive)
		:	SymbolButton(B_EMPTY_STRING, _BuildButtonShape(TorrentActive), 
						 new BMessage(MSG_CONTROL_BUTTON))
	{
		fTorrentActive = TorrentActive;
	}
	
	BShape* _BuildButtonShape(bool TorrentActive)
	{
		float symbolHeight 	= int(be_plain_font->Size() / 1.33) | 1;
		fStartShape 		= _CreatePlayShape(symbolHeight);
		fStopShape			= _CreatePauseShape(symbolHeight);
		
		if( TorrentActive )
			return fStopShape;
		else
			return fStartShape;
	}
	
	//
	//
	//
	void ToggleShape()
	{
		fTorrentActive = !fTorrentActive;
		
		BShape* NewShape = (fTorrentActive) ? fStopShape : fStartShape;
		
		SetSymbol(NewShape);
		
		
	}
	
	BSize MinSize()
	{
		return BSize(24, 24);
	}
	
	BSize MaxSize()
	{
		return BSize(24, 24);
	}
	
private:
	BShape*		fStartShape;
	BShape*		fStopShape;
	bool		fTorrentActive;
};

//
//
//
DownloadItem* MessageFindDownloadItem(BView* view) 
{
	//DownloadItem* Item = NULL;
	
	//
	// It should never happen, but who knows.
	//
	if( view->Parent() == NULL )
		return NULL;
	
	//
	// First check if the parent is DownloadItem
	// If the parent is not DownloadItem, then it's
	// a BGroupView item for sure.
	//
	if( DownloadItem* Item = dynamic_cast<DownloadItem*>(view->Parent()) )
		return Item;
		
	//
	// If the parent parents' are null, we failed..
	//
	if( view->Parent()->Parent() == NULL )
		return NULL;
	
	//
	// Return our last hope.
	//
	return dynamic_cast<DownloadItem*>(view->Parent()->Parent());
}

////
///
//

filter_result MessageItemClickHook(BMessage* msg, BHandler** handler, BMessageFilter* filter)
{
	//
	//
	//
	BView* view = dynamic_cast<BView*>(*handler);
	
	if( !view )
		return B_DISPATCH_MESSAGE;
		

	//
	//
	//
	DownloadItem* Item = MessageFindDownloadItem(view);
	
	
	if( Item == NULL )
		return B_DISPATCH_MESSAGE;
		
	//
	// @TODO: Send to DownloadItemView.
	//
	
	//
	// If we're here, send message to the parent.
	//
	BPoint where;
	msg->FindPoint("be:view_where", &where);

	//
	//
	//
	Item->MouseDown(where);
		

	//printf("Received message for handler %s\n", (*handler)->Name());
	//msg->PrintToStream();

	return B_SKIP_MESSAGE;
}




static const rgb_color _ActiveBackgroundColor = {230, 230, 230, 255};
static const rgb_color _NormalBackgroundColor = {245, 245, 245, 255};

DownloadItem::DownloadItem(DownloadView* parentView, TorrentObject* torrentObject)
	:	BGroupView(B_HORIZONTAL, 8),
		fSelected(false),
		fDownloadView(parentView),
		fTorrentObject(torrentObject)
{
	SetViewColor(245, 245, 245);
	SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);
	
	
	BMimeType mime;
	fTorrentObject->MimeType(mime);

	//
	//
	//
	fIconView 		= new IconView(mime.Type());
	fNameView 		= new BStringView("NameView", torrentObject->Name());
	fInfoView 		= new BStringView("InfoView", "Info");
	fPeerView 		= new BStringView("PeerView", "PeerInfo");
	fProgressBar 	= new BStatusBar("DownloadProgress", B_EMPTY_STRING);
	
	fControlButton 	= new ControlButton(fTorrentObject->IsActive());
	
	//
	//
	//
	if (fTorrentObject->IsMagnet())
		fTorrentObject->SetMetadataCallbackHandler(this);
	
	//
	//
	//
	//BMessageFilter* fMessageFilter = new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook);

	fIconView->AddFilter( new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook) );
	fNameView->AddFilter( new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook) );
	fInfoView->AddFilter( new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook) );
	fPeerView->AddFilter( new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook) );
	fProgressBar->AddFilter( new BMessageFilter(B_MOUSE_DOWN, MessageItemClickHook) );

	
	fProgressBar->SetMaxValue(100);
	fProgressBar->SetBarHeight(12);


	BGroupLayout* layout = GroupLayout();
	//layout->SetInsets(8, 5, 5, 6);
	layout->SetInsets(8, 5, 5, 8);
	layout->AddView(fIconView);
	BView* verticalGroup = BGroupLayoutBuilder(B_VERTICAL, 0)
		.Add(fNameView)
		.Add(fInfoView)
		.Add(fProgressBar)
		.Add(fPeerView)
		.TopView()
	;
	
	verticalGroup->SetViewColor(ViewColor());
	layout->AddView(verticalGroup);
	
	
	verticalGroup = BGroupLayoutBuilder(B_VERTICAL, 2)
		.SetInsets(0, 14, 0, 0)	// inset top by Button size / 2
		.Add(fControlButton)
		//
		//.Add(fBottomButton)
		.TopView()
	;
	verticalGroup->SetViewColor(ViewColor());
	layout->AddView(verticalGroup);
	

	BFont font;
	fInfoView->GetFont(&font);
	float fontSize = font.Size() * 0.8f;
	font.SetSize(max_c(8.0f, fontSize));
	fInfoView->SetFont(&font, B_FONT_SIZE);
	
	fInfoView->SetHighColor(tint_color(fInfoView->LowColor(), B_DARKEN_4_TINT));
	fPeerView->SetHighColor(tint_color(fPeerView->LowColor(), B_DARKEN_4_TINT));
	
	fNameView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	fInfoView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	fPeerView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

}

DownloadItem::~DownloadItem()
{
	
}

void DownloadItem::AttachedToWindow()
{
	UpdateInfo();
}

void DownloadItem::AllAttached()
{
	fControlButton->SetTarget(this);
	
	SetViewColor(B_TRANSPARENT_COLOR);
	SetLowColor(245, 245, 245);
	//SetLowColor(230, 230, 230);
	SetHighColor(tint_color(LowColor(), B_DARKEN_1_TINT));
}



void DownloadItem::Draw(BRect updateRect)
{
	BRect bounds(Bounds());
	bounds.bottom--;
	FillRect(bounds, B_SOLID_LOW);
	bounds.bottom++;
	StrokeLine(bounds.LeftBottom(), bounds.RightBottom());
}

void DownloadItem::UpdateInfo()
{

	fTorrentObject->Update();
	
	FormatProgressText(fProgressText, fTorrentObject);
	FormatStatusText(fStatusText, fTorrentObject);
	
	float DownloadProgress = static_cast<float>(fTorrentObject->Statistics()->percentDone) * 100.0f;
	
	
	LockLooper();

	fProgressBar->SetTo(DownloadProgress);
	fInfoView->SetText(fProgressText);
	fPeerView->SetText(fStatusText);
	
	UnlockLooper();

/*
	BMessage* Message = new BMessage(MSG_UPDATE_DOWNLOAD);
	
	
	Message->AddFloat("DownloadProgress", DownloadProgress);
	Message->AddString("DownloadInfo", DownloadInfo);
	Message->AddString("DownloadPeer", PeerInfo);
	
	//
	//
	//
	this->PostMessage(Message);
*/
}



void DownloadItem::MouseDown(BPoint point)
{
	//
	//
	//
	fDownloadView->ItemClick(this);
}

void DownloadItem::MessageReceived(BMessage* message)
{
	switch( message->what ) 
	{
	case MSG_CONTROL_BUTTON:
		{
			fControlButton->ToggleShape();
			
			//
			// @TODO: set up for finish torrent i.e stop seeding.
			//			
			if( fTorrentObject->IsActive() )
				fTorrentObject->StopTransfer();
			else
				fTorrentObject->StartTransfer();
		}
		break;
	case MSG_TRANSMISSION_METADATA_CALLBACK:
		OnMetadataComplete();
		break;
//	case MSG_UPDATE_DOWNLOAD:
//		OnUpdateDownload(message);
//		UpdateInfo();
//		break;
	default:
		BGroupView::MessageReceived(message);
		break;
	}
}

const TorrentObject* DownloadItem::GetTorrentObject() const
{
	return fTorrentObject;
}

void DownloadItem::OnUpdateDownload(BMessage* Msg)
{

}

void DownloadItem::OnMetadataComplete()
{
	BMimeType mime;
	fTorrentObject->MimeType(mime);

	//
	//
	//
	fIconView->SetMime(mime.Type());
	fNameView->SetText(fTorrentObject->Name());
}

/*
void DownloadItem::UpdateStatusText()
{
	char FormatBuffer[1024] = {0};	
	const tr_stat* st = fTorrentObject->Statistics();
	

	//
	//
	//
	switch( st->activity )
	{
	case TR_STATUS_STOPPED:
		// @TODO: if finishedSeeding.
		fStatusText.SetTo("Paused");
		break;
//	case TR_STATUS_DOWNLOAD_WAIT:
//		return "Waiting to download" B_UTF8_ELLIPSIS;
//		break;
	case TR_STATUS_CHECK:
		fStatusText.SetToFormat("Checking existing data (%s%%)",
			tr_strpercent(FormatBuffer, st->recheckProgress * 100.0, sizeof(FormatBuffer)));
		break;
	case TR_STATUS_CHECK_WAIT:
		fStatusText.SetTo("Waiting to check existing data" B_UTF8_ELLIPSIS);
		break;
	case TR_STATUS_DOWNLOAD:
		BString DownloadText = tr_formatter_speed_KBps(FormatBuffer, st->pieceDownloadSpeed_KBps, sizeof(FormatBuffer));
		BString UploadText = tr_formatter_speed_KBps(FormatBuffer, st->pieceUploadSpeed_KBps, sizeof(FormatBuffer));
		fStatusText.SetToFormat("Downloading from %d of %d connected peer - "
							  	UTF8_DOWN_ARROW_CHARACTER " %s, " UTF8_UP_ARROW_CHARACTER " %s", 
			st->webseedsSendingToUs + st->peersSendingToUs,
			st->webseedsSendingToUs + st->peersConnected,
			DownloadText.String(), UploadText.String());
			
		break;
	}
}*/

void DownloadItem::OnSelectionChange()
{
	rgb_color backgroundColor = IsSelected() ? _ActiveBackgroundColor : _NormalBackgroundColor;

	
	SetLowColor(backgroundColor);
	
	//
	// Update the Layout background
	//
	for( int i = 0; i < 3; i++ )
	{
		BView* item = GroupLayout()->View()->ChildAt( i );
		
		//
		// This shouldn't fail.
		//	
		if( item == NULL )
			continue;
			
		//
		//
		//
		item->SetViewColor(backgroundColor);
		item->Invalidate();
	}
	
	//
	// BStringView uses ViewColor for the background.
	//
	fNameView->SetViewColor(backgroundColor);
	fInfoView->SetViewColor(backgroundColor);
	fPeerView->SetViewColor(backgroundColor);
	
	
	fNameView->Invalidate();
	fInfoView->Invalidate();
	fPeerView->Invalidate();
	
	Invalidate();
}


bool DownloadItem::IsSelected() const
{
	return fSelected;
}


void DownloadItem::Select()
{
	fSelected = true;
	
	OnSelectionChange();
}


void DownloadItem::Deselect()
{
	fSelected = false;
	
	OnSelectionChange();
}

void DownloadItem::ToggleSelection()
{
	if( IsSelected() )
		Deselect();
	else
		Select();
}



