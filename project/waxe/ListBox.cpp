#include "HaxeAPI.h"

value wx_list_box_create(value inParams,value inChoices)
{
	CreationParams params(inParams);
	wxArrayString choices;
	Val2ArrayString(inChoices,choices);
   wxListBox *window = new wxListBox(params.parent,params.id,
              params.position,params.size,choices,params.flags,wxDefaultValidator,params.text);

   return WXToValue(window);
}
DEFINE_PRIM(wx_list_box_create,2)


WIN_PROPERTY(wx_list_box,wxListBox,selection,GetSelection,SetSelection,Val2Int)
WIN_PROPERTY_IDX(wx_list_box,wxListBox,string,GetString,SetString,Val2Str)

