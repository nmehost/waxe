#include "HaxeAPI.h"

value wx_combo_box_create(value inParams,value inChoices)
{
	CreationParams params(inParams);
	wxArrayString choices;
	Val2ArrayString(inChoices,choices);
   wxComboBox *window = new wxComboBox(params.parent,params.id,params.text,
										  params.position,params.size,choices,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_combo_box_create,2)



