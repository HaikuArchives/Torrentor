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
