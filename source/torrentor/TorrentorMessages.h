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
//	File Name:		TorrentorMessages.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------

//		Message					Target			Description
// MSG_OPEN_MAGNET_REQUEST | TorrentorApp | Request to show OpenMagnetWindow 	|
// MSG_OPEN_MAGNET_LINK	   | TorrentorApp | Request to open a magnet link 		|
// MSG_OPEN_TORRENT_RESULT | TorrentorApp | ... 
//
enum
{
	__MSG_TORRENTOR_START	= 0x4000,
	MSG_OPEN_MAGNET_REQUEST,				// 
	MSG_OPEN_MAGNET_LINK,
	MSG_OPEN_TORRENT_RESULT,
	
	//
	MSG_TORRENT_COMPLETENESS,
	
	//
	MSG_TORRENT_PAUSE_START_ALL,
};
