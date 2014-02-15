#include "HaxeAPI.h"

value wx_mdi_parent_frame_create(value inParams)
{
   CreationParams params(inParams,wxDEFAULT_FRAME_STYLE);
   wxFrame *frame = new wxMDIParentFrame(params.parent,params.id,params.text,
                                         params.position,params.size,params.flags);

   return WXToValue(frame);
}

DEFINE_PRIM(wx_mdi_parent_frame_create,1)
