package wx;

class Lib
{
   static var currentID = 10000;

   public static function nextID()
   {
      return currentID++;
   }
}

