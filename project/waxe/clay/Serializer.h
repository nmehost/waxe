#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <wx/string.h>

#include<map>

namespace clay
{

class Serializer
{
   typedef std::map<wxString,Serializer> StringMap;

public:
   Serializer(const char *inVal=0) : mSVal(inVal), mIsMap(false) { }
   Serializer(const wxString &inVal) : mSVal(inVal), mIsMap(false) { }

   bool IsMap() const { return mIsMap; }

   bool HasKey(const wxString &inKey) const
   {
      return mIsMap && mMap.find(inKey)!=mMap.end();
   }
   bool HasKey(int inKey) const
   {
      if (!mIsMap) return false;
      wxString key;
      key.Printf("%d",inKey);
      return mMap.find(key)!=mMap.end();
   }


   Serializer &operator[](const char *inString)
   {
      mIsMap = true;
      return mMap[inString];
   }

   Serializer &operator[](const wxString &inString)
   {
      mIsMap = true;
      return mMap[inString];
   }
   Serializer &operator[](int inIndex)
   {
      wxString key;
      key.Printf("%d",inIndex);
      mIsMap = true;
      return mMap[key];
   }


   void operator=(const wxString &inRHS) { mIsMap = false; mSVal = inRHS; }
   void operator=(const char *inRHS) { mIsMap = false; mSVal = inRHS; }
   void operator=(int inRHS)
   {
      mIsMap = false;
      mSVal.Printf("%d",inRHS);
   }

   operator int () const
   {
      if (mIsMap) return 0;
      return atoi( mSVal.c_str() );
   }
   operator wxString () const
   {
      if (mIsMap) return "";
      return mSVal;
   }

   wxString ToSerial() const;
   bool FromSerial(const char *inBuffer,int &inPos);



private:
   bool mIsMap;
   StringMap mMap;
   wxString mSVal;
};


} // end namespace clay

#endif
