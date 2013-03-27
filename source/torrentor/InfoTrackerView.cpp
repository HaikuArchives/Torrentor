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
//	File Name:		InfoTrackerView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <libtransmission/transmission.h>

#include <ColumnTypes.h>
#include <locale/Country.h>


#include "TorrentObject.h"
#include "InfoTrackerView.h"

#include <stdio.h>

enum
{
	COLUMN_TRACKER_ADDRESS	= 0,
	COLUMN_TRACKER_UPDATE,
	COLUMN_TRACKER_STATUS
};



InfoTrackerView::InfoTrackerView(const TorrentObject* torrent)
	:	BColumnListView("InfoTrackerView", 0, B_PLAIN_BORDER, true),
		fTorrent(torrent)
{
//		B_ALLOW_COLUMN_MOVE | B_ALLOW_COLUMN_RESIZE
//		| B_ALLOW_COLUMN_POPUP | B_ALLOW_COLUMN_REMOVE

	SetColumnFlags(B_ALLOW_COLUMN_RESIZE);
	
	AddColumn(new BStringColumn("URL", 180, 50, 500, B_TRUNCATE_MIDDLE), COLUMN_TRACKER_ADDRESS);
	AddColumn(new BStringColumn("Update", 180, 50, 500, B_TRUNCATE_MIDDLE), COLUMN_TRACKER_UPDATE);
	AddColumn(new BStringColumn("Status", 180, 50, 500, B_TRUNCATE_MIDDLE), COLUMN_TRACKER_STATUS);
	
	
	
	int trackerCount = 0;
	tr_tracker_stat* trackerStat = tr_torrentTrackers( fTorrent->Handle(), &trackerCount );
	
	//
	//
	for( int i = 0; i < trackerCount; i++ )
	{
		BRow* row = new BRow();
		
		row->SetField(new BStringField(trackerStat[i].announce), COLUMN_TRACKER_ADDRESS);
		row->SetField(new BStringField(trackerStat[i].lastAnnounceResult), COLUMN_TRACKER_UPDATE);
		
		AddRow(row, (BRow*)NULL);
	}
	tr_torrentTrackersFree(trackerStat, trackerCount);
}
