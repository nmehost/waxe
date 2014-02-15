#include "HaxeAPI.h"

value wx_frame_create(value inParams)
{
   CreationParams params(inParams,wxDEFAULT_FRAME_STYLE);
   wxFrame *frame = new wxFrame(params.parent,params.id,params.text,
                                params.position,params.size,params.flags);

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
   }
   return alloc_null();
}
DEFINE_PRIM(wx_frame_set_menu_bar,2)
