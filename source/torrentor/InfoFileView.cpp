#include <libtransmission/transmission.h>

#include <ColumnTypes.h>
#include <locale/Country.h>


#include "TorrentObject.h"
#include "InfoFileView.h"

#include <stdio.h>

enum
{
	COLUMN_FILE_NAME	= 0,
};



InfoFileView::InfoFileView(const TorrentObject* torrent)
	:	BColumnListView("InfoFileView", 0, B_PLAIN_BORDER, true),
		fTorrent(torrent)
{
	//
	//
	//
	AddColumn(new BStringColumn("Name", 180, 50, 500,
		B_TRUNCATE_MIDDLE), COLUMN_FILE_NAME);
	
	//AddColumn(new GraphColumn("Progress", 180, 50, 500), COLUMN_PEER_PROGRESS);
	
	//
	//
	//
	int FileCount = fTorrent->Info()->fileCount;
	tr_file* fileList = fTorrent->Info()->files;
	
	for( int i = 0; i < FileCount; i++ )
	{
		BRow* row = new BRow();
		
		//
		//
		//
		const char* name = strchr(fileList[i].name, '/') + 1;
		
		
		row->SetField(new BStringField(name), COLUMN_FILE_NAME);
		//row->SetField(new BIntegerField(PeerStatus[i].progress * 100.0), COLUMN_PEER_PROGRESS);
		
		AddRow(row, (BRow*)NULL);
	}

/*


void           tr_torrentPeersFree( tr_peer_stat * peerStats,
*/
}
