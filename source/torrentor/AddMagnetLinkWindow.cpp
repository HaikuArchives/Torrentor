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
//	File Name:		AddMagnetLinkWindow.h
//	Authors:		Guido Pola <prodito@live.com>
//	Description:	
//------------------------------------------------------------------------------
#include <Application.h>
#include <Button.h>
#include <Clipboard.h>
#include <ControlLook.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <String.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>

#include "TorrentorMessages.h"
#include "AddMagnetLinkWindow.h"

enum
{
	MSG_MAGNET_OPEN,	
};

AddMagnetLinkWindow::AddMagnetLinkWindow()
	:	BWindow(BRect(),
				"Open Magnet Link", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS |
				B_ASYNCHRONOUS_CONTROLS |
				B_NOT_ZOOMABLE |
				B_NOT_RESIZABLE |
				B_PULSE_NEEDED )
{
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	
	const float spacing = be_control_look->DefaultItemSpacing();
	
	//
	//
	//
	BButton* fCancelButton 	= new BButton("", "Cancel", new BMessage(B_QUIT_REQUESTED));
	BButton* fOpenButton 	= new BButton("", "Open", new BMessage(MSG_MAGNET_OPEN));
	BStringView* fLinkLabel	= new BStringView("", "Link:");
	fLinkText = new BTextControl("", "", NULL);
	fLinkText->SetExplicitMinSize(BSize(400, 20));
	
	//
	//
	//
	AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.AddGroup(B_HORIZONTAL)
			.Add(fLinkLabel)
			.Add(fLinkText)
		.End()
		.AddGroup(B_HORIZONTAL)
			.Add(fCancelButton)
			.AddGlue()
			.Add(fOpenButton)
		.End()
		.SetInsets(spacing, spacing, spacing, spacing)
	);
	
	fLinkText->MakeFocus();
	fLinkText->SetText(GetMagnetFromClipboard());
	fOpenButton->MakeDefault(true);
	CenterOnScreen();
	Run();
}

BString AddMagnetLinkWindow::GetMagnetFromClipboard() {
	const char *text = NULL;
	ssize_t numBytes;
	status_t status;
	BMessage *clip = (BMessage *)NULL;
	if (be_clipboard->Lock()) {
		if ((clip = be_clipboard->Data())) {
			status = clip->FindData("text/plain", B_MIME_TYPE, (const void **)&text, &numBytes);
		}
		be_clipboard->Unlock();
	}
	
	if (status == B_OK) {
		BString link(text);
		if (link.FindFirst("magnet:") == 0) {
			return link;
		}
	}
	
	return B_EMPTY_STRING;
}

void AddMagnetLinkWindow::MessageReceived(BMessage* message)
{
	switch( message->what )
	{
	case MSG_MAGNET_OPEN:
		OnMagnetOpenRequest();
		break;
	default:
		BWindow::MessageReceived(message);
		break;
	}
}

void AddMagnetLinkWindow::OnMagnetOpenRequest()
{
	//
	//
	//
	BMessage* message = new BMessage(MSG_OPEN_MAGNET_LINK);
	message->AddString("URL", fLinkText->Text());
	
	be_app->PostMessage(message);
	Quit();
}

