package wx;

class FlexGridSizer extends GridSizer
{
	function new(inHandle:Dynamic) { super(inHandle); }

	public static function create(rows:Null<Int>,cols:Int,vgap:Int=0,hgap:Int=0)
	{
		return new FlexGridSizer( wx_sizer_create_flex_grid(rows,cols,vgap,hgap) );
	}

	public function addGrowableRow(row:Int, proportion:Int=0)
	{
		wx_sizer_add_growable_row(wxHandle,row,proportion);
	}

	public function addGrowableCol(col:Int, proportion:Int=0)
	{
		wx_sizer_add_growable_col(wxHandle,col,proportion);
	}

	static var wx_sizer_create_flex_grid = Loader.load("wx_sizer_create_flex_grid",4);
	static var wx_sizer_add_growable_row = Loader.load("wx_sizer_add_growable_row",3);
	static var wx_sizer_add_growable_col = Loader.load("wx_sizer_add_growable_col",3);
}
