
class ctp2_Window;
class ctp2_ListBox;

class ProfileVar;

class ProfileEdit
{
  private:
	
	ctp2_Window *m_window;
	ctp2_ListBox *m_list;
	
  public:
	ProfileEdit(AUI_ERRCODE *err);
	~ProfileEdit();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();
	static AUI_ERRCODE Cleanup();
	void FillList();

	void SelectItem(ProfileVar *var);
	static void ListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void ToggleBoolVar(aui_Control *control, uint32 action, uint32 data, void *cookie);
    static void SetNumVar(aui_Control *control, uint32 action, uint32 data, void *cookie);

    static void CloseWindow(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void EnableMessages(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

	
