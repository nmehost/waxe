package wx;

#if cpp
typedef WeakRef<T> = cpp.vm.WeakRef<T>;
#else
class WeakRef<T>
{
   var ref:Dynamic;
   public function new(inObject:T, ?x:Bool) ref = inObject;
   public function get():T return ref;
   public function set(t:T):T {return ref=t; return t;}
}
#end
