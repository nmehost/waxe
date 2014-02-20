#include <clay/Skin.h>
#include <clay/Layout.h>
#include <algorithm>
   

namespace clay
{

static int sTabStart  = 1;
static int sTabHeight = 22;
static int sTabTGap   = 2;
static int sTabTotal   = sTabHeight + sTabTGap;
static int sTabBoxX   = 16+16;
static int sTBGrip = 10;
   
static int sGripWidth = 15;
   

static int sDX[] = 
   { 0, 2, 3,3,3, 4,4,4,4,4, 5,5,5,5, 6,6,6, 7,7, 8, 9 };

static int mGPos[][2] = { { 2,4 },   {2,10},  {2,16},
                             { 7, 7},   { 7, 13 } };


// wxAUI built in bitmaps
static unsigned char close_bits[]={
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xcf, 0xf9,
      0x9f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x9f, 0xfc, 0xcf, 0xf9, 0xe7, 0xf3,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static bool sCloseInit = false;
static wxBitmap *sCloseBitmaps[3] = { 0,0,0 };
static bool sMenuInit = false;
static wxBitmap *sMenuBitmaps[3] = { 0,0,0 };


wxBitmap *BitmapFromBits(const unsigned char bits[], int w, int h,
                             const wxColour& color)
{
    wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
    img.Replace(0,0,0,123,123,123);
    img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
    img.SetMaskColour(123,123,123);
    return new wxBitmap(img,24);
}



void HostedButton::Render(wxDC &inDC)
{
   inDC.DrawBitmap(mBitmap[mState], mRect.x, mRect.y, true);
}




class DefaultSkin : public Skin
{
public:
   wxPen mActiveTabPen[21];
   wxPen mTabPen[21];
   bool  mDoShadowText;

   DefaultSkin()
   {
      mRefDC = 0;
      mColour[scMedium] =  wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
      mColour[scLight] =  wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHILIGHT);
      mColour[scDark] =  wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
      mColour[sc3DLight] =  wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
      mColour[sc3DDark] =  wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
      mColour[scError] =  wxColour(255,0,255);
      mColour[scTitle] =  wxColour(80,80,180);
      mColour[scTitleText] =  wxColour(255,255,255);
      mColour[scTitleTextShadow] =  wxColour(0,0,0);
      mColour[scTabOutline] =  wxColour(20,20,128);
      mColour[scBlack] =  wxColour(0,0,0);

      int dr = mColour[scMedium].Red() - mColour[scDark].Red();
      int dg = mColour[scMedium].Green() - mColour[scDark].Green();
      int db = mColour[scMedium].Blue() - mColour[scDark].Blue();
      mTBLow = wxColour(  mColour[scMedium].Red() - dr/2,
                          mColour[scMedium].Green() - dg/2,
                          mColour[scMedium].Blue() - db/2 );

      dr = mColour[scMedium].Red() - mColour[scLight].Red();
      dg = mColour[scMedium].Green() - mColour[scLight].Green();
      db = mColour[scMedium].Blue() - mColour[scLight].Blue();
      mTBHigh = wxColour( mColour[scMedium].Red() - dr/2,
                          mColour[scMedium].Green() - dg/2,
                          mColour[scMedium].Blue() - db/2 );
   
		#ifdef HX_WINDOWS
		int size = 10;
		mDoShadowText = false;
		#else
		int size = 12;
		mDoShadowText = true;
		#endif
      mFont = new wxFont(size,wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_LIGHT);
      mFontActive = new wxFont(size,wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL);

      int r0 = mColour[scLight].Red();
      int g0 = mColour[scLight].Green();
      int b0 = mColour[scLight].Blue();

      dr = mColour[scDark].Red()-r0;
      dg = mColour[scDark].Green()-g0;
      db = mColour[scDark].Blue()-b0;


      for(int y=0;y<21;y++)
      {
         mActiveTabPen[y] = wxPen( wxColour( r0+dr*y/21, g0+dg*y/20, b0+db*y/21 ) );
         mTabPen[y] = wxPen( wxColour( (r0+dr*y/21)*8/10, (g0+dg*y/21)*8/10, (b0+db*y/21)*8/10 ) );
      }
   }
   
   ~DefaultSkin()
   {
      delete mRefDC;
   }
 



   
   int AddCloseBox(HostedButtons &outButtons,int inX,int inY,int inHeight)
   {
      if (!sCloseInit)
      {
         sCloseInit = true;
         sCloseBitmaps[0] = BitmapFromBits(close_bits,16,16,*wxWHITE);
         sCloseBitmaps[1] = BitmapFromBits(close_bits,16,16,*wxBLACK);
         sCloseBitmaps[2] = BitmapFromBits(close_bits,16,16,*wxBLACK);
      }
   
      int n = outButtons.size();
      outButtons.resize(n+1);
      HostedButton &but = outButtons[n];
      but.mBitmap[0] = *sCloseBitmaps[0];
      but.mBitmap[1] = *sCloseBitmaps[1];
      but.mBitmap[2] = *sCloseBitmaps[2];
      but.mRect = wxRect(inX+2, inY+ (inHeight-16)/2, 16, 16);
      but.mCommand = bcClose;
      return 20;
   }
   
   
   int AddMenuButton(HostedButtons &outButtons,int inX,int inY,int inHeight)
   {
      if (!sMenuInit)
      {
         sMenuInit = true;
         wxMemoryDC dc;
         for(int i=0;i<3;i++)
         {
            sMenuBitmaps[i] = new wxBitmap(16,16,32);
            wxBitmap &bmp = *sMenuBitmaps[i];
            dc.SelectObject(bmp);
            #if 1
            dc.SetBrush( *wxTRANSPARENT_BRUSH );
            #else
            dc.SetBrush( wxBrush(wxColor(255,0,255)) );
            #endif
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawRectangle(-2,-2,20,20);

            dc.SetBrush( *wxTRANSPARENT_BRUSH );
            dc.SetPen(*wxTRANSPARENT_PEN);
   
            wxPoint arrow[] = { wxPoint(3,6), wxPoint(11,6), wxPoint(7,10) };

            if (i>0)
            {
               dc.SetBrush( *wxBLACK_BRUSH );
               dc.DrawPolygon(3,arrow);
            }
            else
            {
               dc.SetPen(wxPen(mColour[sc3DDark] ));
               dc.DrawLine(arrow[2],arrow[0]);
               dc.DrawLine(arrow[0],arrow[1]);
               dc.SetPen(wxPen(mColour[scLight] ));
               dc.DrawLine(arrow[1],arrow[2]);

            }

   
            wxBitmap empty;
            dc.SelectObject(empty);
            #if 0
            // No good when anti-aliased lines are used (wxMac)...
            bmp.SetMask( new wxMask(bmp,wxColour(255,0,255)) );
            #endif
         }
      }
   
      int n = outButtons.size();
      outButtons.resize(n+1);
      HostedButton &but = outButtons[n];
      but.mBitmap[0] = *sMenuBitmaps[0];
      but.mBitmap[1] = *sMenuBitmaps[1];
      but.mBitmap[2] = *sMenuBitmaps[2];
      but.mFireOnDown = false;
      but.mRect = wxRect(inX+2, inY+ (inHeight-16)/2, 16, 16);
      but.mCommand = bcMenu;
      return 20;
   }
   
   
   
   
   





   void DrawTitleBar(wxDC &inDC,const wxRect &inRect,wxString &inTitle)
   {
      // DrawRect(inDC,inRect,scLight);
      wxRect r = inRect;
      r.height = inRect.height * 2 / 3;
      inDC.GradientFillLinear( r, mColour[sc3DLight], mColour[scMedium], wxSOUTH);
      r.y += r.height;
      r.height = inRect.GetBottom() - r.y;
      inDC.GradientFillLinear( r, mColour[scMedium], mColour[sc3DDark], wxSOUTH);
   
      if (!inTitle.empty())
      {
         inDC.SetFont(*mFontActive);
         if (mDoShadowText)
         {
            inDC.SetTextForeground(mColour[scTitleTextShadow]);
            inDC.DrawText(inTitle,r.x+5,r.y+3);
         }
         inDC.SetTextForeground(mColour[scTitleText]);
         inDC.DrawText(inTitle,r.x+4,r.y+2);
      }
   
   }
   
   int GetSashSize()
   {
      return 4;
   }
   
   
   void DrawRect(wxDC &inDC,const wxRect &inRect,SkinColour inCol)
   {
      inDC.SetPen(*wxTRANSPARENT_PEN);
      inDC.SetBrush( wxBrush( mColour[inCol], wxSOLID )  );
      inDC.DrawRectangle(inRect);
   }

   void RenderBackground(wxDC &inDC,const wxRect &inRect)
   {
      DrawRect(inDC,inRect,scBG);
   }

   void RenderEmptyNotebook(wxDC &inDC,const wxRect &inRect)
   {
      DrawRect(inDC,inRect,sc3DDark);
   }




   void DrawEmptyFrame(wxDC &inDC,const wxSize &inSize)
   {
      DrawRect(inDC,wxRect( wxPoint(0,0), inSize) ,scBG);
   }

   
   void LineRect(wxDC &inDC,const wxRect &inRect,SkinColour inCol)
   {
      inDC.SetPen( wxPen( mColour[inCol],1 ) );
      inDC.DrawLine(inRect.x,inRect.y,
              inRect.x+inRect.width-1,inRect.y);
      inDC.DrawLine(inRect.x+inRect.width-1,inRect.y,
              inRect.x+inRect.width-1, inRect.y+inRect.height-1);
      inDC.DrawLine(inRect.x+inRect.width-1, inRect.y+inRect.height-1,
              inRect.x, inRect.y+inRect.height-1);
      inDC.DrawLine( inRect.x, inRect.y+inRect.height-1,
              inRect.x,inRect.y);
   }
   
   
   wxRect GetNotebookPageSize(const wxRect &inRect)
   {
      return wxRect( inRect.x+1, inRect.y+1+sTabTotal,
                     inRect.width-2, inRect.height-2-sTabTotal);
   }

   void RenderGripper(wxDC &inDC,int inX,int inY)
   {
      inDC.SetPen( wxPen(mColour[scLight]) );

      int i;
      for(i=0;i<5;i++)
      {
         inDC.DrawLine(inX+1+mGPos[i][0],inY+3+mGPos[i][1],
                       inX+3+mGPos[i][0],inY+3+mGPos[i][1]);
         inDC.DrawLine(inX+3+mGPos[i][0],inY+1+mGPos[i][1],
                       inX+3+mGPos[i][0],inY+3+mGPos[i][1]);
      }

      inDC.SetPen( wxPen(mColour[scDark]) );

      for(i=0;i<5;i++)
      {
         inDC.DrawLine(inX+1+mGPos[i][0],inY+0+mGPos[i][1],
                       inX+3+mGPos[i][0],inY+0+mGPos[i][1]);
         inDC.DrawLine(inX+0+mGPos[i][0],inY+1+mGPos[i][1],
                       inX+0+mGPos[i][0],inY+3+mGPos[i][1]);
      }
   }
   
   
   void LayoutTabs(const wxRect &inParent,TabInfos &inInfo,
                         int inCurrent, int &inFirst,
                         wxRect &outDragBox,
                         wxRect &outTabDragBox,
                         HostedButtons &outButtons)
   {
      int x = inParent.x+sTabStart;
      int layout_width = inParent.width - 7; /* 7 = tail size */

      outButtons.resize(0);
      outDragBox = wxRect(x,inParent.y,sTabBoxX,sTabTotal);

      AddMenuButton( outButtons, x+12, inParent.y+sTabTGap,sTabHeight);

      x+=sTabBoxX;

      outTabDragBox = wxRect();

      if (!mRefDC)
          mRefDC = new wxScreenDC();

      int fixed_size = x-inParent.x;
   
      int t;
      for(t=0;t<inFirst;t++)
         *inInfo[t].mHitRect = wxRect();
   
       mRefDC->SetFont(*mFont);
   
       int last = inParent.GetRight();
       std::vector<int> others;

       for(; t<inInfo.size() ;t++)
       {
          TabInfo &info = inInfo[t];
          info.mTruncated = false;
          int w = mRefDC->GetTextExtent(info.mTitle).x + 10;
          if (info.mIcons && !info.mIcons->IsEmpty())
          {
             wxIcon icon = info.mIcons->GetIcon(16);
             if (icon.GetHeight()==16)
                w+= icon.GetWidth() + 1;
          }

          if (t==inCurrent)
          {
             w+=sGripWidth;
             w+=1;
             if (info.mClosable)
             {
                w+=AddCloseBox(outButtons,x+w,inParent.y+sTabTGap,sTabHeight);
                w+=3;
             }
             outTabDragBox = wxRect(x,inParent.y,w,sTabTotal);
             fixed_size += w;
          }
          else
             others.push_back(w);

          *inInfo[t].mHitRect = wxRect( x, inParent.y, w, sTabTotal);
          x+= w;
       }

       int len = x - inParent.x;

       if (len>layout_width && inInfo.size()>1)
       {
          // how big for non-current tabs?
          int others_len = layout_width - fixed_size;
          if (others_len<0) others_len = 0;

          int max = 0;
          std::sort( others.begin(), others.end() );

          // Find a max-tab-size such that the sum of all tabs sizes less
          //  than this, plus this size * number bigger equals others_len
          int n = others.size();
          int biggest_ok = others_len/n;
          int sum_less = 0;
          for(int o=0;o<n-1;o++)
          {
             sum_less += others[o];

             // If we use others[o] as max size ...
             int remainder = others_len - (sum_less + (n-o-1) * others[o]);
             if (remainder>=0)
             {
                biggest_ok = others[o] + remainder/(n-o-1);
             }
             else
                break;

          }


          int delta = 0;
          for(int t=0;t<inInfo.size();t++)
          {
             TabInfo &tab = inInfo[t];
             wxRect &r = *tab.mHitRect;
             r.x-=delta;
             if (t!=inCurrent)
             {
                if (r.width>biggest_ok)
                {
                   delta += r.width-biggest_ok;
                   if (t==inInfo.size()-1)
                      r.width = inParent.GetRight() - 7 - r.x;
                   else
                      r.width = biggest_ok;
                   tab.mTruncated = true;
                }
             }
             else
             { 
                outTabDragBox.x -= delta;
                for(int b=1;b<outButtons.size();b++)
                   outButtons[b].mRect.x -= delta;
             }
          }
       }
   
       for(; t<inInfo.size() && x<last;t++)
         *inInfo[t].mHitRect = wxRect();
   }
   
   void RenderTab(wxDC &inDC,const wxString &inText,const wxIcon *inIcon,
              int inX,int inY, int inWidth,bool inSelected,
              bool inRenderBlock=0)
   {
      if (!inSelected)
         inY+=2;

      inDC.SetBrush(*wxTRANSPARENT_BRUSH);
   
      int h = sTabHeight;
   
      wxPen *p = inSelected ? mActiveTabPen : mTabPen;
      if (inRenderBlock)
      {
         wxPoint points[] =
            {
               wxPoint(inX        -1,inY+h+1),
               wxPoint(inX        -1,inY+1),
               wxPoint(inX        ,inY),
   
               wxPoint(inX+inWidth+6,inY),
               wxPoint(inX+inWidth+7,inY+1),
               wxPoint(inX+inWidth+7,inY+h+1),
            };
 
         for(int y=0;y<21;y++)
         {
            inDC.SetPen( p[y] );
            inDC.DrawLine(inX,inY+1+y,inX+inWidth+7,inY+1+y);
         }
         inDC.DrawLine(inX,inY+1+21,inX+inWidth+7,inY+1+21);
         inDC.SetPen(wxPen(mColour[scTabOutline],1));
         inDC.DrawPolygon(6, points);

      }
      else
      {
         wxPoint points[] =
            {
               wxPoint(inX        -1,inY+h+1),
               wxPoint(inX        -1,inY+1),
               wxPoint(inX        ,inY),
   
               wxPoint(inX+inWidth-4,inY),
               wxPoint(inX+inWidth,inY+3),
               wxPoint(inX+inWidth+3,inY+h-5),
               wxPoint(inX+inWidth+7,inY+h+1),
            };
   
   
         for(int y=0;y<21;y++)
         {
            inDC.SetPen( p[y] );
            inDC.DrawLine(inX,inY+1+y,inX+inWidth-3+sDX[y],inY+1+y);
         }
         inDC.DrawLine(inX,inY+1+21,inX+inWidth-3+sDX[20],inY+1+21);
         inDC.SetPen(wxPen(mColour[scTabOutline],1));
         inDC.DrawPolygon(7, points);
      }

   
      if (inSelected)
      {
         RenderGripper(inDC,inX,inY);
         inX += sGripWidth;
      }

      inX += 2;
      if (inIcon)
      {
         inDC.DrawIcon(*inIcon,inX,inY+3);
         inX+=inIcon->GetWidth()+1;
      }

      if (mDoShadowText)
      {
         inDC.SetTextForeground(*wxWHITE);
         inDC.DrawText(inText,inX,inY+4);
      }
      inDC.SetTextForeground(*wxBLACK);
      inDC.DrawText(inText,inX,inY+3);
   }
   
   
   
   void RenderTabs(wxDC &inDC,const wxRect &inParent,
            TabInfos &inInfo, int inCurrent, int inFirst,
            HostedButtons &inButtons,bool inCanDrag)
   {
      // Draw the outline directly
      wxRect outline(inParent);
      outline.y+=sTabTotal;
      outline.height-=sTabTotal;
      LineRect(inDC, outline, scTabOutline);




      wxRect tab_back(inParent);
      tab_back.height = sTabTotal;

      wxDC &dc = GetDoubleBufferDC(tab_back);

      DrawRect(dc,tab_back,scBG);
   
      int x = inParent.x+sTabStart;

      int oy = inParent.y + sTabTGap;
      for(int y=0;y<21;y++)
      {
         dc.SetPen( mTabPen[y] );
         dc.DrawLine(x,oy+1+y,x+sTabBoxX-3+sDX[y],oy+1+y);
      }
      if (inCanDrag)
         RenderGripper(dc,x,oy);

      x+=sTabBoxX;
      int last = inParent.GetRight();
   
      std::vector<int> starts;
      std::vector<int> widths;
      std::vector<const wxIcon *> icons;
   
      dc.SetFont(*mFont);

      for(int t=inFirst; t<inInfo.size() && x<last;t++)
      {
         TabInfo &info = inInfo[t];
         wxString text =info.mTitle;
         int w = info.mHitRect->width;

         wxIcon *icon = 0;
         wxIcon icon_buf;
         if (info.mIcons && !info.mIcons->IsEmpty())
         {
           icon_buf = info.mIcons->GetIcon(16);
           icon = &icon_buf;
           if (icon_buf.GetHeight()!=16)
              icon = 0;
         }
   
         if (t==inCurrent)
         {
            starts.push_back(x);
            widths.push_back(w);
            icons.push_back(icon);
         }
         else
         {
            RenderTab(dc, text, icon, x, inParent.y + sTabTGap, w, false,
               info.mTruncated && t==inInfo.size()-1 );
         }
   
         x+= w;
      }
   

      for(int t=starts.size()-1;t>0;t--)
      {
         RenderTab(dc, inInfo[t+inCurrent].mTitle, icons[t], starts[t],
                    inParent.y + sTabTGap, widths[t], false );
      }
   
   
   
      dc.SetFont(*mFontActive);

      if (starts.size())
      {
         RenderTab(dc, inInfo[inCurrent].mTitle, icons[0], starts[0],
                 inParent.y + sTabTGap, widths[0], true );
      }

      for(int b=0;b<inButtons.size();b++)
         inButtons[b].Render(dc);

      SwapDoubleBuffer(inDC,tab_back);
   }


   
   
   wxRect GetNormalBorders()
   {
      return wxRect( 1, 1+sTabTotal,2, 2+sTabTotal);
   
      //return wxRect(1,21,2,22);
   }

   void GetNormalDragBox(Container &inContainer,wxRect &outDragBox)
   {
      if (!mRefDC)
          mRefDC = new wxScreenDC();

      wxString text = inContainer.GetCaption();
      int w = mRefDC->GetTextExtent(text).x + 10 + sGripWidth + 40;

      wxRect r = inContainer.Rect();
      outDragBox = wxRect(r.x,r.y,w,sTabTotal+2);
   }

   
   
   void RenderNormalBorders(wxDC &inDC,const wxRect &inRect,
          Container &inContainer, HostedButtons &inButtons)
   {
      wxRect r = inContainer.Rect();
 
      // Draw this straight on the canvas ...
      wxRect outline(r);
      outline.y+=sTabTotal;
      outline.height-=sTabTotal;
      LineRect(inDC, outline, scTabOutline);


      // draw the tabs off-screen ...
      wxRect tab_back(r);
      tab_back.height = sTabTotal;
      wxDC &tab_buf = GetDoubleBufferDC(tab_back);

      DrawRect(tab_buf,tab_back,scBG);
   
      wxString text = inContainer.GetCaption();
      int w = inRect.width;
      //int w = r.width - 10;

      const wxIcon *icon = 0;
      wxIcon i;
      const wxIconBundle *icons = inContainer.GetIconSet();
      if (icons && !icons->IsEmpty())
      {
         i = icons->GetIcon(16);
         if (i.GetHeight()==16)
            icon = &i;
      }
      

      tab_buf.SetFont(*mFontActive);
      RenderTab(tab_buf, inContainer.GetCaption(), icon, r.x + sTabStart,
                 r.y + sTabTGap, w, true );

      for(int i=0;i<inButtons.size();i++)
         inButtons[i].Render(tab_buf);

      SwapDoubleBuffer(inDC,tab_back);
   }

   void LayoutNormalBorders(wxRect &ioRect, wxRect &outDragBox,
        Container &inContainer,
        HostedButtons &outButtons)
   {
      outButtons.resize(0);
      if (!mRefDC)
          mRefDC = new wxScreenDC();
         
      int w = mRefDC->GetTextExtent(inContainer.GetCaption()).x + 3 + sGripWidth;

      const wxIconBundle *icons = inContainer.GetIconSet();
      if (icons && !icons->IsEmpty())
      {
         wxIcon i = icons->GetIcon(16);
         if (i.GetHeight()==16)
            w += i.GetWidth() + 1;
      }

      if (inContainer.CanClose())
      {
         w+=AddCloseBox(outButtons,ioRect.x+w,ioRect.y+sTabTGap,sTabHeight);
         w+=3;
      }


      outDragBox = ioRect;
      outDragBox.width = w;
      outDragBox.height = sTabTotal;


      ioRect.x+=1;
      ioRect.y+=1+sTabTotal;
      ioRect.width -=2;
      ioRect.height-=2+sTabTotal;
   }

   
   
   /*
      inDC.SetPen(wxPen(mColour[scLight],1));
      inDC.SetBrush( wxBrush( mColour[scTitle], wxSOLID )  );
      wxString title = inContainer.GetCaption();
      r.x-=1;
      r.width+=2;
      r.y-=21;
      r.height=20;
      DrawTitleBar(inDC,r,title);
   
   
      //inDC.DrawRectangle(r);
      if (!title.empty())
      {
         inDC.SetTextForeground(*wxBLACK);
         inDC.SetFont(*mFont);
         inDC.DrawText(title,4,2);
      }
   */
   
   void RenderSash(wxDC &inDC,const wxRect &inParent,int inPos,
                     bool inHorizontalStacked)
   {
      wxRect r = inParent;
      if (inHorizontalStacked)
      {
         r.width = GetSashSize();
         r.x = inPos;
      }
      else
      {
         r.height = GetSashSize();
         r.y = inPos;
      }
      inDC.SetPen(*wxTRANSPARENT_PEN);
      inDC.SetBrush(mColour[scBG]);
      inDC.DrawRectangle(r);

      /*
      inDC.SetPen(wxPen(mColour[scLight]));
      if (inHorizontalStacked)
         inDC.DrawLine(r.x,r.y+1, r.x,r.y+r.height-1);
      else
         inDC.DrawLine(r.x+1,r.y, r.x+r.width-1,r.y);

      inDC.SetPen(wxPen(mColour[scDark]));
      if (inHorizontalStacked)
         inDC.DrawLine(r.x+3,r.y+1, r.x+3,r.y+r.height-1);
      else
         inDC.DrawLine(r.x+1,r.y+3, r.x+r.width-1,r.y+3);
      */

   }



   void LayoutToolbar(wxRect &ioRect,
                      wxRect &outDragBox,
                      HostedButtons &outButtons,
                      bool inWideDock)
   {
      int tb_borders = 0;

      outButtons.resize(0);
      if (inWideDock)
      {
         outDragBox = wxRect( ioRect.x+tb_borders,ioRect.y+tb_borders,
               sTBGrip, ioRect.height-tb_borders*2 );
         ioRect.x+=sTBGrip + tb_borders;
         ioRect.width-=sTBGrip + tb_borders*2;
         ioRect.y+=tb_borders;
         ioRect.height-=tb_borders*2;
      }
      else
      {
         outDragBox = wxRect( ioRect.x+tb_borders,ioRect.y+tb_borders,
                              ioRect.width-tb_borders*2, sTBGrip );
         ioRect.y+=sTBGrip + tb_borders;
         ioRect.height-=sTBGrip + tb_borders*2;
         ioRect.x+=tb_borders;
         ioRect.width-=tb_borders*2;
      }

   }

   void RenderToolbar(wxDC &inDC,const wxRect &inParent,
                      wxRect &inDragBox,
                      HostedButtons &inButtons,
                      bool inWideDock)
   {
      //wxColour c0 = mColour[scLight];
      //wxColour c1 = mColour[scDark];
      //inDC.GradientFillLinear(inDragBox, c0, c1, wxSOUTH );
      inDC.GradientFillLinear(inDragBox, mTBHigh, mTBLow, wxSOUTH );

      inDC.SetPen( wxPen( mColour[scDark] ) );
      inDC.SetBrush( *wxTRANSPARENT_BRUSH );
      //inDC.DrawRectangle(inParent.x,inParent.y,
                         //inParent.width,inParent.height,3);

      int size = inWideDock ? inParent.height : inParent.width;
      int dots = (size-5)/10;
      int p0 = (size - (dots-1)*10)/2 - 2;

      for(int i=0;i<dots;i++)
      {
         int p = p0 + i * 10;
         int x = inParent.x + ( inWideDock ? 4 : p );
         int y = inParent.y + ( inWideDock ? p : 4 );

         inDC.SetPen( wxPen(mColour[scLight]) );
         inDC.DrawLine(x+1,y+3, x+3,y+3);
         inDC.DrawLine(x+3,y+1, x+3,y+3);

         inDC.SetPen( wxPen(mColour[scDark]) );
         inDC.DrawLine(x+1,y+0, x+3,y+0);
         inDC.DrawLine(x+0,y+1, x+0,y+3);
      }

      // These shadows are drawn on the sash area - this may cause some
      //  flashing/redraw artefacts
      inDC.SetPen( wxPen( mColour[scDark] ) );

      inDC.DrawLine( inParent.x+1, inParent.y+inParent.height,
                     inParent.x+inParent.width, inParent.y+inParent.height);
      inDC.DrawLine( inParent.x+inParent.width, inParent.y+1,
                     inParent.x+inParent.width, inParent.y+inParent.height);
   }

   void RenderToolbarBackground(wxDC &inDC, const wxRect &inRect)
   {
      inDC.GradientFillLinear( inRect, mTBHigh, mTBLow, wxSOUTH);
   }

   void RenderToolbarSubRect(wxDC &inDC,
                   const wxRect &inRect,const wxRect &inSubRect)
   {
      int h = inRect.height;
      if (h<1) return;

      RenderToolbarBackground(inDC,inRect);
   }



   static wxDC &GetDoubleBufferDC(const wxRect &inRect)
   {
      if (!mMemDC)
      {
         mMemDC = new wxMemoryDC;
      }

      int w = std::max(2,inRect.width);
      int h = std::max(2,inRect.height);
      if (!mDoubleBuffer ||mDoubleBuffer->GetWidth() < w || mDoubleBuffer->GetHeight() < h)
      {
         wxBitmap empty;
         mMemDC->SelectObject(empty);
         delete mDoubleBuffer;
         mDoubleBuffer = new wxBitmap(w,h);
         mMemDC->SelectObject(*mDoubleBuffer);
         //printf("Created double buffer %dx%D (%d)\n", mDoubleBuffer->GetWidth(), mDoubleBuffer->GetHeight(),
              //mDoubleBuffer->GetDepth() );
      }

      mMemDC->SetDeviceOrigin(-inRect.x,-inRect.y);
      return *mMemDC;
   }

   static void SwapDoubleBuffer(wxDC &inDC,const wxRect &inRect)
   {
      if (inRect.width>0 && inRect.height>0)
      {
         mMemDC->SetDeviceOrigin(0,0);
         inDC.Blit(inRect.x,inRect.y,inRect.width,inRect.height, mMemDC, 0, 0);
      }
   }
 

   wxDC     *mRefDC;
   wxColour mColour[scSIZE];
   wxFont   *mFontActive;
   wxFont   *mFont;

   wxColour mTBLow;
   wxColour mTBHigh;

   static wxBitmap *mDoubleBuffer;
   static wxMemoryDC *mMemDC;
};

wxMemoryDC *DefaultSkin::mMemDC = 0;
wxBitmap *DefaultSkin::mDoubleBuffer = 0;


Skin * CreateDefaultSkin() { return new DefaultSkin(); }



} // end namespace clay
