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


#ifndef _DESKNOTEVIEW_H
#define _DESKNOTEVIEW_H

#include <Application.h>
#include <InterfaceKit.h>
#include <String.h>

#include "ColorMenuItem.h"
#include "DeskNoteApp.h"
#include "DeskNoteTextView.h"


#define DN_LAUNCH 'dnLN'
#define DN_COLOR 'dnCL'

const rgb_color palette[] = {
	{ 255, 221, 191, 255 },		// lightest red
	{ 255, 187, 128, 255 },		// light red
	{ 216, 158, 108, 255 },		// red
	{ 179, 83, 0, 255 },		// dark red

	{ 255, 244, 191, 255 },		// lightest yellow
	{ 255, 233, 128, 255 },		// light yellow
	{ 215, 178, 0, 255 },		// yellow
	{ 179, 148, 0, 255 },		// dark yellow

	{ 216, 255, 180, 255 },		// lightest green
	{ 177, 255, 169, 255 },		// light green
	{ 72, 187, 61, 255 },		// green
	{ 47, 122, 40, 255 },		// dark green

	{ 213, 234, 255, 255 },		// lightest blue
	{ 170, 213, 255, 255 },		// light blue
	{ 67, 131, 196, 255 },		// blue
	{ 36, 71, 106, 255 },		// dark blue

	{ 244, 244, 244, 255 },		// off white
	{ 188, 188, 188, 255 },		// light grey
	{ 96, 96, 96, 255 },		// dark grey
	{ 11, 11, 11, 255 },		// off black
};

class DeskNoteTextView;

class DeskNoteView : public BView {
public:
	DeskNoteView(BRect rect);
	DeskNoteView(BMessage* data);
	~DeskNoteView();

	virtual status_t 	Archive(BMessage* data, bool deep = true) const;
	virtual void 		Draw(BRect rect);
	virtual void 		MessageReceived(BMessage* msg);
	virtual void 		FrameResized(float width, float height);
	virtual void 		MouseDown(BPoint point);
	virtual void 		SaveNote(BMessage* msg);
	virtual void 		RestoreNote(BMessage* msg);

	static BArchivable* Instantiate(BMessage* data);
	static int32 		ResizeViewMethod(void* data);

	static const char 	defaultText[];
	static const char 	aboutText[];

private:
	void				_BuildColorMenu(BMenu* menu);
	void 				_SetColors();
	void 				_ShowContextMenu(BPoint where);
	bool 				WeAreAReplicant;
	DeskNoteTextView*	textView;
	BScrollView*		fScrollView;
	BRect 				ourSize;
	BMenu*				colorMenu;
	BMessage* 			orginalSettings;
	BPopUpMenu* 		popupMenu;
	rgb_color 			background;
	rgb_color 			foreground;
	rgb_color 			widgetcolour;
	BDragger* 			dragger;
};

#endif // _DESKNOTEVIEW_H
