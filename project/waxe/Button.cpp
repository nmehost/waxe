#include "HaxeAPI.h"

value wx_button_create(value inParams)
{
	CreationParams params(inParams);
   wxButton *window = new wxButton(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_button_create,1)


WIN_PROPERTY(wx_button,wxButton,label,GetLabel,SetLabel,Val2Str)

