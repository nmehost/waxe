#include "HaxeAPI.h"
#include <wx/glcanvas.h>

// TODO: Manage this with in a window class...
wxGLContext *sgContext = 0;

value wx_glcanvas_create(value inParams)
{
	CreationParams params(inParams);
    int *attrs = 0;
    wxGLCanvas *window = new wxGLCanvas(params.parent,params.id, attrs,
			  params.position,params.size,params.flags);

    if (sgContext==0)
       sgContext = new wxGLContext(window, (wxGLContext *)0);

   #ifdef HX_WINDOWS
   wglMakeCurrent(window->GetHDC(),sgContext->GetGLRC());
   #endif

    int x,y;
    // Hack to fake a resize to get wxwindows to render before a reszie
    //window->GetParent()->GetSize(&x, &y);
    //window->GetParent()->SetSize(-1, -1, x + 1, y, wxSIZE_USE_EXISTING);
    //window->GetParent()->SetSize(-1, -1, x, y, wxSIZE_USE_EXISTING);

   return WXToValue(window);
}
DEFINE_PRIM(wx_glcanvas_create,1)

value wx_glcanvas_make_current(value inCanvas)
{
	wxGLCanvas *canvas;
	if (ValueToWX(inCanvas,canvas))
   {
		canvas->SetCurrent(*sgContext);
   }
	return alloc_null();
}
DEFINE_PRIM(wx_glcanvas_make_current,1)

value wx_glcanvas_flip(value inCanvas)
{
	wxGLCanvas *canvas;
	if (ValueToWX(inCanvas,canvas))
		canvas->SwapBuffers();
	return alloc_null();
}
DEFINE_PRIM(wx_glcanvas_flip,1)

