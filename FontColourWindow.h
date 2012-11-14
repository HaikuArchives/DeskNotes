
// The DeskNoteWindow header file.

#ifndef _FONTCOLOURWINDOW_H
#define _FONTCOLOURWINDOW_H

#include <InterfaceKit.h>


#define DN_COLOUR_MENU 'dnCM'
#define DN_COLOUR_CHANGE 'dnCH'
#define DN_PROPERTIES_REVERT 'dnRT'
#define DN_PROPERTIES_CLOSE 'dnCL'
#define DN_PROPERTIES_URGENT_CLOSE 'dnUC'


class FontColourWindow : public BWindow
{
	public:
		FontColourWindow (BRect rect, BMessenger *msg, BMessage *initial);
		void MessageReceived (BMessage *msg);
		~FontColourWindow ();

		bool QuitRequested ();

		static void CalculateWindowFrame (BRect *windowFrame, BRect parentFrame);

	private:
		BMessenger *messenger;
		BMessage *orginalSettings;
		int fontSize;
		font_family fontFamily;
		uint16 fontFace;
		rgb_color background, foreground;

		// The interface components go here.
		// The Font family
		BMenuBar *fontFamilyMenu;
		// The Font Face.  (Bold, Italic, etc.)
		BMenuBar *fontFaceMenu;
		// The font size.
		BMenuBar *fontSizeMenu;
		// Whether to change the foreground or background colours.
		BMenuBar *colourMenu;
		BPopUpMenu *colourPopupMenu;
		BMenuItem *backgroundColourItem;
		BMenuItem *foregroundColourItem;
		BMenuItem *currentSelection;
		BColorControl *colourControl;

		BStringView *title;
		BTextView *exampleText;

		BButton *revertButton;
};

#endif

