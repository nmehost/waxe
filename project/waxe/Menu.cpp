#include "HaxeAPI.h"

// --- MenuBar ----------------------------------------------

value wx_menu_bar_create()
{
   return WXToValue( new wxMenuBar() );
}

DEFINE_PRIM(wx_menu_bar_create,0);


value wx_menu_bar_append(value inMenuBar, value inMenu, value inTitle)
{
	wxMenuBar *menu_bar;
	wxMenu *menu;
	if (ValueToWX(inMenuBar,menu_bar) && ValueToWX(inMenu,menu) )
		menu_bar->Append(menu, Val2Str(inTitle) );
   return alloc_null();
}

DEFINE_PRIM(wx_menu_bar_append,3);


// --- Menu ----------------------------------------------

value wx_menu_create(value inTitle, value inFlags)
{
   wxMenu *menu =  new wxMenu( Val2Str(inTitle), val_int(inFlags) );
   return WXToValue( menu );
}

DEFINE_PRIM(wx_menu_create,2)

	
value wx_menu_append(value inMenu, value inID, value inLable, value inHelp, value inKind)
{
   wxMenu *menu;
	if (ValueToWX(inMenu,menu))
	{
		wxItemKind kind = wxITEM_NORMAL;
		switch(val_int(inKind))
		{
			case 0 : kind = wxITEM_NORMAL; break;
			case 1 : kind = wxITEM_SEPARATOR; break;
			case 2 : kind = wxITEM_CHECK; break;
			case 3 : kind = wxITEM_RADIO; break;
		}
		menu->Append(val_int(inID), Val2Str(inLable), Val2Str(inHelp), kind );
	}

   return alloc_null();
}

DEFINE_PRIM(wx_menu_append,5)


value wx_menu_append_separator(value inMenu)
{
   wxMenu *menu;
	if (ValueToWX(inMenu,menu))
		menu->AppendSeparator();
	return alloc_null();
}

DEFINE_PRIM(wx_menu_append_separator,1)
