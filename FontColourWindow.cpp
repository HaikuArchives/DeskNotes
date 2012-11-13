
// File: $RCSFile$
// Revision: $Revision: 1.1 $
// Date: $Date: 2011/03/20 21:57:39 $
#include "FontColourWindow.h"
// Bring the maths library in.
#include <math.h>

FontColourWindow::FontColourWindow (BRect rect, BMessenger *msg, BMessage *initial):BWindow (rect,
	"DeskNotes", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE) {
	// B_TITLED_WINDOW_LOOK 
	BRect viewSize, otherSize;
	font_height fntHeight;
	int y;
	messenger = new BMessenger (*msg);
	BMessage *modelMsg;
	const char *pointer;
	ssize_t dataSize;
	const void *dataPointer;
	
	// Take a copy of the inital values to restore to if we cancel.
	orginalSettings = new BMessage (*initial);
	be_plain_font -> GetHeight (&fntHeight);
	y = (int)ceil (fntHeight.ascent + fntHeight.descent + fntHeight.leading) + 2;
	viewSize.Set (0, 0 , (rect.right - rect.left), (int)ceil (fntHeight.ascent) + 2);
	initial -> FindString ("title", &pointer);
	title = new BStringView (viewSize, "The Title", pointer);
	title -> SetAlignment (B_ALIGN_CENTER);
	AddChild (title);
	
	// Now find out what font we start with, and what colour we use.
	initial -> FindData ("background_colour", B_RGB_COLOR_TYPE, &dataPointer, &dataSize);
	background = (*(rgb_color *)dataPointer);

	initial -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, &dataPointer, &dataSize);
	foreground = (*(rgb_color *)dataPointer);
	
	initial -> FindData ("font_family_name", B_STRING_TYPE, &dataPointer, &dataSize);
	strncpy(fontFamily, (const char*)dataPointer, dataSize);
	
	initial -> FindData ("font_face", B_UINT16_TYPE, &dataPointer, &dataSize);
	fontFace = (*(uint16 *)dataPointer);

	viewSize.OffsetBy (3, viewSize.bottom + 2);
	colourMenu = new BMenuBar (viewSize, "Colour Menu", 0);
	colourPopupMenu = new BPopUpMenu ("Background Colour");
	modelMsg = new BMessage (DN_COLOUR_MENU);
	backgroundColourItem = new BMenuItem ("Background Colour", modelMsg);
	modelMsg = new BMessage (DN_COLOUR_MENU);
	foregroundColourItem = new BMenuItem ("Foreground Colour", modelMsg);
	currentSelection = backgroundColourItem;
	colourPopupMenu -> AddItem (backgroundColourItem);
	colourPopupMenu -> AddItem (foregroundColourItem);
	colourPopupMenu -> SetTargetForItems (this);
	colourMenu -> AddItem (colourPopupMenu);
	AddChild (colourMenu);
	otherSize = colourMenu -> Bounds();
	viewSize.OffsetBy (0,otherSize.bottom - otherSize.top + 2);
	modelMsg = new BMessage (DN_COLOUR_CHANGE);
	colourControl = new BColorControl (viewSize.LeftTop (), B_CELLS_32x8, 2, "Colour Selector", modelMsg);
	AddChild (colourControl);
	colourControl -> SetValue (background);
	
	// Add the revert button, taking away the border added earlier.
	viewSize.OffsetTo (-3, colourControl -> Frame().bottom + 2);
	viewSize.left += 70;
	viewSize.right -= 70;
	modelMsg = new BMessage (DN_PROPERTIES_REVERT);
	revertButton = new BButton (viewSize, "Revert", "Revert", modelMsg);
	AddChild (revertButton);
}

void FontColourWindow::MessageReceived (BMessage *msg) {
	void *pointer;
	BMessage *newMsg;
	switch (msg -> what) {
		case DN_COLOUR_MENU:
			msg -> FindPointer ("source", &pointer);
			if ((BMenuItem *) pointer != currentSelection) {
				currentSelection = (BMenuItem *) pointer;
				if (currentSelection == backgroundColourItem)
					colourControl -> SetValue (background);
				if (currentSelection == foregroundColourItem)
					colourControl -> SetValue (foreground);
			}		
			break;
		case DN_COLOUR_CHANGE:
			newMsg = new BMessage (orginalSettings->what);
			if (currentSelection == backgroundColourItem)
				background = colourControl -> ValueAsColor();
			if (currentSelection == foregroundColourItem)
				foreground = colourControl -> ValueAsColor();
			newMsg -> AddData ("background_colour", B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
			newMsg -> AddData ("foreground_colour", B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
			messenger -> SendMessage (newMsg);
			delete newMsg;
			break;
		case DN_PROPERTIES_REVERT:
			// Restore the original settings.
			messenger -> SendMessage (orginalSettings);
			break;
		case DN_PROPERTIES_URGENT_CLOSE:
			// We need to close quickly - no messages should be sent back.
			this -> Lock();
			this -> Quit();
			break;
		default:
			BWindow::MessageReceived (msg);
			break;
	}
}

FontColourWindow::~FontColourWindow () {
	delete messenger;
	delete orginalSettings;
}

bool FontColourWindow::QuitRequested () {
	BMessage *newMsg;
	newMsg = new BMessage (DN_PROPERTIES_CLOSE);
    messenger -> SendMessage (newMsg);
    delete newMsg;
	return true;
}

void FontColourWindow::CalculateWindowFrame (BRect *windowFrame ,BRect parentFrame)
{
	BRect screenSize;
	BScreen *currentScreen;
	int width = 271;
	int height = 115;
	
	currentScreen = new BScreen();
	screenSize = currentScreen -> Frame();		// Find out how big the screen is.
	delete currentScreen;						// Delete the object, unlock the screen.
	
	windowFrame -> Set (parentFrame.left
					   ,parentFrame.top
					   ,parentFrame.left + width
					   ,parentFrame.top + height);
	// Ideal placing is just above and to the right of the replicant handle.
	windowFrame -> OffsetBy (30,-30 + (parentFrame.bottom - parentFrame.top));
	
	// If we are off the top of the screen we need to knock it down a bit.
	if (windowFrame -> top < screenSize.top + 17) {
		windowFrame -> OffsetBy (0, screenSize.top - windowFrame -> top + 17);
	}
	
	// If we are off the bottom of the screen we need to drag it back up.
	if (windowFrame -> bottom + 5 > screenSize.bottom) {
		windowFrame -> OffsetBy (0, (screenSize.bottom - windowFrame -> bottom) - 5);
	}
	if (windowFrame -> right > screenSize.right) {
		windowFrame -> OffsetBy ((screenSize.right - windowFrame -> right) - 5, 0);
	}
}

	
