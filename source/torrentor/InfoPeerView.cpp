#include <libtransmission/transmission.h>

#include <ColumnTypes.h>
#include <locale/Country.h>


#include "TorrentObject.h"
#include "InfoPeerView.h"

#include <stdio.h>

enum
{
	COLUMN_PEER_ADDRESS	= 0,
	COLUMN_PEER_CLIENT,
	COLUMN_PEER_PROGRESS,
	COLUMN_PEER_PORT,
};



InfoPeerView::InfoPeerView(const TorrentObject* torrent)
	:
	BColumnListView("InfoPeerView", 0, B_PLAIN_BORDER, true),
	fTorrent(torrent)
{
	SetColumnFlags(B_ALLOW_COLUMN_RESIZE);
	
	GraphColumn* fProgressColumn = NULL;
	
	AddColumn(new BStringColumn("IP Address", 180, 50, 500, B_TRUNCATE_MIDDLE), COLUMN_PEER_ADDRESS);
	AddColumn(new BStringColumn("Client", 140, 50, 500, B_TRUNCATE_MIDDLE), COLUMN_PEER_CLIENT);
	AddColumn(fProgressColumn = new GraphColumn("Progress", 80, 80, 140), COLUMN_PEER_PROGRESS);
	AddColumn(new BIntegerColumn("Port", 60, 60, 60), COLUMN_PEER_PORT);

	//
	//
	//
	fProgressColumn->SetTextColor(make_color(0, 0, 0));
	fProgressColumn->SetBackgroundColor(make_color(77, 164, 255));

	
	//
	//
	//
	int peerCount = 0;
	tr_peer_stat* PeerStatus = tr_torrentPeers( fTorrent->Handle(), &peerCount );
	
	
	
	for( int i = 0; i < peerCount; i++ )
	{
		const tr_peer_stat* peer = &PeerStatus[i];
		BRow* row = new BRow();
		
		
		row->SetField(new BStringField(peer->addr), COLUMN_PEER_ADDRESS);
		row->SetField(new BStringField(peer->client), COLUMN_PEER_CLIENT);
		row->SetField(new BIntegerField(peer->progress * 100.0), COLUMN_PEER_PROGRESS);
		row->SetField(new BIntegerField(peer->port), COLUMN_PEER_PORT);
		
		AddRow(row);
	}
	tr_torrentPeersFree( PeerStatus, peerCount );
}
