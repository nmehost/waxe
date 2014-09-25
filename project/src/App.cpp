#include <wx/wx.h>
#include <wx/image.h>

#include <stdio.h>

#define NEKO_COMPATIBLE
#ifndef STATIC_LINK
#define IMPLEMENT_API
#endif

#include <hx/CFFI.h>
#include "HaxeAPI.h"
#include "WaxeBinVersion.h"
#include <WaxeWorksVersion.h>


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


value wx_get_ndll_version()
{
   return alloc_int( WAXE_BINARY_VERSION );
}
DEFINE_PRIM(wx_get_ndll_version,0);

value wx_get_waxe_works_version()
{
   return alloc_string( WAXE_WORKS_VERSION );
}
DEFINE_PRIM(wx_get_waxe_works_version,0);

/*
 * Allow access to wxApp::s_macAboutMenuItemId
 * Needed for correct placement of the OSX About menu
 */
value wx_get_s_macAboutMenuItemId()
{
  return alloc_int( wxTheApp->s_macAboutMenuItemId );
}
DEFINE_PRIM(wx_get_s_macAboutMenuItemId,0);

value wx_set_s_macAboutMenuItemId(value inID)
{
  wxTheApp->s_macAboutMenuItemId = val_int(inID);
  return val_null;
}
DEFINE_PRIM(wx_set_s_macAboutMenuItemId,1);

int link_App() { return 0; }



