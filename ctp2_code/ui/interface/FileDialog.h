
#ifndef FILE_DIALOG_H__
#define FILE_DIALOG_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_TextField;

#define k_FILE_DIALOG_SAVE 1
#define k_FILE_DIALOG_LOAD 2
#define k_FILE_DIALOG_CANCEL 3

enum FILE_DIALOG_MODE {
	FILE_DIALOG_LOAD,
	FILE_DIALOG_SAVE
};

class FileDialog;
class aui_Control;

typedef void (FileDialogCallback)(FileDialog *dialog, uint32 action, const MBCHAR *path, void *cookie);

class FileDialog {
  private:
	ctp2_Window *m_window;
	ctp2_ListBox *m_list;
	MBCHAR m_dirPath[_MAX_PATH];
	FILE_DIALOG_MODE m_mode;
	ctp2_TextField *m_field;

	FileDialogCallback *m_callback;
	void *m_cookie;

  public:
	FileDialog();
	~FileDialog();

	void Open(FILE_DIALOG_MODE mode, FileDialogCallback *cb, void *cookie, const MBCHAR *dirPath);
	void Close();

	void AddFile(const MBCHAR *path, void *cookie);
	void Fill();

	const MBCHAR *GetSelectedFile();

	static void LoadCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SaveCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void NameCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

#endif
