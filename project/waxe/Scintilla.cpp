#include "HaxeAPI.h"
#include <wx/stc/stc.h>

value wx_scintilla_create(value inParams)
{
   CreationParams params(inParams);
   wxStyledTextCtrl *window = new wxStyledTextCtrl(params.parent,params.id,
                 params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_scintilla_create,1)

value wx_scintilla_set_value(value inSci, value inString)
{
   wxStyledTextCtrl *sci;
   if (ValueToWX(inSci, sci))
   {
      sci->SetText( Val2Str(inString) );
   }
   return alloc_null();
}
DEFINE_PRIM(wx_scintilla_set_value,2)


value wx_scintilla_get_value(value inSci)
{
   wxStyledTextCtrl *sci;
   if (ValueToWX(inSci, sci))
      return WXToValue( sci->GetText( ) );

   return alloc_null();
}
DEFINE_PRIM(wx_scintilla_get_value,1)


//WIN_PROPERTY(wx_scintilla,wxStyledTextCtrl,label,GetLabel,SetLabel,Val2Str)

