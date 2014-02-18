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
#ifdef STATIC_LINK
//extern "C" int nme_oglexport_register_prims();
#endif

extern "C" int waxe_register_prims()
{
   #ifdef STATIC_LINK
   //nme_oglexport_register_prims();
   #endif
   return 0;
}


