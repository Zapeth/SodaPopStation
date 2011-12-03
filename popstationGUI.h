#ifndef __POPSTATIONGUI_H
#define __POPSTATIONGUI_H

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/gbsizer.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "header.h"

class gameDataDialog;
class configPanel;
class popFrame: public wxFrame
{
public:
    popFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size
            , long style);

    void onPSButton(wxCommandEvent& event);
    void onOutputPBP (wxCommandEvent& event);
    void onConvert(wxCommandEvent& event);
    void onExtract(wxCommandEvent& event);
    void onGDialog(wxCommandEvent& event);
    void onKeyPress(wxKeyEvent& event);
    void onDiscChange(wxCommandEvent& event);
    void onTextEnter(wxCommandEvent& event);
    void onCompressChange(wxScrollEvent& event);
    void MainPage();
    //Custom Page Events
    void ExtraPage();
    //AboutPage
    void AboutPage();
    void onIconImage(wxCommandEvent& event);
    void onBackImage(wxCommandEvent& event);
    void onInfoImage(wxCommandEvent& event);
    void onBackMusic(wxCommandEvent& event);
    void onIconAnim(wxCommandEvent& event);
    void onBootImage(wxCommandEvent& event);
    void configProcessing(wxString docname);
    void configCheck();
    void gameidDetection(const wxString source_file, int CurrentDisc);
    void focusFind(wxFocusEvent& event);

    DECLARE_EVENT_TABLE()
private:
     wxStaticText* Desc1, *Desc2, *Desc3, *Desc4, *Desc5, *Desc6, *Desc7, *Desc8;
     wxStaticText *AboutText;
     wxButton* PSButton;
     wxButton* OutputPBP;
     wxChoice* PSfiletext;
     wxTextCtrl* OutputPBP_tc;
     wxTextCtrl* DiscTitle, *GameTitle, *DiscIDcode, *DiscIDnum, *GameIDcode, *GameIDnum;
     wxButton* gdDialogButton;
     wxButton* ConvertButton,*ExtractISOButton;
     wxPanel* MyPanel, *ExtraPanel, *AboutPanel;
     wxNotebook* MyNotebook;
     wxSlider* CompressionSlider;
     wxGridBagSizer *sizer,*ExtraSizer;
     //wxMenu *mode;
     wxStaticBoxSizer *IconImage,*BackImage,*InfoImage,*BackMusic,*IconAnim,*BootImage;
     wxFileDialog *fileIconI,*fileBackI,*fileInfoI,*fileBackM,*fileIconA,*fileBootI;
     wxTextCtrl *textIconI,*textBackI,*textInfoI,*textBackM,*textIconA,*textBootI;
     wxButton *buttonIconI,*buttonBackI,*buttonInfoI,*buttonBackM,*buttonIconA,*buttonBootI;
     wxFileDialog* infileDialog;
     wxDirDialog *outFolderDialog;
     gameData popParm[5];//Parameters for Multidisc support
     configPanel* confPanel; 
     wxArrayString FileSelection;
     gameDataDialog* gdDialog;
     int windowWithFocus;
	settings configoptions;

enum{
    ID_PSfile=1000,
    ID_PSButton=1001,
    ID_OutputPBP_tc=1002,
    ID_OutputPBP=1003,
    ID_Desc1=1004,
    ID_Desc2=1005,
    ID_Desc3=1006,
    ID_Desc4=1007,
    ID_Desc5=1008,
    ID_Desc6=1009,
    ID_Desc7=1010,
    ID_Desc8=1011,
    ID_DiscTitle=1012,
    ID_GameTitle=1013,
    ID_DiscCode=1014,
    ID_DiscNum=1015,
    ID_GameCode=1016,
    ID_GameNum=1017,
    ID_GameDataButton=1018,
    ID_CompressionSlider=1019,
    ID_IconImage=1020,
    ID_BackImage=1021,
    ID_InfoImage=1022,
    ID_BackMusic=1023,
    ID_IconAnim=1024,
    ID_BootImage=1025,
    ID_buttonIconI=1026,
    ID_buttonBackI=1027,
    ID_buttonInfoI=1028,
    ID_buttonBackM=1029,
    ID_buttonIconA=1030,
    ID_buttonBootI=1031,
    ID_fileIconI=1032,
    ID_fileBackI=1033,
    ID_fileInfoI=1034,
    ID_fileBackM=1035,
    ID_fileIconA=1036,
    ID_fileBootI=1037,
    ID_multiDisc=1090,
    ID_ConvertButton=1099,
    ID_ExtractISOButton=1100,
    ID_gdDialog=1101,
    ID_cfPanel=1102,
    ID_AboutText=1200
};    
};


 #endif
