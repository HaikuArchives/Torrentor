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
//	File Name:		DownloadView.cpp
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <GroupLayoutBuilder.h>
#include <GroupView.h>
#include <Looper.h>
#include <Messenger.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <SpaceLayoutItem.h>
#include "DownloadItem.h"
#include "DownloadView.h"

#include <cstdio>


class DownloadScrollView : public BScrollView {
public:
	DownloadScrollView(BView* target)
		:	BScrollView("DownloadScrollView", target, 0, false, true, B_NO_BORDER)
	{
	}

protected:
	virtual void DoLayout()
	{
		BScrollView::DoLayout();
		
		//
		// Tweak scroll bar layout to hide part of the frame for better looks.
		//
		BScrollBar* scrollBar = ScrollBar(B_VERTICAL);
		scrollBar->MoveBy(1, 1);
		scrollBar->ResizeBy(0, -B_V_SCROLL_BAR_WIDTH);
		Target()->ResizeBy(1, 0);
		
		//
		// Set the scroll steps
		if (BView* item = Target()->ChildAt(0)) 
		{
			scrollBar->SetSteps(item->MinSize().height + 1, item->MinSize().height + 1);
		}
	}
};

DownloadView::DownloadView()
	:	BGroupView(B_VERTICAL, 0.0),
		fSelectedItem(NULL)
{
	SetFlags(Flags() | B_PULSE_NEEDED);
	SetViewColor(245, 245, 245);
	AddChild(BSpaceLayoutItem::CreateGlue());
	
	//
	// We attach ourselves to our custom scrollview.
	//
	fScrollView = new DownloadScrollView(this);
}

DownloadView::~DownloadView()
{
	
}


void DownloadView::Pulse()
{
	for( int32 i = GroupLayout()->CountItems() - 1;
		BLayoutItem* item = GroupLayout()->ItemAt(i); i--) 
	{
		
		DownloadItem* view = dynamic_cast<DownloadItem*>(item->View());
		if (!view)
			continue;
		
		//
		//
		//
		//BMessage* Message = new BMessage(MSG_UPDATE_DOWNLOAD);
		//BMessenger(view).SendMessage(Message);
		
		view->UpdateInfo();
	}
}

void DownloadView::MouseDown(BPoint point)
{
	//
	//
	//
	if( fSelectedItem )
		fSelectedItem->Deselect();
		
	
	fSelectedItem = NULL;

}

BView* DownloadView::ScrollView() const
{
	return static_cast<BView*>(fScrollView);
}

int32 DownloadView::IndexOf(BPoint point) const
{
	
	for( int32 i = GroupLayout()->CountItems() - 1;
		BLayoutItem* item = GroupLayout()->ItemAt(i); i--) 
	{
		if( item->Frame().Contains(point) )
			return i;
	}
	return -1;
}

const DownloadItem*	DownloadView::ItemAt(int32 ItemIndex)
{
	return 	NULL;
}

const DownloadItem*	DownloadView::ItemSelected() const
{
	return fSelectedItem;	
}

void DownloadView::RemoveItem(const DownloadItem* Item)
{
	//
	//
	//
	if( fSelectedItem == Item )
		fSelectedItem = NULL;
		
	
	//
	//
	//
	GroupLayout()->RemoveView( const_cast<DownloadItem*>(Item) );
	
	//
	//
	//
	delete Item;
	
}

BSize DownloadView::MinSize()
{
	BSize minSize = BGroupView::MinSize();
	return BSize(minSize.width, (GroupLayout()->CountItems() - 1) * 68);
}
/*
BSize DownloadView::MaxSize()
{
	BSize maxSize = BGroupView::MaxSize();
	return maxSize; //BSize(maxSize.width, MinSize().Height());
}

BSize DownloadView::PreferredSize()
{
	return MinSize();
}*/

void DownloadView::DoLayout()
{
	BGroupView::DoLayout();

	if (BScrollBar* scrollBar = ScrollBar(B_VERTICAL)) 
	{
		BSize minSize = BGroupView::MinSize();
		float height = Bounds().Height();
		float max = minSize.height - height;
		scrollBar->SetRange(0, max);
		if (minSize.height > 0)
			scrollBar->SetProportion(height / minSize.height);
		else
			scrollBar->SetProportion(1);
	}
}

//
//
//
void DownloadView::AddItem(TorrentObject* torrentObject)
{
	int32 ItemIndex = GroupLayout()->CountItems() - 1;
	
	DownloadItem* Item = new DownloadItem(this, torrentObject);
	GroupLayout()->AddView(ItemIndex, Item);
}


void DownloadView::ItemClick(DownloadItem* Item)
{
	//
	// 	If the item is not selected, we need to deselect 
	//	the previous selected item (if any).
	//
	if( fSelectedItem && Item != fSelectedItem )
	{
		fSelectedItem->Deselect();
	}
	
	//
	//
	//
	Item->Select();
	
	//
	//
	//
	fSelectedItem = Item;
	
}
