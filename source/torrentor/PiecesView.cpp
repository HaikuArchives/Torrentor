#include <View.h>
#include <ControlLook.h>

#include <libtransmission/transmission.h>

#include "TorrentObject.h"

#include "PiecesView.h"



static const rgb_color _BorderColor 	= {230, 230, 230, 255};
static const rgb_color _BackgroundColor = {255, 255, 255, 255};
static const rgb_color _BlockColor 		= {50, 150, 255, 255};


PiecesView::PiecesView(const TorrentObject* torrent)
	:	BView("PicesView", B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW),
		fTorrent(torrent)
{

}


void PiecesView::Draw(BRect updateRect)
{
	BRect frame(Bounds());

	//
	//
	//


	
	SetLowColor(_BackgroundColor);
	
	FillRect(frame, B_SOLID_LOW);

	//
	//
	//
	be_control_look->DrawBorder(this, frame, updateRect, _BorderColor, B_PLAIN_BORDER);
}

