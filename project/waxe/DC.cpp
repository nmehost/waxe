#include "HaxeAPI.h"

value wx_dc_create_paint(value inWindow)
{
	wxWindow *window;
	if (ValueToWX(inWindow,window))
	{
		wxPaintDC *dc = new wxPaintDC(window);
		return WXToDeletingValue(dc);
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_create_paint,1)


value wx_dc_draw_line(value inDC, value inX1, value inY1, value inX2, value inY2)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
	{
		dc->DrawLine(val_int(inX1), val_int(inY1), val_int(inX2), val_int(inY2) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_line,5)

value wx_dc_draw_rectangle(value inDC, value x, value y, value w, value h)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
	{
		dc->DrawRectangle(val_int(x), val_int(y), val_int(w), val_int(h) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_rectangle,5)


value wx_dc_draw_ellipse(value inDC, value x, value y, value w, value h)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
	{
		dc->DrawEllipse(val_int(x), val_int(y), val_int(w), val_int(h) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_ellipse,5)

value wx_dc_draw_circle(value inDC, value x, value y, value rad)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
	{
		dc->DrawCircle(val_int(x), val_int(y), val_int(rad) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_circle,4)

value wx_dc_draw_text(value inDC, value text, value x, value y)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
	{
		dc->DrawText(Val2Str(text), val_int(x), val_int(y) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_text,4)


value wx_dc_draw_bitmap(value inDC, value inBitmap, value x, value y, value transparent)
{
	wxDC *dc;
	wxBitmap *bitmap;
	if (ValueToWX(inDC,dc) && ValueToWX(inBitmap,bitmap) )
	{
		dc->DrawBitmap(*bitmap, val_int(x), val_int(y), val_bool(transparent) );
	}
	return alloc_null();
}
DEFINE_PRIM(wx_dc_draw_bitmap,5)





value wx_dc_clear(value inDC)
{
	wxDC *dc;
	if (ValueToWX(inDC,dc))
      dc->Clear();
	return alloc_null();
}
DEFINE_PRIM(wx_dc_clear,1)

value wx_dc_set_pen(value inDC,value inPen)
{
	wxDC *dc;
	wxPen *pen;
	if (ValueToWX(inDC,dc) && ValueToWX(inPen,pen) )
      dc->SetPen(*pen);
	return alloc_null();
}
DEFINE_PRIM(wx_dc_set_pen,2)


value wx_dc_set_font(value inDC,value inFont)
{
	wxDC *dc;
	wxFont *font;
	if (ValueToWX(inDC,dc) && ValueToWX(inFont,font) )
      dc->SetFont(*font);
	return alloc_null();
}
DEFINE_PRIM(wx_dc_set_font,2)

value wx_dc_set_brush(value inDC,value inBrush)
{
	wxDC *dc;
	wxBrush *brush;
	if (ValueToWX(inDC,dc) && ValueToWX(inBrush,brush) )
      dc->SetBrush(*brush);
	return alloc_null();
}
DEFINE_PRIM(wx_dc_set_brush,2)


value wx_dc_set_background(value inDC,value inBrush)
{
	wxDC *dc;
	wxBrush *brush;
	if (ValueToWX(inDC,dc) && ValueToWX(inBrush,brush) )
      dc->SetBackground(*brush);
	return alloc_null();
}
DEFINE_PRIM(wx_dc_set_background,2)






