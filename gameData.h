#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "wx/wx.h"
#include "wx/wfstream.h"
#include "wx/txtstrm.h"
#include "wx/tokenzr.h"
#include "wx/regex.h"
#include "wx/srchctrl.h"
#include "wx/hashset.h"
#include "header.h"

const int num_of_elements_shown_in_LB=14;//Number of Elements shown in Listbox.Used for pageup and pagedown
enum{
    tGameID=0,
    tSaveFolder=1,
    tSaveDesc=2,
    tGameName=3,
    tVideoFormat=4,
    tScanID=5
};

//For getting game Prefixes form gameInfo.db

class gameDataDialog : public wxDialog
{
public:
    gameDataDialog(wxWindow* parent, wxWindowID id, const wxString& title,
            const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize,
            long style=wxCAPTION|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX);
    void onGameSelect(wxCommandEvent& event);
    void generateGameList();
    void onChar(wxKeyEvent& event);
    void onOK(wxCommandEvent& event);
    void Search(wxCommandEvent& event);
    void SearchUpdate(wxCommandEvent& event);
    void SearchID(const wxString& searchID, gameData& metaData);
    wxArrayString PrefixList();
    wxString getGameIDcode(){return m_gameIDcode;}
    wxString getGameIDnum(){return m_gameIDnum;}
    wxString getSaveIDcode(){return m_saveIDcode;}
    wxString getSaveIDnum(){return m_saveIDnum;}
    wxString getSaveFolder(){return m_saveFolder;}
    wxString getSaveDesc(){return m_saveDesc;}
    wxString getRegion(){return m_region;}
    wxString getGameTitle(){return m_gameTitle;}
    wxString getVideoFormat(){return m_videoFormat;}
    wxString getScannerID(){return m_scannerID;}

   
DECLARE_EVENT_TABLE()
private:
    wxStaticText* Desc1, *Desc2, *Desc3, *RegionDesc;
    wxListBox* gameName;
    wxChoice* gameID;
    wxTextCtrl* gameNum;
    gameDataList gameInfo;
    wxSizer* sizer;
   // wxStaticLine* line;
    wxString m_gameIDcode;
    wxString m_gameIDnum;
    wxString m_saveIDcode;
    wxString m_saveIDnum;
    wxString m_saveFolder;
    wxString m_saveDesc;
    wxString m_region;
    wxString m_gameTitle;
    wxString m_videoFormat;
    wxString m_scannerID;
    int selectedInfo;
    wxSearchCtrl* searchBox; 
    
     enum{
    ID_Desc1=1000,
    ID_gameTitle=1001,
    ID_gameID=1002,
    ID_gameNum=1003,
    ID_Desc2=1004,
    ID_Desc3=1005,
    ID_RegionDesc=1006,
    ID_searchBox=1007
    //ID_line,
};
   
};


#endif
