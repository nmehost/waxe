#include "HaxeAPI.h"

value wx_checkbox_create(value inParams)
{
	CreationParams params(inParams);
   wxCheckBox *window = new wxCheckBox(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_checkbox_create,1)

WIN_PROPERTY(wx_checkbox,wxCheckBox,checked,IsChecked,SetValue,Val2Bool)

