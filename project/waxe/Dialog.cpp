#include "HaxeAPI.h"

value wx_dialog_create(value inParams)
{
	CreationParams params(inParams,wxDEFAULT_DIALOG_STYLE);
   wxDialog *frame = new wxDialog(params.parent,params.id,params.text,
										  params.position,params.size,params.flags);

   frame->Show();
   wxTheApp->SetTopWindow( frame );

   return WXToValue(frame);
}

DEFINE_PRIM(wx_dialog_create,1)

value wx_file_dialog_show(value ioData)
{
   wxWindow *parent = 0;
   wxString message;
   wxString directory;
   wxString file;
   wxString filter;
   int      style;
   wxPoint  position;
   wxSize   size;

   ValueToWX(val_field(ioData,val_id("parent")),parent);
   ValueToWX(val_field(ioData,val_id("message")),message);
   ValueToWX(val_field(ioData,val_id("directory")),directory);
   ValueToWX(val_field(ioData,val_id("file")),file);
   ValueToWX(val_field(ioData,val_id("filter")),filter);
   ValueToWX(val_field(ioData,val_id("style")),style);
   ValueToWX(val_field(ioData,val_id("size")),size);

   wxFileDialog *dlg = new wxFileDialog(parent,message,directory,file,filter,style,position,size);
   bool result = dlg->ShowModal()==wxID_OK;

   if (result)
   {
     alloc_field(ioData,val_id("file"), WXToValue(dlg->GetFilename()));
     alloc_field(ioData,val_id("directory"), WXToValue(dlg->GetDirectory()));
     wxArrayString files;
     dlg->GetFilenames(files);
     value array = val_field(ioData,val_id("files"));
     for(int i=0;i<files.size();i++)
        val_array_push(array,WXToValue(files[i]));
   }

   dlg->Destroy();
   return alloc_bool(result);
}

DEFINE_PRIM(wx_file_dialog_show,1)



value wx_dir_dialog_show(value ioData)
{
   wxWindow *parent = 0;
   wxString message;
   wxString directory;
   int      style;
   wxPoint  position;
   wxSize   size;

   ValueToWX(val_field(ioData,val_id("parent")),parent);
   ValueToWX(val_field(ioData,val_id("message")),message);
   ValueToWX(val_field(ioData,val_id("directory")),directory);
   ValueToWX(val_field(ioData,val_id("style")),style);
   ValueToWX(val_field(ioData,val_id("size")),size);

   wxDirDialog *dlg = new wxDirDialog(parent,message,directory,style,position,size);
   bool result = dlg->ShowModal()==wxID_OK;

   if (result)
   {
     alloc_field(ioData,val_id("directory"), WXToValue(dlg->GetPath()));
   }

   dlg->Destroy();
   return alloc_bool(result);
}

DEFINE_PRIM(wx_dir_dialog_show,1)

