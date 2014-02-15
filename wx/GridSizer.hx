package wx;

class GridSizer extends Sizer
{
	function new(inHandle:Dynamic) { super(inHandle); }

	public static function create(rows:Null<Int>,cols:Int,vgap:Int=0,hgap:Int=0)
	{
		return new GridSizer( wx_sizer_create_grid(rows,cols,vgap,hgap) );
	}


	static var wx_sizer_create_grid = Loader.load("wx_sizer_create_grid",4);

}
