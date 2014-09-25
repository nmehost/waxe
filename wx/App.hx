package wx;

class App
{
   static public var build(get, null):String;
   static public var ndllVersion(get, null):Int;
   static public var waxeWorksVersion(get, null):String;
   /*
    * Allow access to wxApp::s_macAboutMenuItemId
    * Needed for correct placement of the OSX About menu
    */
   static public var s_macAboutMenuItemId(get, set) : Int;
   /*
    * Allow access to wxApp::s_macPreferencesMenuItemId
    * Needed for correct placement of the OSX Preferences menu
    */
   static public var s_macPreferencesMenuItemId(get, set) : Int;


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

   static public function get_s_macAboutMenuItemId() : Int
   {
      return wx_get_s_macAboutMenuItemId();
   }
   static public function set_s_macAboutMenuItemId(inID:Int) : Int
   {
      wx_set_s_macAboutMenuItemId(inID);
      return inID;
   }

   static public function get_s_macPreferencesMenuItemId() : Int
   {
      return wx_get_s_macPreferencesMenuItemId();
   }
   static public function set_s_macPreferencesMenuItemId(inID:Int) : Int
   {
      wx_set_s_macPreferencesMenuItemId(inID);
      return inID;
   }

   static var wx_set_top_window = Loader.load("wx_set_top_window",1);
   static var wx_boot = Loader.load("wx_boot",1);
   static var wx_quit = Loader.load("wx_quit",0);
   static var wx_get_ndll_version = Loader.load("wx_get_ndll_version", 0);
   static var wx_get_waxe_works_version = Loader.load("wx_get_waxe_works_version", 0);
   static var wx_get_s_macAboutMenuItemId = Loader.load("wx_get_s_macAboutMenuItemId", 0);
   static var wx_set_s_macAboutMenuItemId = Loader.load("wx_set_s_macAboutMenuItemId", 1);
   static var wx_get_s_macPreferencesMenuItemId = Loader.load("wx_get_s_macPreferencesMenuItemId", 0);
   static var wx_set_s_macPreferencesMenuItemId = Loader.load("wx_set_s_macPreferencesMenuItemId", 1);
}
