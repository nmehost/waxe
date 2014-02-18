#include "HaxeAPI.h"

value wx_text_ctrl_create(value inParams)
{
	CreationParams params(inParams);
   wxTextCtrl *window = new wxTextCtrl(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_text_ctrl_create,1)


WIN_PROPERTY(wx_text_ctrl,wxTextCtrl,value,GetValue,SetValue,Val2Str)
