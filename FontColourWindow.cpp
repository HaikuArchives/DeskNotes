
// File: $RCSFile$
// Revision: $Revision: 1.1 $
// Date: $Date: 2011/03/20 21:57:39 $
#include "FontColourWindow.h"
// Bring the maths library in.

#include <math.h>
#include <MenuField.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>

FontColourWindow::FontColourWindow(
		BRect rect, BMessenger *msg, BMessage *initial)
	:
	BWindow (rect, "DeskNotes",
			B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS)
{
	// B_TITLED_WINDOW_LOOK
	messenger = new BMessenger (*msg);
	ssize_t dataSize;
	const void *dataPointer;

	// Take a copy of the inital values to restore to if we cancel.
	orginalSettings = new BMessage(*initial);
	// Now find out what font we start with, and what colour we use.
	initial -> FindData ("background_colour", B_RGB_COLOR_TYPE, &dataPointer, &dataSize);
	background = (*(rgb_color *)dataPointer);
	originalBackground = background;

	initial -> FindData ("foreground_colour", B_RGB_COLOR_TYPE, &dataPointer, &dataSize);
	foreground = (*(rgb_color *)dataPointer);
	originalForeground = foreground;

	initial -> FindData ("font_family_name", B_STRING_TYPE, &dataPointer, &dataSize);
	strncpy(fontFamily, (const char*)dataPointer, dataSize);

	initial -> FindData ("font_face", B_UINT16_TYPE, &dataPointer, &dataSize);
	fontFace = (*(uint16 *)dataPointer);

	colourPopupMenu = new BPopUpMenu ("Background Colour");
	backgroundColourItem = new BMenuItem ("Background Colour", new BMessage (DN_COLOUR_MENU));
	foregroundColourItem = new BMenuItem ("Foreground Colour",  new BMessage (DN_COLOUR_MENU));

	colourPopupMenu -> AddItem (backgroundColourItem);
	colourPopupMenu -> AddItem (foregroundColourItem);
	backgroundColourItem -> SetMarked(true);

	colourMenu = new BMenuField(rect, "", "", colourPopupMenu);

	colourMenu->SetDivider(0);
	colourPopupMenu -> SetTargetForItems (this);
	colourControl = new BColorControl (B_ORIGIN,
			B_CELLS_32x8, 8.0, "Colour Selector", new BMessage (DN_COLOUR_CHANGE));
	colourControl -> SetValue (background);

	defaultsButton = new BButton ("Defaults", "Defaults", new BMessage (DN_PROPERTIES_DEFAULTS));
	// Add the revert button, taking away the border added earlier.
	revertButton = new BButton ("Revert", "Revert", new BMessage (DN_PROPERTIES_REVERT));

	SetLayout(new BGroupLayout(B_VERTICAL));

	AddChild(BLayoutBuilder::Group<>(B_VERTICAL)
		.Add(colourMenu)
		.Add(colourControl)
		.AddGroup(B_HORIZONTAL)
			.Add(defaultsButton)
			.Add(revertButton)
			.AddGlue()
		.End()
		.SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING,
			B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
	);
	_CheckButtons();
}


bool FontColourWindow::IsDefaultsColor ()
{
	return background == kDefaultBackgroundColor
		&& foreground == kDefaultForegroundColor;
}


bool FontColourWindow::IsRevertableColor ()
{
	return background != originalBackground
		|| foreground != originalForeground;

}


void FontColourWindow::_CheckButtons()
{
	defaultsButton->SetEnabled(!IsDefaultsColor());
	revertButton->SetEnabled(IsRevertableColor());
}


void FontColourWindow::_UpdateColorControl()
{
	if (colourPopupMenu->FindMarked() == backgroundColourItem)
		colourControl -> SetValue (background);
	if (colourPopupMenu->FindMarked() == foregroundColourItem)
		colourControl -> SetValue (foreground);
}


void FontColourWindow::MessageReceived (BMessage *msg)
{
	void *pointer;
	BMessage *newMsg;

	if (msg->WasDropped()) {
		rgb_color* color = NULL;
		ssize_t size = 0;

		if (msg->FindData("RGBColor", (type_code)'RGBC', (const void**)&color,
				&size) == B_OK) {
			if (colourPopupMenu->FindMarked() == backgroundColourItem) {
				background = *color;
				colourControl -> SetValue (background);
			}
			if (colourPopupMenu->FindMarked() == foregroundColourItem) {
				foreground = *color;
				colourControl -> SetValue (foreground);
			}
			newMsg = new BMessage (orginalSettings->what);
			newMsg -> AddData ("background_colour",
					B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
			newMsg -> AddData ("foreground_colour",
					B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
			messenger -> SendMessage (newMsg);
			delete newMsg;
			_CheckButtons();
			return;
		}
	}

	switch (msg -> what) {
		case DN_COLOUR_MENU:
				if (colourPopupMenu->FindMarked() == backgroundColourItem)
					colourControl -> SetValue (background);
				if (colourPopupMenu->FindMarked() == foregroundColourItem)
					colourControl -> SetValue (foreground);
			break;

		case DN_COLOUR_CHANGE:
			newMsg = new BMessage (orginalSettings->what);
			if (colourPopupMenu->FindMarked() == backgroundColourItem)
				background = colourControl -> ValueAsColor();
			if (colourPopupMenu->FindMarked() == foregroundColourItem)
				foreground = colourControl -> ValueAsColor();
			newMsg -> AddData ("background_colour",
					B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
			newMsg -> AddData ("foreground_colour",
					B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
			messenger -> SendMessage (newMsg);
			delete newMsg;
			_CheckButtons();
			break;

		case DN_PROPERTIES_REVERT:
			// Restore the original settings.
			messenger -> SendMessage (orginalSettings);
			background = originalBackground;
			foreground = originalForeground;
			_UpdateColorControl();
			_CheckButtons();
			break;

		case DN_PROPERTIES_DEFAULTS: {
			// Restore the default settings.
			BMessage defaultSettings(*orginalSettings);
			background = kDefaultBackgroundColor;
			foreground = kDefaultForegroundColor;
			defaultSettings.ReplaceData ("background_colour", B_RGB_COLOR_TYPE, &background, sizeof (rgb_color));
			defaultSettings.ReplaceData ("foreground_colour", B_RGB_COLOR_TYPE, &foreground, sizeof (rgb_color));
			messenger -> SendMessage (&defaultSettings);
			_UpdateColorControl();
			_CheckButtons();
			break;
		}

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


FontColourWindow::~FontColourWindow ()
{
	delete messenger;
	delete orginalSettings;
}


bool FontColourWindow::QuitRequested ()
{
	BMessage *newMsg;
	newMsg = new BMessage (DN_PROPERTIES_CLOSE);
	messenger -> SendMessage (newMsg);
	delete newMsg;
	return true;
}


void FontColourWindow::CalculateWindowFrame(BRect *windowFrame, BRect parentFrame)
{
	BRect screenSize;
	BScreen *currentScreen;
	int width = 271;
	int height = 115;

	currentScreen = new BScreen();
	screenSize = currentScreen -> Frame();		// Find out how big the screen is.
	delete currentScreen;						// Delete the object, unlock the screen.

	windowFrame -> Set (parentFrame.left, parentFrame.top,
						parentFrame.left + width, parentFrame.top + height);
	// Ideal placing is just above and to the right of the replicant handle.
	windowFrame -> OffsetBy (30, -30 + (parentFrame.bottom - parentFrame.top));

	// If we are off the top of the screen we need to knock it down a bit.
	if (windowFrame -> top < screenSize.top + 17) {
		windowFrame -> OffsetBy (0, screenSize.top - windowFrame -> top + 17);
	}

	// If we are off the bottom of the screen we need to drag it back up.
	if (windowFrame -> bottom + 5 > screenSize.bottom) {
		windowFrame -> OffsetBy(0, (screenSize.bottom - windowFrame -> bottom) - 5);
	}
	if (windowFrame -> right > screenSize.right) {
		windowFrame -> OffsetBy ((screenSize.right - windowFrame -> right) - 5, 0);
	}
}

