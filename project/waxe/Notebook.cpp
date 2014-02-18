#include "HaxeAPI.h"
#include <wx/notebook.h>

value wx_notebook_create(value inParams)
{
	CreationParams params(inParams);
   wxNotebook *window = new wxNotebook(params.parent,params.id,
										  params.position,params.size,params.flags);

   return WXToValue(window);
}
DEFINE_PRIM(wx_notebook_create,1)


value wx_notebook_add_page(value inNotebook, value inPage,value inText,value inSelect, value inImage)
{
    wxNotebook *book=0;
    wxWindow   *page=0;

    if ( ValueToWX(inNotebook,book) && ValueToWX(inPage,page) )
    {
       return alloc_bool(book->AddPage(page, Val2Str(inText), val_bool(inSelect), val_int(inImage) ) );
    }
    return alloc_bool(false);

}
DEFINE_PRIM(wx_notebook_add_page,5)
