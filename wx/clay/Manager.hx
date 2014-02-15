package wx.clay;

class Manager
{
   public var root(wxGetRoot,null):Container;

   var mHandle:Dynamic;
   function _wx_deleted() { mHandle=null; }


   public function new(inFrame:wx.Frame)
   {
      mHandle = wx_clay_man_create(inFrame.wxHandle,this);
      var root = Container.wxCreate();
      root.wxHandle = wx_clay_man_init_root_container(mHandle,root);
   }

   function wxGetRoot():Container
   {
      return wx_clay_man_get_root(mHandle);
   }


   static var wx_clay_man_create = Loader.load("wx_clay_man_create",2);
   static var wx_clay_man_get_root = Loader.load("wx_clay_man_get_root",1);
   static var wx_clay_man_init_root_container = Loader.load("wx_clay_man_init_root_container",2);
}
