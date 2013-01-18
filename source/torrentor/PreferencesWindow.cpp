#include <Button.h>
#include <CheckBox.h>
#include <ControlLook.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <SpaceLayoutItem.h>
#include <TabView.h>
#include <TextControl.h>
#include <Window.h>

#include "FolderSelect.h"
#include "PreferencesWindow.h"


static const BRect kDefaultFrame = BRect(120, 80, 800, 600);


PreferencesWindow::PreferencesWindow()
	:	BWindow(kDefaultFrame, "Settings", 
				B_TITLED_WINDOW_LOOK,
				B_NORMAL_WINDOW_FEEL, 
				B_AUTO_UPDATE_SIZE_LIMITS | 
				B_ASYNCHRONOUS_CONTROLS | 
				B_NOT_ZOOMABLE)

{
	SetLayout(new BGroupLayout(B_VERTICAL));
	
	
	//fApplyButton = new BButton(B_TRANSLATE("Apply"), new BMessage(MSG_APPLY));
	fApplyButton = new BButton("Apply");
	
	
	float spacing = be_control_look->DefaultItemSpacing();

	BTabView* tabView = new BTabView("settings pages", B_WIDTH_FROM_LABEL);
	
	
	tabView->AddTab(_CreateTorrentsPage(spacing));

	AddChild(BGroupLayoutBuilder(B_VERTICAL, spacing)
		.Add(tabView)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
//			.Add(fRevertButton)
			.AddGlue()
//			.Add(fCancelButton)
			.Add(fApplyButton)
		)
		.SetInsets(spacing, spacing, spacing, spacing)
	);

	
	
	//
	//
	//
	CenterOnScreen();
	Run();
}


BView* PreferencesWindow::_CreateTorrentsPage(float spacing)
{
	//
	//
	//
	fAutoAddTorrentsFrom = new BCheckBox("Auto add torrents from",
		"Automatically add torrents from:", 
		NULL /*new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED)*/);
	fAutoAddTorrentsFrom->SetValue(B_CONTROL_OFF);
	
	
	fAutoAddTorrentsFromFolder = new BTextControl("auto add torrents from folder",
		"", "",	NULL/*new BMessage(MSG_DOWNLOAD_FOLDER_CHANGED)*/);
	fAutoAddTorrentsFromFolder->SetEnabled(false);


	BCheckBox* fShowOptionsDialog = new BCheckBox("Show options dialog",
		"Show options dialog", 
		NULL /*new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED)*/);
	fShowOptionsDialog->SetValue(B_CONTROL_OFF);
	
	BCheckBox* fStartWhenAdded = new BCheckBox("start-when-added",
		"Start when added", 
		NULL /*new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED)*/);
	fStartWhenAdded->SetValue(B_CONTROL_OFF);
	
	BCheckBox* fAppenDotPart = new BCheckBox("append-dot-part",
		"Append \".part\" to incomplete files names", 
		NULL /*new BMessage(MSG_AUTO_HIDE_INTERFACE_BEHAVIOR_CHANGED)*/);
	fAppenDotPart->SetValue(B_CONTROL_OFF);
	
	
	//FolderSelect* fDownloadFolder = new FolderSelect("download-folder-select", "Download Folder:",
	//												 "/boot/home/Desktop");
	
	//
	//
	//
	BView* view = BGroupLayoutBuilder(B_VERTICAL, spacing / 2)
		.Add(BGridLayoutBuilder(spacing / 2, spacing / 2)
			.Add(fAutoAddTorrentsFrom, 0, 0)
			.Add(fAutoAddTorrentsFromFolder, 1, 0)
		)
		.Add(fShowOptionsDialog)
		.Add(fStartWhenAdded)
		.Add(fAppenDotPart)
		.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
		.SetInsets(spacing, spacing, spacing, spacing)

		.TopView()
	;
	view->SetName("Torrents");
	return view;

}



bool PreferencesWindow::QuitRequested()
{
	if( !IsHidden() )
		Hide();
	
	return false;
}

