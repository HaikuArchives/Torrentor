//------------------------------------------------------------------------------
//	Copyright (c) 2010, Stephan Aßmus <superstippi@gmx.de>.
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
//	File Name:		DownloadView.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#ifndef TORRENTOR_DOWNLOAD_VIEW_H
#define TORRENTOR_DOWNLOAD_VIEW_H

class DownloadItem;
class TorrentObject;


class DownloadView : public BGroupView 
{
public:
	DownloadView();
	~DownloadView();

	virtual BSize MinSize();
	
	virtual void Pulse();
	virtual void MouseDown(BPoint point);
	
	void AddItem(TorrentObject* torrentObject);
	void RemoveItem(const DownloadItem* Item);
	
	int32 IndexOf(BPoint point) const;
	
	const DownloadItem*	ItemAt(int32 ItemIndex);
	const DownloadItem*	ItemSelected() const;
	
	void ItemClick(DownloadItem* Item);
	
	bool QuitRequested();

protected:
	virtual void DoLayout();
	
private:
	DownloadItem* fSelectedItem;
};




#endif // TORRENTOR_DOWNLOAD_VIEW_H
