package wx;

#if neko
import neko.Lib;
import sys.io.Process;
#elseif cpp
import cpp.Lib;
import sys.io.Process;
#end


class Loader
{
   public static var foundNdll = false;
   private static var moduleInit = false;
   private static var moduleName = "";

   #if false
   public static function load(func:String, args:Int):Dynamic 
   {
      return Lib.load(moduleName, func, args);
   }
   #else

   static public function findHaxeLib(inLib:String) 
   {
      //Sys.setCwd(Sys.args()[Sys.args().length-1]);
      try 
      {
         var proc = new Process("haxelib", ["path", inLib]);

         if (proc != null) 
         {
            var stream = proc.stdout;

            try 
            {
               while(true) 
               {
                  var s = stream.readLine();

                  if (s.substr(0, 1) != "-") 
                  {
                     stream.close();
                     proc.close();

                     var parts = s.split("\\").join("/");
                     var parts = parts.split("/");
                     while(parts.length>1)
                     {
                        var test = parts.join("/")+"/ndll";
                        if (sys.FileSystem.exists(test))
                        {
                           s = parts.join("/");
                           break;
                        }
                        parts.pop();
                     }

                     loaderTrace("Found haxelib " + s);
                     return s;
                  }
               }

            } catch(e:Dynamic) { }

            stream.close();
            proc.close();
         }

      } catch(e:Dynamic) { }

      return "";
   }

   public static function load(func:String, args:Int):Dynamic 
   {
      #if waxe_static_link
      return Lib.load(moduleName, func, args);
      #end

      if (moduleInit) 
      {
         if (!foundNdll)
            return null;
         #if ndll_silent_fail
         try {
            return Lib.load(moduleName, func, args);
         }
         catch(e:Dynamic)
         {
            return function() trace("Error using undefined function. " + e + " - Maybe you need to rebuild waxe.ndll");
         }
         #else
         return Lib.load(moduleName, func, args);
         #end
      }

      moduleInit = true;
      moduleName = "waxe";

      // Look in current directory first(for installed apps)
      var result:Dynamic = tryLoad("./waxe", func, args);
      if (result == null)
         result = tryLoad(".\\waxe", func, args);

      // Try standard neko path(NEKOPATH variable, system path/library paths)
      if (result == null)
         result = tryLoad("waxe", func, args);

      // Try haxelib
      if (result == null) 
      {
         var slash =(sysName().substr(7).toLowerCase() == "windows") ? "\\" : "/";
         var haxelib = findHaxeLib("waxe");

         if (haxelib != "") 
         {
            result = tryLoad(haxelib + slash + "ndll" + slash + sysName() + slash + "waxe", func, args);
            // Try haxelib64 ...
            if (result == null)
               result = tryLoad(haxelib + slash + "ndll" + slash + sysName() + "64" + slash + "waxe", func, args);
         }
      }
	  
      loaderTrace("Result : " + result );

      foundNdll = result!=null;

      #if neko
      loadNekoAPI();
      #end

      return result;
   }

   #if neko
   public static function loadNekoAPI() 
   {
      var init =  load("neko_init", 5);

      if (init != null) 
      {
         loaderTrace("Found nekoapi @ " + moduleName);
         init(function(s) return new String(s), function(len:Int) { var r = []; if (len > 0) r[len - 1] = null; return r; }, null, true, false);

      }
      else
      {
         #if !ndll_silent_fail
         throw("Could not find NekoAPI interface.");
         #end
      }
   }
   #end

   public static function loaderTrace(inStr:String) 
   {
      // Problems with initialization order in cpp...
      #if (cpp && hxcpp_api_level<331)
      var get_env = Lib.load("std", "get_env", 1);
      var debug =(get_env("WAXE_LOAD_DEBUG") != null);
      #else
      var debug =(Sys.getEnv("WAXE_LOAD_DEBUG") !=null);
      #end

      if (debug)
         Lib.println(inStr);
   }

   static function sysName() 
   {
      // Problems with initialization order in cpp...
      #if (cpp && hxcpp_api_level<331)
      var sys_string = Lib.load("std", "sys_string", 0);
      return sys_string();
      #else
      return Sys.systemName();
      #end
   }

   public static function tryLoad(inName:String, func:String, args:Int):Dynamic 
   {
      try 
      {
         var result =  Lib.load(inName, func, args);

         if (result != null) 
         {
            loaderTrace("Got result " + inName);
            moduleName = inName;
            return result;
         }

      } catch(e:Dynamic) 
      {
         loaderTrace("Failed to load : " + inName);
      }

      return null;
   }
   #end
}
