
// The DeskNoteTextView header

#ifndef _DESKNOTETEXTVIEW_H
#define _DESKNOTETEXTVIEW_H

#include <InterfaceKit.h>
#include <String.h>
#include <Application.h>

#include "DeskNoteApp.h"


class DeskNoteTextView : public BTextView {
public:
						DeskNoteTextView (BRect textViewRect, const char *name,
							BRect textRect, uint32 resizingMode, uint32 flags);
						DeskNoteTextView (BMessage *data);

	virtual status_t 	Archive (BMessage *data, bool deep = true) const;
	void 				MouseDown (BPoint point);
	virtual				void KeyDown(const char *bytes, int32 numBytes);

	static BArchivable*	Instantiate (BMessage *data);

private:
	BString 			strId;
};

#endif // _DESKNOTETEXTVIEW_H

