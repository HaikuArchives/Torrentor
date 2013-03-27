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
//	File Name:		PiecesView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <View.h>
#include <ControlLook.h>

#include <libtransmission/transmission.h>

#include "TorrentObject.h"

#include "PiecesView.h"


static const int32	kMaxPieces			= 16;
static const float	kBorder				= 1.0f;

static const rgb_color kBorderColor 		= {165, 165, 165, 255};
static const rgb_color kBackgroundColor 	= {255, 255, 255, 255};
static const rgb_color kBlockEmptyColor		= {255, 255, 255, 255};
static const rgb_color kBlockFlashingColor	= {204,  142, 64, 255};
static const rgb_color kBlockCompleteColor	= {8, 123, 214, 255};
static const BSize 	kViewSize				= BSize(128, 128);


enum PieceStatus_t
{
    PIECE_EMPTY,
    PIECE_DOWNLOADING,
    PIECE_HIGH_PEERS,
    PIECE_FINISHED,
    PIECE_FLASHING
};


PiecesView::PiecesView(const TorrentObject* torrent)
	:	BView("PicesView", B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW),
		fTorrent(torrent),
		fFirstRun(true),
		fPiecesMap(NULL),
		fPiecesPercent(NULL)
{
}

PiecesView::~PiecesView()
{
	delete[] fPiecesMap;
	delete[] fPiecesPercent;
}


void PiecesView::Draw(BRect updateRect)
{
	BRect frame(Bounds());
	
	SetLowColor(kBackgroundColor);
	FillRect(frame, B_SOLID_LOW);
	
	SetLowColor(kBorderColor);
	StrokeRect(frame, B_SOLID_LOW);	
	
	if( fTorrent->IsMagnet() )
		return;
		
	//
	// this should not be here.
	//
	if( fFirstRun )
	{
		float width = kViewSize.Width();
		fNumPieces 	= MIN(fTorrent->Info()->pieceCount, kMaxPieces * kMaxPieces);
		fAcross 	= ceil(sqrt(fNumPieces));
		fWidth 		= (width - (fAcross + 1) * kBorder) / fAcross;
		fExtraBorder= (width - ((fWidth + kBorder) * fAcross + kBorder)) / 2;
	
		fPiecesMap = new int8[fNumPieces];
		fPiecesPercent  = new float[fNumPieces];	
	}
	//
	//
	//
	tr_torrentAmountFinished(fTorrent->Handle(), fPiecesPercent, fNumPieces);
	
	
	//
	//
	//
	for( int i = 0; i < fNumPieces; i++ )
	{
		rgb_color BlockColor;
		
		int row = i % fAcross;
		int col = i / fAcross;
        
        BRect BlockRect(BPoint(row * (fWidth + kBorder) + kBorder + fExtraBorder,
						frame.Width() - (col + 1) * (fWidth + kBorder) - fExtraBorder),
						BSize(fWidth, fWidth));
						
		//
		//
		//
       	if( fPiecesPercent[i] == 1.0 )
		{
			if( fPiecesMap[i] != PIECE_FINISHED && !fFirstRun )
			{
				BlockColor = kBlockFlashingColor;
				fPiecesMap[i] = PIECE_FINISHED;
			}
			else
			{
				BlockColor = kBlockCompleteColor;
				fPiecesMap[i] = PIECE_FINISHED;
			}
        }
        else if( fPiecesPercent[i] == 0.0 )
        {
			BlockColor = kBlockEmptyColor;
			fPiecesMap[i] = PIECE_EMPTY;
        }
        else
        {
			BlockColor = tint_color(kBlockCompleteColor, fPiecesPercent[i]);
            fPiecesMap[i] = PIECE_DOWNLOADING;
        }

		SetLowColor(BlockColor);
		FillRect(BlockRect, B_SOLID_LOW);
	}

	//
	// Draw grid.
	//
	BeginLineArray(fAcross * 2);
	
	//
	// Draw vertical lines
	//
	for( int i = 0; i < fAcross; i++ )
	{
		int x = i * (fWidth + kBorder) + kBorder + fExtraBorder;
		int y = frame.Width() - (i + 1) * (fWidth + kBorder) - fExtraBorder;
		
		//
		//
		//
		if( i > 0 )
			AddLine(BPoint(x, 0), BPoint(x, frame.Height()), kBorderColor);
			
		if( i < fAcross - 1 )
			AddLine(BPoint(0, y), BPoint(frame.Width(), y), kBorderColor);
	}
	EndLineArray();

	//
	//
	//
	if( fFirstRun )
		fFirstRun = false;
}

BSize PiecesView::MinSize()
{
	return kViewSize;
}

BSize PiecesView::PreferredSize()
{
	return kViewSize;
}

BSize PiecesView::MaxSize()
{
	return kViewSize;
}


int PiecesView::GetPieceIndex(int x, int y)
{
	BRect frame(Bounds());
	
	int numAcross = frame.Width() / kMaxPieces;
	int row = y / kMaxPieces;
	int col = x / kMaxPieces;
	
	return (row * numAcross) + col;
}


