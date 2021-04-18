/*
 * Original copyright 2000 by Colin Stewart (http://www.owlfish.com/).
 * All rights reserved.
 * Distributed under the terms of the BSD (3-clause) License.
 *
 * Authors:
 *		Janus2, 2015
 *
 */


#include "DeskNoteApp.h"

int main() {
	DeskNoteApp* myApp;
	myApp = new DeskNoteApp();
	myApp->Run();
	delete myApp;
	return 0;
}


const char DeskNoteApp::header[] = "DESKNOTE";
const char* app_signature = "application/x-vnd.cms103-DeskNotes";

DeskNoteApp::DeskNoteApp()
	:
	BApplication(app_signature)
{
	BRect rect, scrRect;
	BPath dir;
	BFile* file;
	BMessage* msg;
	BScreen* currentScreen;
	bool SettingsOK = true;
	int fileVer;
	char* buffer = new char[1024];

	find_directory(B_USER_SETTINGS_DIRECTORY, &dir);
	dir.Append("DeskNotes Settings");
	file = new BFile(dir.Path(), B_READ_ONLY);
	msg = new BMessage();

	if (file->InitCheck() == B_OK) {
		if (file->Read(buffer, strlen(header)) != (ssize_t)strlen(header))
			SettingsOK = false;

		if (strcasecmp(header, buffer) != 0)
			SettingsOK = false;

		file->Read(&fileVer, sizeof(NotesVersion));
		if (fileVer != NotesVersion)
			SettingsOK = false;
		msg->Unflatten(file);

	} else
		SettingsOK = false;

	delete file;
	delete buffer;

	if (SettingsOK) {
		currentScreen = new BScreen();	// This locks the screen.
		scrRect = currentScreen->Frame(); // Find out how big the screen is.
		delete currentScreen;	// Delete the object, unlock the screen.
		msg->FindRect("windowPos", &rect);
		if (rect.right > scrRect.right) {
			// If the window would be off-screen then try correcting it...
			float width, scrWidth;
			scrWidth = scrRect.right - scrRect.left;
			width = rect.right - rect.left;
			if (width > scrWidth)
				width = scrWidth - 20;
			rect.right = scrRect.right - 2;
			rect.left = rect.right - width;
		}
		if (rect.bottom > scrRect.bottom) {
			// If the window would be off-screen then try correcting it...
			float height, scrHeight;
			scrHeight = scrRect.bottom - scrRect.top;
			height = rect.bottom - rect.top;
			if (height > scrHeight)
				height = scrHeight - 20;
			rect.bottom = scrRect.bottom - 2;
			rect.top = rect.bottom - height;
		}
	} else
		rect.Set(100, 80, 340, 260); // Inital size for the window.

	myNote = new DeskNoteWindow(rect);
	myNote->RestoreNote(msg);
	myNote->Show();
	delete msg;
}


bool DeskNoteApp::QuitRequested()
{
	BAlert* alert;
	BPath dir;
	BFile* file;
	BMessage* msg;
	int ver = NotesVersion;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &dir) == B_OK) {
		// This is where we save our notes position, size, and text content.
		dir.Append("DeskNotes Settings");
		file = new BFile(dir.Path(), B_READ_WRITE | B_CREATE_FILE);
		file->Write(header, strlen(header));
		file->Write(&ver, sizeof(ver));
		msg = getSettings();
		msg->Flatten(file);
		delete file;
		delete msg;

	} else {
		alert = new BAlert("DeskNotes", "Unable to save note!", "OK");
		alert->Go();
	}

	return true;
}


BMessage*
DeskNoteApp::getSettings()
{
	BMessage* msg = new BMessage();
	BRect rect;

	rect = myNote->Frame();
	msg->AddRect("windowPos", rect);
	myNote->SaveNote(msg);

	return msg;
}
