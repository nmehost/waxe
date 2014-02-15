#include "HaxeAPI.h"
#include "clay/Layout.h"
#include "clay/ToolBox.h"

using namespace clay;

value wx_clay_man_create(value inFrame, value inOwner)
{
   wxFrame *frame;
   if (! ValueToWX(inFrame,frame) )
      return alloc_null();

   Manager *manager = new Manager(frame);

   manager->mData.SetClientObject( new HaxeData(inOwner) );

   return WXToValue(manager);
}

DEFINE_PRIM(wx_clay_man_create,2)



value wx_clay_man_init_root_container(value inManager, value inOwner)
{
   Manager *manager;
   if (! ValueToWX(inManager,manager) )
      return alloc_null();

   manager->GetRootContainer()->mData.SetClientObject( new HaxeData(inOwner) );
   return WXToValue(manager->GetRootContainer());
}
DEFINE_PRIM(wx_clay_man_init_root_container,2)


value wx_clay_man_get_root(value inManager)
{
   Manager *manager;
   if (! ValueToWX(inManager,manager) )
      return alloc_null();

   return dynamic_cast<HaxeData *>(manager->GetRootContainer()->mData.GetClientObject())->GetHaxeObject();
}
DEFINE_PRIM(wx_clay_man_get_root,1)


// --- Container ----------------------------------------------


value wx_container_add_window(value *arg, int argc)
{
   enum { aParent, aWindow, aNewContainer, aWhere, aFlags, aSIZE };

   Container *parent;
   wxWindow    *window;

   if (! ValueToWX(arg[aParent],parent) )
      return alloc_null();
   
   if (! ValueToWX(arg[aWindow],window) )
      return alloc_null();

   Container *c = 0;

   ToolBox *toolbox;
   if (ValueToWX(arg[aWindow],toolbox) )
      c = parent->Add(toolbox, (AddPosition)val_int(arg[aWhere]) );
   else
      c = parent->Add(window, (AddPosition)val_int(arg[aWhere]), wxIconBundle(), val_int(arg[aFlags]), wsNormal );

   c->mData.SetClientObject( new HaxeData(arg[aNewContainer]) );
   return WXToValue(c);
}
DEFINE_PRIM_MULT(wx_container_add_window)


value wx_container_get_width(value inContainer, value inWhich)
{
   Container *container;
   if ( ValueToWX(inContainer,container) )
      return alloc_int(container->GetSize((SizeType)val_int(inWhich)).x);
   return alloc_null();
}
DEFINE_PRIM(wx_container_get_width,2)


value wx_container_set_width(value inContainer, value inWhich, value inW)
{
   Container *container;
   if ( ValueToWX(inContainer,container) )
      container->SetWidth(val_int(inW),(SizeType)val_int(inWhich));
   return alloc_null();
}
DEFINE_PRIM(wx_container_set_width,3)


value wx_container_get_height(value inContainer, value inWhich)
{
   Container *container;
   if ( ValueToWX(inContainer,container) )
      return alloc_int(container->GetSize((SizeType)val_int(inWhich)).y);
   return alloc_null();
}
DEFINE_PRIM(wx_container_get_height,2)


value wx_container_set_height(value inContainer, value inWhich, value inW)
{
   Container *container;
   if ( ValueToWX(inContainer,container) )
      container->SetHeight(val_int(inW),(SizeType)val_int(inWhich));
   return alloc_null();
}
DEFINE_PRIM(wx_container_set_height,3)



// --- Toolbox ----------------------------------------------


value wx_toolbox_create(value inParent, value inName, value inPos, value inRadioPreview, value inSkin)
{
   wxWindow *parent;
   ValueToWX(inParent,parent);

   ToolBox *toolbox = ToolBox::Create(parent,(ToolButtonLabel)val_int(inPos), val_bool(inRadioPreview), 0, Val2Str(inName) );
   return WXToValue(toolbox);
}
DEFINE_PRIM(wx_toolbox_create,5)

value wx_toolbox_add_control(value inToolbox,value inControl)
{
   ToolBox *toolbox;
   wxWindow *control;
   if (ValueToWX(inToolbox,toolbox) && ValueToWX(inControl,control))
      toolbox->AddControl(control);
   return alloc_null();
}
DEFINE_PRIM(wx_toolbox_add_control,2)

value wx_toolbox_add_separator(value inToolbox)
{
   ToolBox *toolbox;
   if (ValueToWX(inToolbox,toolbox))
      toolbox->AddSeparator();
   return alloc_null();
}
DEFINE_PRIM(wx_toolbox_add_separator,1)


value wx_toolbox_add_tool(value *arg,int argc)
{
   enum { aToolbox, aID, aLabel, aBitmap, aRadio, aHelp };

   ToolBox *toolbox;
   if (ValueToWX(arg[aToolbox],toolbox))
   {
      wxBitmap *bitmap;
      wxBitmap empty;
      ValueToWX(arg[aBitmap],bitmap);
      if (!bitmap) bitmap = &empty;
      toolbox->AddTool( val_int(arg[aID]),  Val2Str(arg[aLabel]), *bitmap,
                 Val2Str(arg[aHelp]), val_bool(arg[aRadio]) ? wxITEM_RADIO : wxITEM_NORMAL );
   }
   return alloc_null();
}
DEFINE_PRIM_MULT(wx_toolbox_add_tool)



