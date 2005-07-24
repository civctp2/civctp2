
#include "c3.h"
#include "FileDialog.h"
#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "c3ui.h"
#include "ctp2_Static.h"
#include "ctp2_button.h"
#include "ctp2_textfield.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

static MBCHAR *s_block = "GenericFileDialog";
extern C3UI *g_c3ui;

FileDialog::FileDialog()
{
	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_block);
	Assert(m_window);
	
	m_list = (ctp2_ListBox *)aui_Ldl::GetObject(s_block, "List");
	Assert(m_list);

	m_field = (ctp2_TextField *)aui_Ldl::GetObject(s_block, "NameField");
	Assert(m_field);
	
	aui_Ldl::SetActionFuncAndCookie(s_block, "LoadButton", LoadCallback, this);
	aui_Ldl::SetActionFuncAndCookie(s_block, "SaveButton", SaveCallback, this);
	aui_Ldl::SetActionFuncAndCookie(s_block, "CancelButton", CancelCallback, this);
	aui_Ldl::SetActionFuncAndCookie(s_block, "NameField", NameCallback, this);
	aui_Ldl::SetActionFuncAndCookie(s_block, "List", ListCallback, this);
	m_mode = FILE_DIALOG_LOAD;
}

FileDialog::~FileDialog()
{
	if(m_window) {
		aui_Ldl::DeleteHierarchyFromRoot(s_block);
		m_window = NULL;
	}

	m_list = NULL;
}

void FileDialog::Open(FILE_DIALOG_MODE mode, FileDialogCallback *cb, void *cookie, const MBCHAR *dirPath)
{
	Assert(m_window);
	if(!m_window) return;

	m_callback = cb;
	_fullpath(m_dirPath, dirPath, _MAX_PATH);
	m_cookie = cookie;

	Fill();

	m_field->SetFieldText("");
	g_c3ui->AddWindow(m_window);
	m_window->Show();

	m_mode = mode;
	ctp2_Button *saveButt = (ctp2_Button *)aui_Ldl::GetObject(s_block, "SaveButton");
	ctp2_Button *loadButt = (ctp2_Button *)aui_Ldl::GetObject(s_block, "LoadButton");

	switch(mode) {
		case FILE_DIALOG_LOAD:
			saveButt->Hide();
			loadButt->Show();
			break;
		case FILE_DIALOG_SAVE:
			loadButt->Hide();
			saveButt->Show();
			break;
	}
}

void FileDialog::Close()
{
	if(!m_window)
		return;

	g_c3ui->RemoveWindow(m_window->Id());
}

void FileDialog::AddFile(const MBCHAR *path, void *cookie)
{
	Assert(m_list);
	if(!m_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("GenericFileItem");
	Assert(item);
	if(!item) return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);

	Assert(box);
	if(!box) return;

	box->SetText(path);
	
	item->SetUserData(cookie);
	m_list->AddItem(item);
}

void FileDialog::Fill()
{
	MBCHAR path[_MAX_PATH];
#ifdef WIN32
	HANDLE				lpFileList;
	WIN32_FIND_DATA		fileData;

	sprintf(path, "%s%s*.*", FILE_SEP, m_dirPath);

	lpFileList = FindFirstFile(path, &fileData);

	Assert(lpFileList != INVALID_HANDLE_VALUE);
	if(lpFileList == INVALID_HANDLE_VALUE) return;
#else
	DIR *dir = opendir(m_dirPath);
	if (!dir) return;
	struct dirent *dent = 0;
#endif
	m_list->Clear();
#ifdef WIN32
	do {

		if(!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			AddFile(fileData.cFileName, NULL);
		}
	} while(FindNextFile(lpFileList, &fileData));
	
	FindClose(lpFileList);
#else
	struct stat st;
	while ((dent = readdir(dir))) {
		snprintf(path, sizeof(path), "%s%s%s", m_dirPath, FILE_SEP, dent->d_name);
		int rc = stat(path, &st);
		if (!S_ISDIR(st.st_mode)) {
			AddFile(dent->d_name, NULL);
		}
	}
	closedir(dir);
#endif
}

const MBCHAR *FileDialog::GetSelectedFile()
{
	static MBCHAR file[_MAX_PATH];
	file[0] = 0;

	Assert(m_list);
	if(!m_list) return file;
	
	ctp2_ListItem *item = (ctp2_ListItem *)m_list->GetSelectedItem();
	if(item) {
		ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(box);
		if(box) {
			strcpy(file, box->GetText());
		}
	} else {
		Assert(m_field);
		if(!m_field) return file;
		
		m_field->GetFieldText(file, _MAX_PATH - 1);
	}
	return file;
}
	
void FileDialog::LoadCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	FileDialog *di = (FileDialog *)cookie;
	if(di->m_callback) {
		MBCHAR full[_MAX_PATH];
		sprintf(full, "%s%s%s", di->m_dirPath, FILE_SEP, di->GetSelectedFile());
		di->m_callback(di, k_FILE_DIALOG_LOAD, full, di->m_cookie);
	}
	di->Close();
}

void FileDialog::SaveCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	FileDialog *di = (FileDialog *)cookie;
	if(di->m_callback) {
		MBCHAR full[_MAX_PATH];
		sprintf(full, "%s%s%s", di->m_dirPath, FILE_SEP, di->GetSelectedFile());
		di->m_callback(di, k_FILE_DIALOG_SAVE, full, di->m_cookie);
	}
	di->Close();
}

void FileDialog::CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	FileDialog *di = (FileDialog *)cookie;
	if(di->m_callback) {
		di->m_callback(di, k_FILE_DIALOG_CANCEL, NULL, di->m_cookie);
	}
	di->Close();
				   
}

void FileDialog::ListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_LISTBOX_ACTION_SELECT && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		return;
	}

	FileDialog *di = (FileDialog *)cookie;
	Assert(di);
	if(!di) return;

	Assert(di->m_list);
	if(!di->m_list) return;

	ctp2_ListItem *item = (ctp2_ListItem *)di->m_list->GetSelectedItem();
	if(!item) return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
	Assert(box);
	if(!box) return;

	Assert(di->m_field);
	if(!di->m_field) return;

	di->m_field->SetFieldText(box->GetText());

	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		if(di->m_mode == FILE_DIALOG_LOAD) {
			LoadCallback(control, AUI_BUTTON_ACTION_EXECUTE, data, cookie);
		} else {
			SaveCallback(control, AUI_BUTTON_ACTION_EXECUTE, data, cookie);
		}
	}
}

void FileDialog::NameCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_TEXTFIELD_ACTION_EXECUTE) return;
	FileDialog *di = (FileDialog *)cookie;
	Assert(di);
	if(!di) return;

	Assert(di->m_list);
	if(!di->m_list) return;
	
	di->m_list->DeselectItem(di->m_list->GetSelectedItem());

	MBCHAR fieldText[_MAX_PATH];
	Assert(di->m_field);
	if(!di->m_field) return;

	di->m_field->GetFieldText(fieldText, _MAX_PATH);

	ctp2_ListItem *item = NULL;
	sint32 i;
	for(i = 0; i < di->m_list->NumItems(); i++) {
		item = (ctp2_ListItem *)di->m_list->GetItemByIndex(i);
		if(item) {
			ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
			if(box) {
				if(!stricmp(box->GetText(), fieldText)) {
					di->m_list->SelectItem(item);
					return;
				}
			}
		}
	}
}

