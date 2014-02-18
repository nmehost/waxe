#include <wx/wx.h>
#include <wx/image.h>

#include <stdio.h>

#define NEKO_COMPATIBLE
#ifndef STATIC_LINK
#define IMPLEMENT_API
#endif

#include <hx/CFFI.h>
#include "HaxeAPI.h"

// --- Bootstrap Object ---------------------------

static value sgOnInit;

class wxWidgetsApp : public wxApp
{
public:
    wxWidgetsApp()
    {
    }
    virtual ~wxWidgetsApp() { }
    virtual bool OnInit()
    {
       wxImage::AddHandler(new wxGIFHandler);
       //wxImage::AddHandler(new wxBMPHandler);

       val_call0(sgOnInit);

       SetExitOnFrameDelete(true);

       return true;
    }

};

IMPLEMENT_APP_NO_MAIN(wxWidgetsApp)


// --- Interface ---------------------------------

value wx_boot(value inOnInit)
{
  sgOnInit = inOnInit;
  wchar_t *prog[] = { (wchar_t *)0};
  int argc = 0;
  int result =  wxEntry(argc,prog);
  return alloc_int(result);
}
DEFINE_PRIM(wx_boot,1)

value wx_quit()
{
   wxTheApp->ExitMainLoop();
   return val_null;
}
DEFINE_PRIM(wx_quit,0)

value wx_set_top_window(value inWindow)
{
	wxTopLevelWindow *window;
	if (ValueToWX(inWindow,window))
   	wxTheApp->SetTopWindow(window);
   return val_null;
}
DEFINE_PRIM(wx_set_top_window,1)


// Reference this to bring in all the symbols for the static library
int link_Bitmap();
int link_Brush();
int link_Button();
int link_CheckBox();
int link_Clay();
int link_ComboBox();
int link_DC();
int link_Dialog();
int link_Font();
int link_Frame();
int link_GLCanvas();
int link_HaxeAPI();
int link_ListBox();
int link_MDI();
int link_Menu();
int link_Notebook();
int link_Panel();
int link_Pen();
int link_Scintilla();
int link_Sizer();
int link_StaticBox();
int link_StaticText();
int link_TextCtrl();
int link_Timer();
int link_Window();


extern "C" int waxe_register_prims()
{
   #ifdef STATIC_LINK
   link_Bitmap();
   link_Brush();
   link_Button();
   link_CheckBox();
   link_Clay();
   link_ComboBox();
   link_DC();
   link_Dialog();
   link_Font();
   link_Frame();
   link_GLCanvas();
   link_HaxeAPI();
   link_ListBox();
   link_MDI();
   link_Menu();
   link_Notebook();
   link_Panel();
   link_Pen();
   link_Scintilla();
   link_Sizer();
   link_StaticBox();
   link_StaticText();
   link_TextCtrl();
   link_Timer();
   link_Window();
   #endif
   return 0;
}


