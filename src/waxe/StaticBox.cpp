#include "HaxeAPI.h"

value wx_static_box_create(value inParams)
{
	CreationParams params(inParams);
   wxStaticBox *window = new wxStaticBox(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_static_box_create,1)


