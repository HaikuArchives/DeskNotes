
// File: $RCSFile$
// Revision: $Revision: 1.1 $
// Date: $Date: 2011/03/20 21:57:39 $
#include "DeskNoteTextView.h"

DeskNoteTextView::DeskNoteTextView (BRect textViewRect, const char *name, BRect textRect,
		uint32 resizingMode, uint32 flags) : BTextView (textViewRect, name, textRect, resizingMode, flags)
{
	strId << real_time_clock_usecs();
}


DeskNoteTextView::DeskNoteTextView (BMessage *data) : BTextView (data)
{
	data -> FindString("strId", &strId);
}


status_t DeskNoteTextView::Archive (BMessage *data, bool deep) const
{
	BTextView::Archive (data, deep);
	data -> AddString ("class", "DeskNoteTextView");
	data -> AddString ("add_on", app_signature);
	data -> AddString ("strId", strId );
	return B_NO_ERROR;
}


void DeskNoteTextView::MouseDown (BPoint point)
{
	if (!Window () -> IsActive ()) Window () -> Activate (true);
	BPoint mousePoint;
	uint32 mouseButtons;
	GetMouse (&mousePoint, &mouseButtons, false);
	if (mouseButtons != B_SECONDARY_MOUSE_BUTTON)
		BTextView::MouseDown (point);
	Parent () -> MouseDown (point);
}


BArchivable * DeskNoteTextView::Instantiate (BMessage *data)
{
	if (!validate_instantiation(data, "DeskNoteTextView"))
		return NULL;
	return new DeskNoteTextView(data);
}


void DeskNoteTextView::KeyDown(const char *bytes, int32 numBytes)
{
	BTextView::KeyDown(bytes, numBytes);
	BString str;
	str << "/boot/var/DeskNote." << strId;
	BFile file(str.String(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (B_OK == file.InitCheck()) {
		file.Write(Text(), strlen(Text()));
	}
}

