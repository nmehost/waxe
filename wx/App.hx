package wx;

class App
{
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


  static var wx_set_top_window = Loader.load("wx_set_top_window",1);
  static var wx_boot = Loader.load("wx_boot",1);
  static var wx_quit = Loader.load("wx_quit",0);
}
