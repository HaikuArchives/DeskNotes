
// File: $RCSFile$
// Revision: $Revision: 1.1 $
// Date: $Date: 2011/03/20 21:57:39 $
// DeskNoteView code

#include "DeskNoteView.h"
#include "FontColourWindow.h"
#include <Invoker.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <SupportKit.h>
#include <OS.h>

const char DeskNoteView::defaultText[]
				= "DeskNotes Version 1.1.0\n\n"
				"Written by Colin Stewart\n\nCopyright 2000";
const char DeskNoteView::aboutText[]
				= "DeskNotes Version 1.1.0\n\nCopyright 2000 by Colin Stewart\n";

DeskNoteView::DeskNoteView(BRect rect)
		:
		BView (rect, "DeskNotes", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
	BRect draggerRect = rect;
	BRect textViewRect = rect;
	BPopUpMenu *draggerPop;
	BMenuItem *popupMenu;
	BMessage *popupMessage;
	ourSize = rect;

	WeAreAReplicant = false;

	draggerRect.OffsetTo(B_ORIGIN);
	draggerRect.top = draggerRect.bottom - 7;
	draggerRect.right = draggerRect.left + 7;

	dragger = new BDragger(draggerRect, this, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);

	draggerPop = dragger -> PopUp();
	popupMessage = new BMessage (DN_LAUNCH);
	popupMenu = new BMenuItem ("Launch DeskNotes", popupMessage);
	draggerPop -> AddItem (popupMenu, 1);	// Add the launch menu item.

	popupMessage = new BMessage (DN_FNT_CLR);
	popupMenu = new BMenuItem ("Settings", popupMessage);
	draggerPop -> AddItem (popupMenu, 1);	// Add the change font/colour menu item.


	textViewRect.bottom -= 8;				// Don't collide with the widgets.

	textView = new DeskNoteTextView (textViewRect, "TextView",
					textViewRect, B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED);
	textView -> SetText (defaultText, strlen (defaultText));
	AddChild (textView);

	background = kDefaultBackgroundColor;
	foreground = kDefaultForegroundColor;
	SetViewColor (background);
	AddChild(dragger);
	CascadeFontAndColour();
	propertiesWindow = NULL;
}


DeskNoteView::DeskNoteView (BMessage *data) : BView (data)
{
	const rgb_color *bckgrnd, *fregrnd;
	ssize_t size;

	propertiesWindow = NULL;
	WeAreAReplicant = true;
	textView = dynamic_cast<DeskNoteTextView *> (FindView ("TextView"));
	dragger = dynamic_cast<BDragger *> (FindView ("_dragger_"));

	data -> FindData ("background_colour", B_RGB_COLOR_TYPE, (const void **)&bckgrnd, &size);
	data -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, (const void **)&fregrnd, &size);

	background = *bckgrnd;
	foreground = *fregrnd;

	CascadeFontAndColour();
	SetResizingMode (B_FOLLOW_NONE);
}


DeskNoteView::~DeskNoteView ()
{

}


status_t DeskNoteView::Archive (BMessage *data, bool deep) const
{
	BView::Archive (data, deep);
	// These first two fields are internal Archiving fields.
	data -> AddString ("class", "DeskNoteView");
	data -> AddString ("add_on", app_signature);

	// Now add the foreground and background colours.
	data -> AddData ("background_colour", B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
	data -> AddData ("foreground_colour", B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));

	return B_NO_ERROR;
}


void DeskNoteView::Draw (BRect rect)
{
	ourSize = Bounds();
	BRect rct = ourSize;
	BRect cleanRct = ourSize;

	rct.top = rct.bottom - 7;
	rct.left = rct.right - 7;

	cleanRct.top = rct.top;
	cleanRct.right = rct.left - 1;	// Don't overlap with the resize widget.
	SetHighColor (widgetcolour);
	FillRect (rct);
	SetHighColor (background);
	FillRect (cleanRct);
}


void DeskNoteView::MessageReceived (BMessage *msg)
{
	BAlert *alert;
	BRect windSize, ourSize;
	BMessenger *messenger;
	BFont currentFont;
	font_family currentFamily;
	font_style currentStyle;
	uint16 currentFace;
	const rgb_color *bckgrnd, *fregrnd;
	ssize_t size;

	switch (msg -> what) {
		case B_ABOUT_REQUESTED:
			// Set the mouse cursor!
			be_app -> SetCursor (B_HAND_CURSOR);
			alert = new BAlert ("DeskNotes", aboutText, "OK");
			alert -> Go();
			break;
		case DN_LAUNCH:
			be_roster -> Launch (app_signature);
			break;
		case DN_CHG_FNT_CLR:
			msg -> FindData ("background_colour", B_RGB_COLOR_TYPE, (const void **)&bckgrnd, &size);
			msg -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, (const void **)&fregrnd, &size);
			background = *bckgrnd;
			foreground = *fregrnd;
			CascadeFontAndColour();
			break;
		case DN_FNT_CLR:
			if (propertiesWindow == NULL) {
				// Set the mouse cursor!
				be_app -> SetCursor (B_HAND_CURSOR);
				ourSize = this -> Frame();
				ourSize.OffsetBy (Window () -> Frame().LeftTop());
				FontColourWindow::CalculateWindowFrame (&windSize, ourSize);
				this->GetFont (&currentFont);
				currentFont.GetFamilyAndStyle (&currentFamily, &currentStyle);
				currentFace = currentFont.Face ();
				messenger = new BMessenger ((BHandler *)this, this->Looper());
				// Set the message type to what we expect to receive.
				orginalSettings = new BMessage (DN_CHG_FNT_CLR);
				// Add the relevant data to the BMessage.
				orginalSettings -> AddString ("title", "Select colors for this note");
				orginalSettings -> AddString ("example", aboutText);
				orginalSettings -> AddData ("background_colour", B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
				orginalSettings -> AddData ("foreground_colour", B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
				orginalSettings -> AddData ("font_family_name", B_STRING_TYPE, &currentFamily, sizeof (font_family));
				orginalSettings -> AddData ("font_face", B_UINT16_TYPE, &currentFace, sizeof (uint16));

				propertiesWindow = new FontColourWindow (windSize, messenger, orginalSettings);
				delete messenger;
				propertiesWindow -> Show();
			}
			break;

		case DN_PROPERTIES_CLOSE:
			delete orginalSettings;
			orginalSettings = NULL;
			propertiesWindow = NULL;
			break;

		default:
			BView::MessageReceived (msg);
	}
}


void DeskNoteView::FrameResized (float width, float height)
{
	BRect txtSize = ourSize = Bounds();

	txtSize.bottom -= 9;
	textView -> SetTextRect (txtSize);
	Invalidate();
}


void DeskNoteView::MouseDown(BPoint point)
{
	thread_id resizeThread;
	BMessage *msg;
	BMenuItem *menuItem;
	BPoint mousePoint;
	uint32 mouseButtons;

	if (!Window () -> IsActive ()) Window () -> Activate (true);

	textView->MakeFocus(true);

	GetMouse (&mousePoint, &mouseButtons, false);
	if (point.x >= (ourSize.right - 7) && point.y >= (ourSize.bottom - 7)) {
		resizeThread = spawn_thread (DeskNoteView::ResizeViewMethod, "Resize Thread", 
										B_DISPLAY_PRIORITY, this);
		if (resizeThread > 0) resume_thread (resizeThread);

	} else if (mouseButtons == B_SECONDARY_MOUSE_BUTTON) {
		_ShowContextMenu(mousePoint);
	}
}


void DeskNoteView::_ShowContextMenu(BPoint where)
{
	bool isRedo;
	undo_state state = textView->UndoState(&isRedo);
	bool isUndo = state != B_UNDO_UNAVAILABLE && !isRedo;

	int32 start;
	int32 finish;
	textView->GetSelection(&start, &finish);

	bool canEdit = textView->IsEditable();
	int32 length = textView->TextLength();

	BPopUpMenu* menu = new BPopUpMenu(B_EMPTY_STRING, false, false);

	BLayoutBuilder::Menu<>(menu)
		.AddItem("Undo", B_UNDO/*, 'Z'*/)
			.SetEnabled(canEdit && isUndo)
		.AddItem("Redo", B_UNDO/*, 'Z', B_SHIFT_KEY*/)
			.SetEnabled(canEdit && isRedo)
		.AddSeparator()
		.AddItem("Cut", B_CUT, 'X')
			.SetEnabled(canEdit && start != finish)
		.AddItem("Copy", B_COPY, 'C')
			.SetEnabled(start != finish)
		.AddItem("Paste", B_PASTE, 'V')
			.SetEnabled(canEdit && be_clipboard->SystemCount() > 0)
		.AddSeparator()
		.AddItem("Select all", B_SELECT_ALL, 'A')
			.SetEnabled(!(start == 0 && finish == length))
		// custom menu
		.AddSeparator()
		.AddItem("About DeskNotes" B_UTF8_ELLIPSIS, new BMessage (B_ABOUT_REQUESTED))
		.AddItem("Settings" B_UTF8_ELLIPSIS, new BMessage (DN_FNT_CLR))
	;

	// If we are replicant add the launch desknotes command to the menu.
	if (WeAreAReplicant) {
		menu -> AddItem (new BMenuItem ("Launch DeskNotes" B_UTF8_ELLIPSIS, new BMessage (DN_LAUNCH)));
	}

	menu->SetTargetForItems(textView);
	ConvertToScreen(&where);
	menu->Go(where, true, true, true);
}

void DeskNoteView::CascadeFontAndColour (void)
{
	BFont fnt;
	widgetcolour.red = (uint8)((double)background.red * (0.8));
	widgetcolour.green = (uint8) ((double)background.green * (0.8));
	widgetcolour.blue = (uint8) ((double)background.blue * (0.8));

	GetFont (&fnt);
	textView -> SetFontAndColor (&fnt, B_FONT_ALL, &foreground);
	textView -> SetViewColor (background);
	this -> Invalidate();
	textView -> Invalidate();
	SetViewColor(background);
	if (dragger != NULL)
		dragger -> Invalidate();

}


void DeskNoteView::SaveNote (BMessage *msg)
{
	// Save the text of the note.
	msg -> AddString ("NoteText", textView -> Text());

	// Save the foreground and background colours.
	msg -> AddData ("background_colour", B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
	msg -> AddData ("foreground_colour", B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
}


void DeskNoteView::RestoreNote (BMessage *msg)
{
	const rgb_color *bckgrnd, *fregrnd;
	ssize_t size;
	const char *text;

	// Find the text of the note.
	if (msg -> FindString ("NoteText", &text) == B_OK) {
		textView -> SetText (text);
	}

	// Find the background colour.
	if (msg -> FindData ("background_colour", B_RGB_COLOR_TYPE, (const void **)&bckgrnd, &size) == B_OK) {
		background = *bckgrnd;
	}

	// Find the foreground colour.
	if (msg -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, (const void **)&fregrnd, &size) == B_OK) {
		foreground = *fregrnd;
	}

	// Update the colour of the text view and widget.
	CascadeFontAndColour ();
}


BArchivable * DeskNoteView::Instantiate (BMessage *data)
{
	if (!validate_instantiation(data, "DeskNoteView"))
		return NULL;
	return new DeskNoteView(data);
}


int32 DeskNoteView::ResizeViewMethod (void *data)
{
	uint32 buttons;
	BPoint cursor;
	float x, y;
	DeskNoteView *theView = (DeskNoteView *)data;

	do {
		theView -> Window () -> Lock();
		theView -> GetMouse (&cursor, &buttons);
		if (cursor.x > 30) x = cursor.x;
		else x = 30;
		if (cursor.y > 20) y = cursor.y;
		else y = 20;
		if (theView -> WeAreAReplicant) theView -> ResizeTo (x, y);
		else theView -> Window () -> ResizeTo (x, y);
		theView -> Window () -> Unlock();
		snooze (20 * 1000);
	}
	while (buttons);

	return 0;
}


void DeskNoteView::DetachedFromWindow ()
{
	const rgb_color *bckgrnd, *fregrnd;
	ssize_t size;
	// If we still have our preferences window open we should close it!
	if (propertiesWindow != NULL) {
		propertiesWindow -> PostMessage (DN_PROPERTIES_URGENT_CLOSE);
		orginalSettings -> FindData ("background_colour", B_RGB_COLOR_TYPE, (const void **)&bckgrnd, &size);
		orginalSettings -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, (const void **)&fregrnd, &size);
		background = *bckgrnd;
		foreground = *fregrnd;
		CascadeFontAndColour();
		delete orginalSettings;
		propertiesWindow = NULL;
	}
}

