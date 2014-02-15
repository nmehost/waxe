#include "HaxeAPI.h"

value wx_panel_create(value inParams)
{
	CreationParams params(inParams);
   wxPanel *window = new wxPanel(params.parent,params.id,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_panel_create,1)

