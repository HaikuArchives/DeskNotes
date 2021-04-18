/*
 * Original copyright 2000 by Colin Stewart (http://www.owlfish.com/).
 * All rights reserved.
 * Distributed under the terms of the BSD (3-clause) License.
 *
 * Authors:
 *		Janus2, 2015
 *
 */


#ifndef _DESKNOTEAPP_H
#define _DESKNOTEAPP_H

#include "DeskNoteWindow.h"
#include <AppKit.h>
#include <StorageKit.h>
#include <String.h>

extern const char* app_signature;

class DeskNoteApp : public BApplication {
public:
						DeskNoteApp();
	virtual bool 		QuitRequested(void);

	static const char 	header[];
	static const int 	NotesVersion = 1;

private:
	virtual BMessage*	getSettings(void);
	DeskNoteWindow*		myNote;
};

#endif // _DESKNOTEAPP_H
