/*
 * Original copyright 2000 by Colin Stewart (http://www.owlfish.com/).
 * All rights reserved.
 * Distributed under the terms of the BSD (3-clause) License.
 *
 * Authors:
 *		Janus2, 2015
 *		Humdinger, 2021
 *
 */


#include <AboutWindow.h>
#include <Catalog.h>
#include <Invoker.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <OS.h>
#include <SupportKit.h>


#include "DeskNoteView.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "View"


const float kWidgetSize = 7;


DeskNoteView::DeskNoteView(BRect rect)
	:
	BView(rect, "DeskNotes", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
	BRect draggerRect = rect;
	BRect textViewRect = rect;
	BPopUpMenu* draggerPop;
	BMenuItem* popupMenu;
	BMessage* popupMessage;
	ourSize = rect;

	WeAreAReplicant = false;

	draggerRect.OffsetTo(B_ORIGIN);
	draggerRect.top = draggerRect.bottom - kWidgetSize;
	draggerRect.right = draggerRect.left + kWidgetSize;

	dragger = new BDragger(draggerRect, this, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
	textViewRect.bottom -= kWidgetSize + 1; // Don't collide with the widgets.

	textView = new DeskNoteTextView(textViewRect, "TextView", textViewRect,
		B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED);
	BString defaultText(B_TRANSLATE(
		"\n\tWelcome to DeskNotes!\n\n"
		"\t*\tUse as Replicants on your Desktop\n"
		"\t*\tResize with the bottom-right widget\n"
		"\t*\tRight-click for a context menu\n"
		"\t*\tUse different colors from the 'Color' menu\n"
		"\t*\tColors dropped from other apps like\n"
		"\t\tIcon-O-Matic change the background\n\n"
		"\tHave a nice day!"));
	textView->SetText(defaultText);
	AddChild(textView);

	background = palette[0];
	SetViewColor(background);
	AddChild(dragger);
	_SetColors();
}


DeskNoteView::DeskNoteView(BMessage* data)
	:
	BView(data)
{
	const rgb_color* bckgrnd;
	ssize_t size;

	WeAreAReplicant = true;
	textView = dynamic_cast<DeskNoteTextView*>(FindView("TextView"));
	dragger = dynamic_cast<BDragger*>(FindView("_dragger_"));

	data->FindData(
		"background_colour", B_RGB_COLOR_TYPE, (const void**) &bckgrnd, &size);

	background = *bckgrnd;
	_SetColors();
	SetResizingMode(B_FOLLOW_NONE);
}


DeskNoteView::~DeskNoteView()
{
}


status_t
DeskNoteView::Archive(BMessage* data, bool deep) const
{
	BView::Archive(data, deep);
	// These first two fields are internal Archiving fields.
	data->AddString("class", "DeskNoteView");
	data->AddString("add_on", app_signature);

	// Now add the foreground and background colours.
	data->AddData(
		"background_colour", B_RGB_COLOR_TYPE, &background, sizeof(rgb_color));

	return B_NO_ERROR;
}


void
DeskNoteView::Draw(BRect rct)
{
	ourSize = Bounds();
	BRect rect = ourSize;
	BRect cleanRect = ourSize;

	rect.top = rect.bottom - kWidgetSize;
	rect.left = rect.right - kWidgetSize;

	cleanRect.top = rect.top;
	cleanRect.right = rect.left - 1; // Don't overlap with the resize widget.
	SetHighColor(widgetcolour);
	FillRect(rect);
	SetHighColor(background);
	FillRect(cleanRect);
}


void
DeskNoteView::MessageReceived(BMessage* msg)
{
	BAlert *alert;
	BRect windSize, ourSize;
	BMessenger *messenger;
	BMessage *message;
	BMenuItem *item;
	BMenu *menu;
	uint16 currentFace;
	const rgb_color *bckgrnd;
	ssize_t size;
	const char *fontFamily, *fontStyle;
	void *ptr;

	switch (msg->what) {
		case B_ABOUT_REQUESTED:
		{
			// Set the mouse cursor!
			be_app->SetCursor(B_HAND_CURSOR);

			BAboutWindow* aboutWin = new BAboutWindow("DeskNotes",
				app_signature);
			aboutWin->AddDescription(B_TRANSLATE(
				"Pin little notes as replicants on your Desktop.\n"
				"Dropped colors change the background of a note."));
			const char* extraCopyrights[] = {
				"2012 siarzhuk ",
				"2015 Janus",
				"2021 Humdinger",
				NULL
			};
			const char* authors[] = {
				B_TRANSLATE("Colin Stewart (original author)"),
				"Humdinger",
				"Janus",
				"siarzhuk",
				NULL
			};
			aboutWin->AddCopyright(2000, "Colin Stewart", extraCopyrights);
			aboutWin->AddAuthors(authors);
			aboutWin->Show();
			break;
		}
		case DN_LAUNCH:
		{
			be_roster->Launch(app_signature);
			break;
		}
		case DN_COLOR:
		{
			ssize_t colorLength;
			if (msg->FindData("color", B_RGB_COLOR_TYPE,
					(const void**)&bckgrnd, &colorLength) == B_OK
				&& colorLength == sizeof(rgb_color)) {
				background = *bckgrnd;
				_SetColors();
			}
			break;
		}
		// Font type
		case FONT_FAMILY:
		{
			fontFamily = NULL;
			fontStyle = NULL;

			// Setting the font family
			msg->FindPointer("source", &ptr);
			fCurrentFont = static_cast <BMenuItem*>(ptr);
			fontFamily = fCurrentFont->Label();
			SetFontStyle(fontFamily, fontStyle);
		}
		break;
		// Font style
		case FONT_STYLE:
	    {
			fontFamily = NULL;
			fontStyle = NULL;

			// Setting the font style
			msg->FindPointer("source", &ptr);
			item = static_cast <BMenuItem*>(ptr);
			fontStyle = item->Label();
			menu = item->Menu();

			if (menu != NULL) {
				fCurrentFont = menu->Superitem();
				if (fCurrentFont != NULL)
					fontFamily = fCurrentFont->Label();
			}
			SetFontStyle(fontFamily, fontStyle);
		}
		break;
		default:
			BView::MessageReceived(msg);
	}
}


void
DeskNoteView::FrameResized(float width, float height)
{
	BRect nowRect = textView->TextRect();
	BRect txtSize = ourSize = Bounds();
	txtSize.InsetBy(nowRect.left, 0); // keep left/right margins of the textview

	txtSize.bottom -= kWidgetSize + 2;
	textView->SetTextRect(txtSize);
	Invalidate();
}


void
DeskNoteView::MouseDown(BPoint point)
{
	thread_id resizeThread;
	BMessage* msg;
	BMenuItem* menuItem;
	BPoint mousePoint;
	uint32 mouseButtons;

	if (!Window()->IsActive())
		Window()->Activate(true);

	textView->MakeFocus(true);

	GetMouse(&mousePoint, &mouseButtons, false);
	if (point.x >= (ourSize.right - kWidgetSize) && point.y
			>= (ourSize.bottom - kWidgetSize)) {
		resizeThread = spawn_thread(DeskNoteView::ResizeViewMethod,
			"Resize Thread", B_DISPLAY_PRIORITY, this);
		if (resizeThread > 0)
			resume_thread(resizeThread);

	}
	else if (mouseButtons == B_SECONDARY_MOUSE_BUTTON)
		_ShowContextMenu(mousePoint);
}


void
DeskNoteView::_BuildStyleMenu(BMenu* menu)
{
	//variables
	font_family plainFamily, family;
	font_style plainStyle, style;
	BMenuItem* menuItem = NULL;
	int32 numFamilies, numStyles;
	BMenu* fontMenu;
	uint32 flags;

	if (menu == NULL)
		return;

	//Font Menu
	fCurrentFont = 0;

	be_plain_font->GetFamilyAndStyle(&plainFamily,&plainStyle);

	// Taking the number of font families
	numFamilies = count_font_families();
	for (int32 i = 0; i < numFamilies; i++) {
		// Getting the font families
		if (get_font_family(i, &family) == B_OK) {
			fontMenu = new BMenu(family);
			fontMenu->SetRadioMode(true);	// I can set only one item as "in use"
			menuItem = new BMenuItem(fontMenu, new BMessage(FONT_FAMILY));
			menuItem->SetTarget(this);
			menu->AddItem(menuItem);

			if (!strcmp(plainFamily,family)) {
				//menuItem->SetMarked (true);
				fCurrentFont = menuItem;
			}
			//Number of styles of that font family
			numStyles = count_font_styles(family);

			for (int32 j = 0; j < numStyles; j++) {
				if (get_font_style(family,j,&style,&flags)==B_OK) {
					menuItem = new BMenuItem(style, new BMessage(FONT_STYLE));
					menuItem->SetTarget(this);
					fontMenu->AddItem(menuItem);

					//if (!strcmp (plainStyle, style))
						//menuItem->SetMarked(true);

				}
			}
		}
	}
}


void
DeskNoteView::_BuildColorMenu(BMenu* menu)
{
	if (menu == NULL)
		return;

	BFont font;
	menu->GetFont(&font);
	font_height fh;
	font.GetHeight(&fh);

	const float itemHeight = ceilf(fh.ascent + fh.descent + 2 * fh.leading);
	const float margin = 8.0;
	const int nbColumns = 4;

	BMessage msgTemplate(DN_COLOR);
	BRect matrixArea(0, 0, 0, 0);

	// we place the color palette, reserving room at the top
	for (uint i = 0; i < sizeof(palette) / sizeof(rgb_color); i++) {
		BPoint topLeft((i % nbColumns) * (itemHeight + margin),
			(i / nbColumns) * (itemHeight + margin));
		BRect buttonArea(topLeft.x, topLeft.y, topLeft.x + itemHeight,
			topLeft.y + itemHeight);
		buttonArea.OffsetBy(margin, margin);

		ColorMenuItem* colItem
			= new ColorMenuItem("", palette[i], new BMessage(msgTemplate));

		if (colItem != NULL && palette[i] == background)
			colItem->SetMarked(true);

		colItem->SetTarget(this);
		menu->AddItem(colItem, buttonArea);

		buttonArea.OffsetBy(margin, margin);
		matrixArea = matrixArea | buttonArea;
	}

	// separator at the bottom to add spacing in the matrix menu
	matrixArea.top = matrixArea.bottom;
	menu->AddItem(new BSeparatorItem(), matrixArea);
}


void
DeskNoteView::_SetColors()
{
	float thresh
		= background.red + (background.green * 1.25f) + (background.blue * 0.45f);
	if (thresh >= 360) {
		foreground.red = 11;
		foreground.green = 11;
		foreground.blue = 11;
		widgetcolour = tint_color(background, B_DARKEN_1_TINT);
	}
	else {
		foreground.red = 244;
		foreground.green = 244;
		foreground.blue = 244;
		widgetcolour = tint_color(background, B_LIGHTEN_1_TINT);
	}

	BFont font;
	GetFont(&font);
	textView->SetFontAndColor(&font, B_FONT_ALL, &foreground);
	textView->SetViewColor(background);
	this->Invalidate();
	textView->Invalidate();
	SetViewColor(background);
	if (dragger != NULL)
		dragger->Invalidate();
}


void
DeskNoteView::_ShowContextMenu(BPoint where)
{
	bool isRedo;
	undo_state state = textView->UndoState(&isRedo);
	bool isUndo = state != B_UNDO_UNAVAILABLE && !isRedo;

	int32 start;
	int32 finish;
	textView->GetSelection(&start, &finish);

	bool canEdit = textView->IsEditable();
	int32 length = textView->TextLength();

	BPopUpMenu* menu = new BPopUpMenu(B_EMPTY_STRING, false, false,B_ITEMS_IN_COLUMN);

	colorMenu = new BMenu(B_TRANSLATE("Color"), 0, 0);
	_BuildColorMenu(colorMenu);

	styleMenu = new BMenu(B_TRANSLATE("Font"));
	styleMenu->SetRadioMode(true);
	_BuildStyleMenu(styleMenu);

	BLayoutBuilder::Menu<>(menu)
		.AddItem(B_TRANSLATE("Undo"), B_UNDO)
		.SetEnabled(canEdit && isUndo)
		.AddItem(B_TRANSLATE("Redo"), B_UNDO)
		.SetEnabled(canEdit && isRedo)
		.AddSeparator()
		.AddItem(B_TRANSLATE("Cut"), B_CUT)
		.SetEnabled(canEdit && start != finish)
		.AddItem(B_TRANSLATE("Copy"), B_COPY)
		.SetEnabled(start != finish)
		.AddItem(B_TRANSLATE("Paste"), B_PASTE)
		.SetEnabled(canEdit && be_clipboard->SystemCount() > 0)
		.AddSeparator()
		.AddItem(B_TRANSLATE("Select all"), B_SELECT_ALL)
		.SetEnabled(!(start == 0 && finish == length))
		.AddSeparator()
		// custom menu
		.AddItem(colorMenu)
		.AddItem(styleMenu)
		.AddSeparator()
		// About section
		.AddItem(B_TRANSLATE("About DeskNotes" B_UTF8_ELLIPSIS),
			new BMessage(B_ABOUT_REQUESTED));

	// If we are replicant add the launch desknotes command to the menu.
	if (WeAreAReplicant) {
		menu->AddItem(new BMenuItem(
			B_TRANSLATE("Launch DeskNotes" B_UTF8_ELLIPSIS),
			new BMessage(DN_LAUNCH)));
	}

	menu->SetTargetForItems(textView);
	ConvertToScreen(&where);
	menu->Go(where, true, true, true);
}


void
DeskNoteView::SaveNote(BMessage* msg)
{
	// Save the text of the note.
	msg->AddString("NoteText", textView->Text());

	// Save the foreground and background colours.
	msg->AddData(
		"background_colour", B_RGB_COLOR_TYPE, &background, sizeof(rgb_color));
}


// Function for the changes in the "type of font"
void DeskNoteView::SetFontStyle(const char* fontFamily, const char* fontStyle)
{
	// Variables
	BMenuItem *superItem;
	BMenuItem *menuItem;
	BFont font;
	font_family oldFamily;
	font_style oldStyle;
	uint32 sameProperties;
	rgb_color sameColor;
	BMenuItem *oldItem;

	textView->GetFontAndColor(&font, &sameProperties, &sameColor);
	// Copying the current font family and font style
	font.GetFamilyAndStyle(&oldFamily, &oldStyle);

	if (strcmp(oldFamily, fontFamily)) {
		oldItem = styleMenu->FindItem(oldFamily);

		if (oldItem != NULL)
			// Removing the check
			oldItem->SetMarked(false);
	}

	font.SetFamilyAndStyle(fontFamily, fontStyle);
	textView->SetFontAndColor(&font);

	superItem = styleMenu->FindItem(fontFamily);

		if (superItem != NULL)
			superItem->SetMarked(true);	// Check the one that was selected
}


void
DeskNoteView::RestoreNote(BMessage* msg)
{
	const rgb_color* bckgrnd;
	ssize_t size;
	const char* text;

	// Find the text of the note.
	if (msg->FindString("NoteText", &text) == B_OK)
		textView->SetText(text);

	// Find the background colour.
	if (msg->FindData("background_colour", B_RGB_COLOR_TYPE,
			(const void**) &bckgrnd, &size) == B_OK)
		background = *bckgrnd;

	// Update the colour of the text view and widget.
	_SetColors();
}


BArchivable*
DeskNoteView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "DeskNoteView"))
		return NULL;
	return new DeskNoteView(data);
}


int32
DeskNoteView::ResizeViewMethod(void* data)
{
	uint32 buttons;
	BPoint cursor;
	float x, y;
	DeskNoteView* theView = (DeskNoteView*) data;

	do {
		theView->Window()->Lock();
		theView->GetMouse(&cursor, &buttons);
		if (cursor.x > 30)
			x = cursor.x;
		else
			x = 30;
		if (cursor.y > 20)
			y = cursor.y;
		else
			y = 20;
		if (theView->WeAreAReplicant)
			theView->ResizeTo(x, y);
		else
			theView->Window()->ResizeTo(x, y);
		theView->Window()->Unlock();
		snooze(20 * 1000);
	}
	while (buttons);

	return 0;
}
