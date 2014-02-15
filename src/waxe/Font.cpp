#include "HaxeAPI.h"

value wx_font_create(value *arg,int n)
{
	enum { aSize, aFamily, aStyle, aWeight, aUnderline, aFace, aLAST };
	wxFont *font = new wxFont( val_int(arg[aSize]), val_int(arg[aFamily]),
                        val_int(arg[aStyle]),
                        val_int(arg[aWeight]), val_bool(arg[aUnderline]),
                        Val2Str(arg[aFace]) );
	return WXToDeletingValue(font);
}
DEFINE_PRIM_MULT(wx_font_create)



