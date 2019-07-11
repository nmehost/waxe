package waxe;

@:cppFileCode( 'extern "C" void waxe_register_prims();')
@:build(waxe.macros.BuildXml.importRelative("../../project/Build.xml"))
@:keep class StaticWaxe
{
   static function __init__()
   {
      #if (cpp && !cppia)
      untyped __cpp__("waxe_register_prims();");
      #end
   }
}


