#include "HaxeAPI.h"

value wx_frame_create(value inParams)
{
   CreationParams params(inParams,wxDEFAULT_FRAME_STYLE);
   wxFrame *frame = new wxFrame(params.parent,params.id,params.text,
                                params.position,params.size,params.flags);
   if (!params.parent && params.size!=wxDefaultSize)
   {
      // Adjust for borders
      int w = 0;
      int h = 0;
      frame->GetClientSize(&w,&h);
      int dx = params.size.x - w;
      int dy = params.size.y - h;
      if ( dx!=0 || dy!=0)
         frame->SetSize( params.size.x + dx, params.size.y+dy );
   }


   return WXToValue(frame);
}

DEFINE_PRIM(wx_frame_create,1)

value wx_frame_set_menu_bar(value inFrame, value inBar)
{
   wxFrame *frame;
   if (ValueToWX(inFrame,frame))
   {
      wxMenuBar *bar;
      ValueToWX(inBar,bar);
      frame->SetMenuBar(bar);
      // TODO - adjust size?
   }
   return alloc_null();
}
DEFINE_PRIM(wx_frame_set_menu_bar,2)

int link_Frame() { return 0; }

