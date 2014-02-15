package wx;

class Loader
{
   public static function load(inFunction:String, inArgCount:Int) : Dynamic
   {
      #if cpp
      return cpp.Lib.load("waxe",inFunction, inArgCount);
      #else
      return neko.Lib.load("waxe",inFunction, inArgCount);
      #end
   }
}
