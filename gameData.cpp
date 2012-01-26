#include <cstdlib>
#include <iterator>

#include "gameData.h"

#include "wx/hashset.h"


BEGIN_EVENT_TABLE(gameDataDialog, wxDialog)
EVT_LISTBOX(ID_gameTitle, gameDataDialog::onGameSelect)
EVT_CHAR_HOOK(gameDataDialog::onChar)
EVT_BUTTON(wxID_OK, gameDataDialog::onOK)
EVT_TEXT(ID_searchBox, gameDataDialog::SearchUpdate)
EVT_TEXT_ENTER(ID_searchBox, gameDataDialog::Search)
EVT_SEARCHCTRL_SEARCH_BTN(ID_searchBox, gameDataDialog::Search)
EVT_SET_FOCUS(gameDataDialog::setFocus)
END_EVENT_TABLE()

WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, MyPrefix);
//WX_DECLARE_STRING_HASH_MAP( wxString, MyPrefix );

gameDataDialog::gameDataDialog(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style)
: wxDialog(parent, id, title, pos, size, style) {
	windowWithFocus=wxNOT_FOUND;
    selectedInfo = wxNOT_FOUND;
    Desc1 = new wxStaticText(this, ID_Desc1, wxS("Game Title"), wxPoint(0, 20), wxDefaultSize, 0);
    Desc2 = new wxStaticText(this, ID_Desc2, wxS("Game ID"), wxPoint(330, 80), wxDefaultSize, 0);
    Desc3 = new wxStaticText(this, ID_Desc3, wxS("Region"), wxPoint(330, 230), wxDefaultSize, 0);
    RegionDesc = new wxStaticText(this, ID_RegionDesc, wxS(""), wxPoint(330, 265), wxDefaultSize);
    //line=new wxStaticLine(this,wxID_ANY,wxPoint(0,380),wxSize(-1,-1),wxLI_HORIZONTAL);

    gameName = new wxListBox(this, ID_gameTitle, wxPoint(0, 80), wxSize(300, 350),
           0,NULL, wxLB_SINGLE);

    gameID = new wxChoice(this, ID_gameID, wxPoint(330, 115), wxSize(75, 30));
    gameNum = new wxTextCtrl(this, ID_gameID, wxS(""), wxPoint(415, 115), wxSize(50, 30), 0, wxTextValidator(wxFILTER_NUMERIC));
    gameNum->SetMaxLength(5);
    searchBox = new wxSearchCtrl(this, ID_searchBox, wxS(""), wxPoint(0, 50), wxSize(300, -1), wxTE_PROCESS_ENTER);
    searchBox->ShowSearchButton(true);
    generateGameList();
    sizer = CreateButtonSizer(wxOK | wxCANCEL | wxNO_DEFAULT);
    sizer->SetDimension(300, 480, -1, -1);
    searchBox->SetFocus();


}

void gameDataDialog::onGameSelect(wxCommandEvent&event) {
    selectedInfo = gameName->GetSelection();
    int result = gameID->FindString(gameInfo.gameIDcode[selectedInfo]);
    gameNum->SetValue(gameInfo.gameIDnum[selectedInfo]);
    if (result != wxNOT_FOUND)
        gameID->SetSelection(result);
    RegionDesc->SetLabel(gameInfo.region[selectedInfo]);

    event.Skip();
}

wxArrayString gameDataDialog::PrefixList(){
	return gameID->GetStrings();
}

void gameDataDialog::generateGameList() {

    gameInfo.gameIDcode.Alloc(10152);
    gameInfo.gameIDnum.Alloc(10152);
    gameInfo.gameTitle.Alloc(10152);
    gameInfo.region.Alloc(10152);
    gameInfo.saveDesc.Alloc(10152);
    gameInfo.saveFolder.Alloc(10152);
    gameInfo.scannerID.Alloc(10152);
    gameInfo.videoFormat.Alloc(10152);

    wxFileInputStream input(wxS("gameinfo.txt"));

    if (!input.IsOk() || input.Eof()) {
        Close();
        return;
    }

    wxTextInputStream infile(input);

    wxString dataline;
    wxArrayString lineTokenizer;
    MyPrefix codeprefix;
    wxRegEx TitleRegion(wxS("^(.*)(\\[.+\\]).*$"), wxRE_EXTENDED); //Removes the region from the game saveDesc
    wxRegEx SaveSplit(wxS("^([[:alpha:]]+)(.*)"), wxRE_EXTENDED);
    dataline = infile.ReadLine();

    while (input.IsOk() && !input.Eof()) {

        lineTokenizer = wxStringTokenize(dataline, wxS(";"), wxTOKEN_RET_EMPTY_ALL);

        wxArrayString fgameID = wxStringTokenize(lineTokenizer[tGameID], wxS("-"), wxTOKEN_RET_EMPTY_ALL); //Splits GameId form:"<code>-<num>" into full game ID

        codeprefix.insert(fgameID[0]);
        gameInfo.gameIDcode.Add(fgameID[0]);
        if(fgameID.size()>1)
            gameInfo.gameIDnum.Add(fgameID[1]);
        else
            gameInfo.gameIDnum.Add(wxT("NULL"));
       
        if (SaveSplit.Matches(lineTokenizer[tSaveFolder])) {


            gameInfo.saveIDcode.Add(SaveSplit.GetMatch(lineTokenizer[tSaveFolder], 1));
            gameInfo.saveIDnum.Add(SaveSplit.GetMatch(lineTokenizer[tSaveFolder], 2));
        }
        gameInfo.saveFolder.Add(lineTokenizer[tSaveFolder]);
        if (TitleRegion.Matches(lineTokenizer[tSaveDesc])) {


            gameInfo.saveDesc.Add(TitleRegion.GetMatch(lineTokenizer[tSaveDesc], 1).Trim());
            gameInfo.region.Add(TitleRegion.GetMatch(lineTokenizer[tSaveDesc], 2));
        }

        gameInfo.gameTitle.Add(lineTokenizer[tGameName]);
        gameInfo.videoFormat.Add(lineTokenizer[tVideoFormat]);
        gameInfo.scannerID.Add(lineTokenizer[tScanID]);
        dataline = infile.ReadLine();
    }
    gameName->Set(gameInfo.gameTitle);

    for (MyPrefix::iterator it = codeprefix.begin(); it != codeprefix.end(); ++it) {
        gameID->Append(*it);
    }
    gameName->SetSelection(0);
}

void gameDataDialog::onOK(wxCommandEvent& WXUNUSED(event)) {
    m_gameIDcode = gameID->GetStringSelection();
    m_gameIDnum = gameNum->GetValue();
    m_region = RegionDesc->GetLabel();
    if (selectedInfo != wxNOT_FOUND) {
        m_saveIDcode= gameInfo.saveIDcode[selectedInfo];
        m_saveIDnum= gameInfo.saveIDnum[selectedInfo];
        m_saveFolder = gameInfo.saveFolder[selectedInfo];
        m_saveDesc = gameInfo.saveDesc[selectedInfo];
        m_gameTitle = gameInfo.gameTitle[selectedInfo];
        m_videoFormat = gameInfo.videoFormat[selectedInfo];
        m_scannerID = gameInfo.scannerID[selectedInfo];
    }


    EndModal(wxID_OK);
}

void gameDataDialog::onChar(wxKeyEvent& event) {
if (gameName->GetId()==windowWithFocus) {
        wxString text_to_search = (wxChar) event.GetKeyCode();
        int selection = 0;
        selection = gameName->GetSelection();
        int count = gameName->GetCount();

        if (count != 0) {
            for (int i = selection + 1; i != selection; i = (i+1) % count) {
                wxString choice = gameName->GetString(i).Lower();

                if (choice.StartsWith(text_to_search)) {

                    gameName->SetSelection(i);
                    break;
                }
            }

        }
    }
    
    event.Skip();
}

void gameDataDialog::setFocus(wxFocusEvent& event){
	windowWithFocus=event.GetId();
	event.Skip();
}
void gameDataDialog::Search(wxCommandEvent& event) {
    wxString text_to_search = searchBox->GetValue().Lower();
    int selection = 0;
    selection = gameName->GetSelection();
    int count = gameName->GetCount();

    if (count != 0) {
        for (int i = selection + 1; i != selection; i = (i+1) % count) {
            wxString choice = gameName->GetString(i).Lower();

            if (choice.StartsWith(text_to_search)) {
                gameName->SetSelection(i);
                selectedInfo = gameName->GetSelection();
                int result = gameID->FindString(gameInfo.gameIDcode[selectedInfo]);
                gameNum->SetValue(gameInfo.gameIDnum[selectedInfo]);
                if (result != wxNOT_FOUND)
                    gameID->SetSelection(result);
                RegionDesc->SetLabel(gameInfo.region[selectedInfo]);
                break;
            }
        }
    }
    event.Skip();
}

void gameDataDialog::SearchUpdate(wxCommandEvent& event) {
    wxString text_to_search = searchBox->GetValue().Lower();
    int selection = 0;
    selection = gameName->GetSelection();
    int count = gameName->GetCount();
    wxString choice;
    int num = 0;
    if (count != 0) {
        for (int i = selection; num < count; i = (1 + i) % count, num++) {
            choice  = gameName->GetString(i).Lower();

            if (choice.StartsWith(text_to_search)) {
                //std::cout <<(char*)choice.char_str()<<'\n';
                gameName->SetSelection(i);
                selectedInfo = gameName->GetSelection();
                int result = gameID->FindString(gameInfo.gameIDcode[selectedInfo]);
                gameNum->SetValue(gameInfo.gameIDnum[selectedInfo]);
                if (result != wxNOT_FOUND)
                    gameID->SetSelection(result);
                RegionDesc->SetLabel(gameInfo.region[selectedInfo]);
                break;
            }
        }
    }
    event.Skip();
}

void gameDataDialog::SearchID(const wxString& searchID, gameData& metaData){
    
    for(unsigned int i=0;i<gameInfo.scannerID.GetCount();i++){
         if(searchID==gameInfo.scannerID[i]){
             metaData.gameID=gameInfo.scannerID[i];
             metaData.gameIDcode=gameInfo.gameIDcode[i];
             metaData.gameIDnum=gameInfo.gameIDnum[i];
             metaData.saveID=gameInfo.saveFolder[i];
             metaData.saveIDcode=gameInfo.saveIDcode[i];
             metaData.saveIDnum=gameInfo.saveIDnum[i];
             metaData.saveDesc=gameInfo.saveDesc[i];
             metaData.region=gameInfo.region[i];
             metaData.gameTitle=gameInfo.gameTitle[i];
             break;
         }
         
    }
    return;
}
