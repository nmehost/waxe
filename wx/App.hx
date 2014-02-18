package wx;

class App
{
   static public var build(get, null):String;
   static public var ndllVersion(get, null):Int;
   static public var waxeWorksVersion(get, null):String;


   public static function boot(inOnInit:Void -> Void)
   {
      #if neko
      var init = Loader.load("neko_init",5);
      if (init!=null)
      {
          init(function(s) return new String(s),
            function(len:Int) { var r=[]; if (len>0) r[len-1]=null; return r; },
            null, true, false );
       }
      else
          throw("Could not find NekoAPI interface.");
      #end

      wx_boot(inOnInit);
   }
  
  
   public static function quit()
   {
      wx_quit();
   }

   public static function setTopWindow(inWindow:TopLevelWindow)
   {
      wx_set_top_window(inWindow.wxHandle);
   }

   static public function get_build():String { return waxe.Version.name; }
   static public function get_ndllVersion():Int { return wx_get_ndll_version(); }
   static public function get_waxeWorksVersion():String { return wx_get_waxe_works_version(); }


   static var wx_set_top_window = Loader.load("wx_set_top_window",1);
   static var wx_boot = Loader.load("wx_boot",1);
   static var wx_quit = Loader.load("wx_quit",0);
   static var wx_get_ndll_version = Loader.load("wx_get_ndll_version", 0);
   static var wx_get_waxe_works_version = Loader.load("wx_get_waxe_works_version", 0);
}
