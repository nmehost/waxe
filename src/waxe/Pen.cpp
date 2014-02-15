#include "HaxeAPI.h"

value wx_pen_create(value inColour,value inWidth, value inPattern)
{
	wxPen *pen = new wxPen( Val2Colour(inColour), val_int(inWidth), val_int(inPattern) );
	return WXToDeletingValue(pen);
}
DEFINE_PRIM(wx_pen_create,3)



