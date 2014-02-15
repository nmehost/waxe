#include "HaxeAPI.h"
#include <wx/gbsizer.h>


wxSizer *Val2Sizer(value inVal)
{
	if (val_is_null(inVal))
		return 0;

	wxSizer *sizer;
	ValueToWX(inVal,sizer);
	return sizer;
}


value wx_sizer_create_box(value inVertical)
{
	wxSizer *sizer =  new wxBoxSizer( val_bool(inVertical) ? wxVERTICAL : wxHORIZONTAL );
	return WXToValue(sizer);
}
DEFINE_PRIM(wx_sizer_create_box,1)

value wx_sizer_create_grid(value rows,value cols, value vgap, value hgap)
{
	if (val_is_null(rows))
	  return WXToValue(new wxGridSizer(val_int(cols),val_int(vgap),val_int(hgap)));
	return WXToValue(new wxGridSizer(val_int(rows),val_int(cols),val_int(vgap),val_int(hgap)));
}
DEFINE_PRIM(wx_sizer_create_grid,4)

value wx_sizer_create_flex_grid(value rows,value cols, value vgap, value hgap)
{
	if (val_is_null(rows))
	  return WXToValue(new wxFlexGridSizer(val_int(cols),val_int(vgap),val_int(hgap)));
	return WXToValue(new wxFlexGridSizer(val_int(rows),val_int(cols),val_int(vgap),val_int(hgap)));
}
DEFINE_PRIM(wx_sizer_create_flex_grid,4)

value wx_sizer_create_grid_bag(value vgap, value hgap)
{
	return WXToValue(new wxGridBagSizer(val_int(vgap),val_int(hgap)));
}
DEFINE_PRIM(wx_sizer_create_grid_bag,2)

value wx_sizer_set_size_hints(value inSizer,value inWindow)
{
   wxWindow *window = 0;
   wxSizer *sizer = Val2Sizer(inSizer);
   if (sizer && ValueToWX(inWindow,window))
       sizer->SetSizeHints(window);
	return alloc_null();
}
DEFINE_PRIM(wx_sizer_set_size_hints,2)


value wx_sizer_fit(value inSizer,value inWindow)
{
   wxWindow *window = 0;
   wxSizer *sizer = Val2Sizer(inSizer);
   if (sizer && ValueToWX(inWindow,window))
       sizer->Fit(window);
	return alloc_null();
}
DEFINE_PRIM(wx_sizer_fit,2)


value wx_sizer_add_spacer(value inSizer,value inSize, value inProportion)
{
   wxSizer *sizer = Val2Sizer(inSizer);
   if (sizer)
   {
      int size = val_int(inSize);
      int prop = val_int(inProportion);
      if (size==0)
         sizer->AddStretchSpacer(prop);
      else
         sizer->AddSpacer(size);
   }
	return alloc_null();
}
DEFINE_PRIM(wx_sizer_add_spacer,3)





value wx_sizer_add(value inHandle,value inSizerOrWindow, value inProportion, value inFlags,value inBorder)
{
	wxSizer *parent;
	if (ValueToWX(inHandle,parent))
	{
		int prop = val_int(inProportion);
		int flags = val_int(inFlags);
		int border = val_int(inBorder);

		wxSizer *sizer;
		wxWindow *window;
		if (ValueToWX(inSizerOrWindow,sizer))
		{
			parent->Add(sizer,prop,flags,border);
		}
		else if (ValueToWX(inSizerOrWindow,window))
		{
			parent->Add(window,prop,flags,border);
		}
		else
			val_throw(alloc_string("Invalid Sizer Child"));
	}
	return alloc_null();
}
DEFINE_PRIM(wx_sizer_add,5)


value wx_sizer_add_growable_row(value inHandle,value inRow, value inProportion)
{
	wxFlexGridSizer *sizer;
	if (ValueToWX(inHandle,sizer))
		sizer->AddGrowableRow(val_int(inRow),val_int(inProportion));

	return alloc_null();
}
DEFINE_PRIM(wx_sizer_add_growable_row,3)

value wx_sizer_add_growable_col(value inHandle,value inCol, value inProportion)
{
	wxFlexGridSizer *sizer;
	if (ValueToWX(inHandle,sizer))
		sizer->AddGrowableCol(val_int(inCol),val_int(inProportion));

	return alloc_null();
}
DEFINE_PRIM(wx_sizer_add_growable_col,3)

value wx_sizer_create_from_static_box(value inBox, value inVertical)
{
   wxStaticBox *box=0;
   if (ValueToWX(inBox,box))
      return WXToValue(new wxStaticBoxSizer(box, (val_bool(inVertical) ? wxVERTICAL : wxHORIZONTAL )));
   return alloc_null();
}
DEFINE_PRIM(wx_sizer_create_from_static_box,2)

value wx_sizer_create_static_box(value inVertical, value inParent, value inTitle)
{
   wxWindow *parent=0;
   if (ValueToWX(inParent,parent))
      return WXToValue(new wxStaticBoxSizer(val_bool(inVertical) ? wxVERTICAL : wxHORIZONTAL, parent, Val2Str(inTitle)));
   return alloc_null();
}
DEFINE_PRIM(wx_sizer_create_static_box,3)

