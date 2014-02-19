package waxe;

@:cppFileCode( 'extern "C" void waxe_register_prims();')
@:buildXml("
<target id='haxe'>
  <lib name='${haxelib:waxe}/lib/${BINDIR}/libwaxe{MSVC_VER}${LIBEXTRA}${LIBEXT}'/>
  <lib name='${haxelib:waxe-works}/lib/${BINDIR}/libwxwidgets{MSVC_VER}${LIBEXTRA}${LIBEXT}'/>
</target>
<include name='${haxelib:waxe}/lib/WaxeLink.xml'/>
")
@:keep class StaticWaxe
{
   static function __init__()
   {
     untyped __cpp__("waxe_register_prims();");
   }
}


