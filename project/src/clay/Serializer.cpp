#include <clay/Serializer.h>

namespace clay
{


wxString Serializer::ToSerial() const
{
   wxString result;
   if (mIsMap)
   {
      result.Printf("m%d:",(int)mMap.size());
      for(StringMap::const_iterator i=mMap.begin();i!=mMap.end();++i)
      {
         wxString key;
         key.Printf("%d=",(int)i->first.length());
         result += key + i->first;
         result+= i->second.ToSerial();
      }
      return result;
   }
   else
   {
      result.Printf("s%d=",(int)mSVal.length());
      return result + mSVal;
   }
}


bool Serializer::FromSerial(const char *inBuffer,int &inPos)
{
   char ch = inBuffer[inPos++];
   if (ch!='s' && ch!='m')
      return false;

   int val=0;
   while( inBuffer[inPos]>='0' && inBuffer[inPos]<='9')
   {
      val = val*10 + inBuffer[inPos]-'0';
      inPos++;
   }

   inPos++;
   if (ch=='s')
   {
      mIsMap=false;
      mSVal = wxString(inBuffer+inPos,val);
      inPos += val;
   }
   else
   {
      mIsMap=true;
      for(int i=0;i<val;i++)
      {
         int len = 0;
         while( inBuffer[inPos]>='0' && inBuffer[inPos]<='9')
         {
            len = len*10 + inBuffer[inPos]-'0';
            inPos++;
         }
         inPos++;

         Serializer &value = mMap[ wxString(inBuffer+inPos,len) ];
         inPos+=len;

         if (!value.FromSerial(inBuffer,inPos))
            return false;
      }
   }

   return true;
}


} // end namespace clay
