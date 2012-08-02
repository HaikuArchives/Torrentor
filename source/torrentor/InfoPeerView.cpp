#include <libtransmission/transmission.h>

#include <ColumnTypes.h>
#include <locale/Country.h>


#include "TorrentObject.h"
#include "InfoPeerView.h"

#include <stdio.h>

enum
{
	COLUMN_PEER_ADDRESS	= 0,
	COLUMN_PEER_PROGRESS,
};



InfoPeerView::InfoPeerView(const TorrentObject* torrent)
	:
	BColumnListView("InfoPeerView", 0, B_PLAIN_BORDER, true),
	fTorrent(torrent)
{	
	AddColumn(new BStringColumn("IP Address", 180, 50, 500,
		B_TRUNCATE_MIDDLE), COLUMN_PEER_ADDRESS);
	AddColumn(new GraphColumn("Progress", 180, 50, 500), COLUMN_PEER_PROGRESS);
	
	//
	//
	//
	int peerCount = 0;
	tr_peer_stat* PeerStatus = tr_torrentPeers( fTorrent->Handle(), &peerCount );
	
	
	
	for( int i = 0; i < peerCount; i++ )
	{
		BRow* row = new BRow();
		
		//
		//
		//
		//BBitmap* FlagIcon = new BBitmap(BRect(0, 0, 15, 15), B_RGBA32);
		
		// Get the flag icon.
		//{
		//	BCountry Country(... geolocation);
		//	Country.GetIcon(FlagIcon);
		//}
		
		
		
		row->SetField(new BStringField(PeerStatus[i].addr), COLUMN_PEER_ADDRESS);
		row->SetField(new BIntegerField(PeerStatus[i].progress * 100.0), COLUMN_PEER_PROGRESS);
		
		AddRow(row, (BRow*)NULL);
	}
	
	tr_torrentPeersFree( PeerStatus, peerCount );
/*


void           tr_torrentPeersFree( tr_peer_stat * peerStats,
*/
}
