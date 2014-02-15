#include "HaxeAPI.h"

value wx_static_text_create(value inParams)
{
	CreationParams params(inParams);
   wxStaticText *window = new wxStaticText(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_static_text_create,1)


WIN_PROPERTY(wx_static_text,wxStaticText,label,GetLabel,SetLabel,Val2Str)
