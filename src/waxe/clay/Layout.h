#ifndef CLAY_LAYOUT_H
#define CLAY_LAYOUT_H

#include <wx/wx.h>

#include<vector>
#include<map>

#ifndef HX_WINDOWS
#define PSEUDO_MDI
#endif


DECLARE_EVENT_TYPE(wxEVT_WINDOW_LIST_CHANGED, -1)

class wxIconBundle;

namespace clay
{

class Skin;
class Serializer;
// TODO: split container into public and provate interfaces
struct HintInfo;

enum ContainerStyle
{
   csFrame, csWindow, csVStack, csHStack, csNotebook,  csMDIClient,  csHold,
};

class Manager;

enum AddPosition { apLeft, apRight, apAbove, apBelow, apOver };

enum SizeType { stBest, stMin,stMax, stFloating, stWideDock, stTallDock,  stSIZE };

enum ChildChangeFlags
{
   ccfCaption       = 0x0001,
   ccfCurrent       = 0x0002,
   ccfActivate      = 0x0004,
   ccfChildClosed   = 0x0008,
   ccfWindow        = 0x0010,
   ccfMinSize       = 0x0020,
   ccfChildAdded    = 0x0020,
};

enum ContainerFlags
{
   cfDestroyOnClose = 0x0001,
   cfFloatFullFrame = 0x0002,
   cfCantClose      = 0x0004,
};

enum WindowStyle
{
   wsNormal,
   wsToolbar,
};

class Container : public wxObject
{
public:
   Container(class Manager *inManager);
   virtual ~Container();

   virtual ContainerStyle GetStyle() = 0;
   virtual void OnChildChange(unsigned int inFlags,Container *inChild) { }

   // Child/parent management
   virtual Container *GetParent()=0;
   virtual void Reparent(Container *inContainer)=0;
   virtual void ReplaceChild(Container *inContainer,Container *inNew=0) = 0;
   virtual Container *Insert(Container *inContainer,Container *inSibling,
                         AddPosition inWhere)=0;
   virtual void CopySizeInfo(Container *inSource);
   virtual Container *GetOnlyChild() { return 0; }
   virtual bool Visible() = 0;
   virtual void DestroyOrClose() = 0;
   virtual bool ShouldFloatFullFrame() const { return false; }
   virtual bool CanClose() { return true; }
   virtual bool ShouldClose() { return true; }
   virtual Container *FindClientContainer() { return 0; }

   virtual void Raise() = 0;

   virtual Container *Add(Container *inContainer,AddPosition inWhere);
   Container *Add(wxWindow *inWindow,AddPosition inWhere,
                    wxIconBundle inIcons=wxIconBundle(),
                    unsigned int inFlags=0,
                    WindowStyle inStyle=wsNormal);
   Container *Add(class ToolBox *inToolBox,AddPosition inWhere);


   virtual Manager *GetManager() { return mManager; }

   // Window info ...
   virtual wxString     GetCaption() { return ""; }
   virtual const wxIconBundle *GetIconSet() { return 0; }
   virtual wxWindow *AsParent() { return 0; }
   virtual int GetUID() { return mUID; }
   virtual void ToSerial(Serializer &outSerializer) const;
   virtual void FromSerial(Serializer &inSerializer);

   // Container details
   virtual bool IsEmpty() { return false; }
   virtual wxWindow *GetWindow() { return 0; }
   virtual bool CanAddChild(AddPosition inWhere)  { return false; }
   virtual bool ShowsChildsTitle() { return true; }
   virtual bool IsWideDock() { return true; }
   virtual bool ChildContains(wxPoint inPos) { return false; }

   // Positioning
   // Rects are w.r.t parent window, not container...
   virtual void Show(bool inShow) = 0;
   virtual void SetRect(const wxRect &inRect) = 0;
   virtual wxRect Rect() = 0;

   virtual void DoLayout() { }
   virtual void RaiseChild(Container *inContainer) { Raise(); }

   
   // Minumim size management
   virtual bool GetPreferredSize(wxSize &ioSize,bool inFromWidth)
      { return false; /* No change */ }
   virtual void SetWidth(int inWidth,SizeType inType) { }
   virtual void SetHeight(int inHeight,SizeType inType) { }
   virtual bool IsFixed(bool inHorizontal,wxSize &ioSize) { return false; }


   virtual wxSize GetSize(SizeType inType);
   virtual void   RecordSize(SizeType inType,wxRect inRect=wxRect());

   virtual bool HintsFromMouse(HintInfo &outHints, Container *inDrop,
                   wxPoint inPos);

   virtual bool GetSiblingDock(Container *inDrop,Container *inSibling,
                   int inDirection,wxRect &outRect) { return false; }

   virtual void GetDockPos(Container *inDrop,
                    int inDirection,
                    wxRect &outRect);
   

   // For rendering and layout-out

   virtual void Render(wxDC &inDC,Skin *inSkin) = 0;

   wxClientDataContainer mData;
   Manager *mManager;
   wxSize  mSizes[stSIZE];
   int     mUID;
};



typedef std::vector<Container *> ContainerList;
typedef std::map<wxWindow *,Container *> ContainerMap;


void ToSerial(const wxFrame *inFrame, Serializer &outSerializer);
void FromSerial(wxFrame *inFrame, Serializer &inSerializer);

class Manager : public wxObject
{
public:
   Manager(wxFrame *inFrame);
   ~Manager();


   Container *Create(wxFrame *inFrame, bool inIsFloating);
   Container *Create(wxWindow *inFrame,WindowStyle inStyle,
                             wxIconBundle inIcons=wxIconBundle(),
                             unsigned int inFlags=0);
   Container *Create(class ToolBox *inToolBox);
   Container *Create(wxMDIParentFrame *inMDI);
   Container *Create(ContainerStyle inStyle,SizeType = stBest);

   Container *Create(Serializer &inSerial);

   Container *GetRootContainer() { return mRootContainer; }

   void FloatContainer(Container *inContainer);

   void HintsFromMouse(Container *inExclude,wxPoint inPos);
   void TryDropFrame(wxFrame *inFrame, Container *inFrameContainer,
           wxPoint inMouse);
   void CloseFloating(wxFrame *inFrame,Container *inFrameContainer);
   void Park(Container *inContainer);
   void HideHints();

   const ContainerMap &GetWindowMap() { return mWindows; }

   wxString ToSerial();
   bool     FromSerial(const wxString &inString);

   static Skin *GetSkin();


   void RecordWindow(wxWindow *inWindow,Container *inContainer);
   void RemoveWindow(wxWindow *inWindow);

   wxClientDataContainer mData;

private:
   wxFrame          *mFrame;
   wxMDIParentFrame *mMDI;
   Container        *mRootContainer;
   Container        *mMDIClientContainer;
   Container        *mHiddenContainer;
   class HintWindow *mHintWindow;

   ContainerList    mTopLevelContainers;
   ContainerMap     mWindows;

   ContainerMap     mTransfer;
   ContainerList    mOldTopLevelContainers;
   bool             mMDIClientUsed;

   static  Skin     *mSkin;
};


} // end namespace clay

#endif

