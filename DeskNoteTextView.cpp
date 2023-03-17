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


#include "DeskNoteTextView.h"


DeskNoteTextView::DeskNoteTextView(BRect textViewRect, const char* name,
	BRect textRect, uint32 resizingMode, uint32 flags)
	:
	BTextView(textViewRect, name, textRect, resizingMode, flags)
{
	strId << real_time_clock_usecs();
}


DeskNoteTextView::DeskNoteTextView(BMessage* data)
	:
	BTextView(data)
{
	data->FindString("strId", &strId);
}


status_t
DeskNoteTextView::Archive(BMessage* data, bool deep) const
{
	BTextView::Archive(data, deep);
	data->AddString("class", "DeskNoteTextView");
	data->AddString("add_on", app_signature);
	data->AddString("strId", strId);
	return B_NO_ERROR;
}


void
DeskNoteTextView::MessageReceived(BMessage* msg)
{
	if (msg->WasDropped()) {
		rgb_color* color;
		ssize_t size;
		if (msg->FindData("RGBColor", B_RGB_COLOR_TYPE,
				(const void **)&color, &size) == B_OK) {
			BMessenger messenger(Parent());
			BMessage message(DN_COLOR);
			message.AddData("color", B_RGB_COLOR_TYPE, color, sizeof(rgb_color));
			messenger.SendMessage(&message);
			return;
		}
	}
	BTextView::MessageReceived(msg);
}


void
DeskNoteTextView::MouseDown(BPoint point)
{
	if (!Window()->IsActive())
		Window()->Activate(true);
	BPoint mousePoint;
	uint32 mouseButtons;
	GetMouse(&mousePoint, &mouseButtons, false);
	if (mouseButtons != B_SECONDARY_MOUSE_BUTTON)
		BTextView::MouseDown(point);
	Parent()->Parent()->MouseDown(point);
}


BArchivable*
DeskNoteTextView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "DeskNoteTextView"))
		return NULL;
	return new DeskNoteTextView(data);
}
