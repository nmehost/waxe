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

      #ifdef HX_WINDOWS
      if (sgContext==0)
         sgContext = new wxGLContext(window, (wxGLContext *)0);


      wglMakeCurrent(window->GetHDC(),sgContext->GetGLRC());
/*
      TODO - still some issues here

    // Hack to fake a resize to get wxwindows to render before a reszie
    int x,y;
    window->GetParent()->GetSize(&x, &y);
    window->GetParent()->SetSize(-1, -1, x + 1, y, wxSIZE_USE_EXISTING);
    window->GetParent()->SetSize(-1, -1, x, y, wxSIZE_USE_EXISTING);
    */
      #endif


   return WXToValue(window);
}
DEFINE_PRIM(wx_glcanvas_create,1)

value wx_glcanvas_make_current(value inCanvas)
{
	wxGLCanvas *canvas;
	if (ValueToWX(inCanvas,canvas))
   {
      if (sgContext==0)
         sgContext = new wxGLContext(canvas, (wxGLContext *)0);

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

int link_GLCanvas() { return 0; }

