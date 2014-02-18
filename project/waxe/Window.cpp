#include "HaxeAPI.h"

value wx_window_create(value inParams)
{
	CreationParams params(inParams);
   wxWindow *window = new wxWindow(params.parent,params.id,
										  params.position,params.size,params.flags);
   return WXToValue(window);
}
DEFINE_PRIM(wx_window_create,1)

value wx_window_fit(value inWindow)
{
   wxWindow *window;
   if (ValueToWX(inWindow,window))
       window->Fit();
   return alloc_null();
}
DEFINE_PRIM(wx_window_fit,1)

value wx_window_refresh(value inWindow)
{
   wxWindow *window;
   if (ValueToWX(inWindow,window))
       window->Refresh();
   return alloc_null();
}
DEFINE_PRIM(wx_window_refresh,1)

value wx_window_destroy(value inWindow)
{
   wxWindow *window;
   if (ValueToWX(inWindow,window))
       window->Destroy();
   return alloc_null();
}
DEFINE_PRIM(wx_window_destroy,1)



WIN_PROPERTY(wx_window,wxWindow,size,GetSize,SetSize,Val2Size)
WIN_PROPERTY(wx_window,wxWindow,client_size,GetClientSize,SetClientSize,Val2Size)
WIN_PROPERTY(wx_window,wxWindow,position,GetPosition,SetPosition,Val2Point)
WIN_PROPERTY(wx_window,wxWindow,sizer,GetSizer,SetSizer,Val2Sizer)
WIN_PROPERTY(wx_window,wxWindow,shown,IsShown,Show,Val2Bool)
WIN_PROPERTY(wx_window,wxWindow,bg_colour,GetBackgroundColour,SetBackgroundColour,Val2Colour)
WIN_PROPERTY(wx_window,wxWindow,name,GetName,SetName,Val2Str)
