#include "HaxeAPI.h"

value wx_brush_create(value inColour,value inPattern)
{
	wxBrush *brush = new wxBrush( Val2Colour(inColour), val_int(inPattern) );
	return WXToDeletingValue(brush);
}
DEFINE_PRIM(wx_brush_create,2)



