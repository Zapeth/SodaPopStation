#include "popstationGUI.h"
#include "gameData.h"
#include "config.h"
#include "popstrip.h"
#include "popstation.h"

BEGIN_EVENT_TABLE(popFrame, wxFrame)
EVT_BUTTON(ID_PSButton, popFrame::onPSButton)
EVT_BUTTON(ID_OutputPBP, popFrame::onOutputPBP)
EVT_BUTTON(ID_ConvertButton, popFrame::onConvert)
EVT_BUTTON(ID_ExtractISOButton, popFrame::onExtract)
EVT_BUTTON(ID_GameDataButton, popFrame::onGDialog)
EVT_CHAR_HOOK(popFrame::onKeyPress)
EVT_CHOICE(ID_PSfile, popFrame::onDiscChange)
EVT_TEXT(wxID_ANY, popFrame::onTextEnter)
EVT_COMMAND_SCROLL(ID_CompressionSlider, popFrame::onCompressChange)
EVT_BUTTON(ID_buttonIconI, popFrame::onIconImage)
EVT_BUTTON(ID_buttonBackI, popFrame::onBackImage)
EVT_BUTTON(ID_buttonInfoI, popFrame::onInfoImage)
EVT_BUTTON(ID_buttonBackM, popFrame::onBackMusic)
EVT_BUTTON(ID_buttonIconA, popFrame::onIconAnim)
EVT_BUTTON(ID_buttonBootI, popFrame::onBootImage)
END_EVENT_TABLE()


popFrame::popFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size
        , long style)
: wxFrame((wxFrame*) parent, id, title, pos, size, style) {
    windowWithFocus=wxNOT_FOUND;
    gdDialog = new gameDataDialog(this, ID_gdDialog, wxS("Select a Game"), wxPoint(25, 25), wxSize(500, 550));
    MyNotebook = new wxNotebook(this, wxID_ANY, wxPoint(-1, -1), wxSize(600, 400), wxNB_DEFAULT);
	
    //Checks setting files
    //TODO
    confPanel=new configPanel(MyNotebook,ID_cfPanel,wxDefaultPosition,wxSize(600,400),wxTAB_TRAVERSAL);
    
    configoptions=confPanel->Read();
    MainPage();
    ExtraPage();
    AboutPage();
    
    MyNotebook->AddPage(MyPanel, wxS("Convert"), true);
    MyNotebook->AddPage(ExtraPanel, wxS("Custom"), false);
    MyNotebook->AddPage(confPanel,wxS("Config"),false);
    MyNotebook->AddPage(AboutPanel, wxS("About"), false);
    //wxPrintf("%s", configoptions.defaultImgSndFolder.c_str());
}

void popFrame::MainPage() {

    MyPanel = new wxPanel(MyNotebook, wxID_ANY, wxPoint(-1, -1), wxSize(600, 400), wxTAB_TRAVERSAL);
    infileDialog = new wxFileDialog(MyPanel, wxS("Select an PS format file"), configoptions.defaultInputFolder, wxS(""), wxS("PS files(*.iso;*.PBP;*.bin;*.img)|*.iso;*.ISO;*.pbp;*.PBP;*.bin;*.BIN;*.img;*.IMG|All Files|*.*"), wxFD_OPEN);
    outFolderDialog = new wxDirDialog(MyPanel, wxS("Choose a Folder"), configoptions.defaultOutputFolder);
    Desc1 = new wxStaticText(MyPanel, ID_Desc1, wxS("Input ISO/PBP"), wxPoint(15, 20), wxDefaultSize, 0);
    Desc2 = new wxStaticText(MyPanel, ID_Desc2, wxS("Output ISO/PBP"), wxPoint(310, 20), wxDefaultSize, 0);
    Desc3 = new wxStaticText(MyPanel, ID_Desc1, wxS("Game Title"), wxPoint(-1, -1), wxDefaultSize, 0);
    Desc4 = new wxStaticText(MyPanel, ID_Desc2, wxS("Save Title"), wxPoint(-1, -1), wxDefaultSize, 0);
    Desc5 = new wxStaticText(MyPanel, ID_Desc1, wxS("Game ID"), wxPoint(-1, -1), wxDefaultSize, 0);
    Desc6 = new wxStaticText(MyPanel, ID_Desc2, wxS("Save ID"), wxPoint(-1, -1), wxDefaultSize, 0);
    Desc7 = new wxStaticText(MyPanel, ID_Desc1, wxS("Compression Level"), wxPoint(-1, -1), wxSize(200, 25), 0);


    PSButton = new wxButton(MyPanel, ID_PSButton, wxS("..."), wxPoint(270, 45), wxSize(25, 25));
    FileSelection.Add(wxS("Disc 1 - No File Selected"));
    FileSelection.Add(wxS("Disc 2 - No File Selected"));
    FileSelection.Add(wxS("Disc 3 - No File Selected"));
    FileSelection.Add(wxS("Disc 4 - No File Selected"));
    FileSelection.Add(wxS("Disc 5 - No File Selected"));
    PSfiletext = new wxChoice(MyPanel, ID_PSfile, wxPoint(15, 45), wxSize(200, 25), FileSelection, wxCB_DROPDOWN);
    PSfiletext->Disable();
    //Emptys file selection array and sets compressionLevel to config file
    for (int i = 0; i < 5; ++i) {
        popParm[i].compressionLevel = configoptions.compressionLevel;
        FileSelection[i].Empty();
    }
    OutputPBP_tc = new wxTextCtrl(MyPanel, ID_OutputPBP_tc, wxS(""), wxPoint(310, 45), wxSize(200, 25), 0);
    OutputPBP = new wxButton(MyPanel, ID_OutputPBP, wxS("..."), wxPoint(565, 45), wxSize(25, 25));

    DiscTitle = new wxTextCtrl(MyPanel, ID_DiscTitle, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0);
    GameTitle = new wxTextCtrl(MyPanel, ID_GameTitle, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0);
    DiscIDcode = new wxTextCtrl(MyPanel, ID_DiscCode, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0);
    DiscIDnum = new wxTextCtrl(MyPanel, ID_DiscNum, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));
    GameIDcode = new wxTextCtrl(MyPanel, ID_GameCode, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0);
    GameIDnum = new wxTextCtrl(MyPanel, ID_GameNum, wxS(""), wxPoint(-1, -1), wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC));
    DiscIDnum->SetMaxLength(5);
    GameIDnum->SetMaxLength(5);

    gdDialogButton = new wxButton(MyPanel, ID_GameDataButton, wxS("..."), wxPoint(-1, -1), wxSize(25, 25));
    ConvertButton = new wxButton(MyPanel, ID_ConvertButton, wxS("Convert"), wxPoint(-1, -1), wxDefaultSize);
    ExtractISOButton = new wxButton(MyPanel, ID_ExtractISOButton, wxS("Extract ISO"), wxPoint(-1, -1), wxDefaultSize);
    CompressionSlider = new wxSlider(MyPanel, ID_CompressionSlider, configoptions.compressionLevel, 0, 9, wxPoint(-1, -1), wxSize(150, 40), wxSL_LABELS);
    
    sizer = new wxGridBagSizer(5, 5);
    sizer->Add(Desc1, wxGBPosition(0, 0), wxGBSpan(1, 4), wxALL | wxSHAPED, 0, NULL);

    sizer->Add(Desc2, wxGBPosition(0, 4), wxGBSpan(1, 2), wxALL | wxSHAPED, 0, NULL);

    sizer->Add(PSfiletext, wxGBPosition(1, 0), wxGBSpan(1, 3), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(PSButton, wxGBPosition(1, 3), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(OutputPBP_tc, wxGBPosition(1, 4), wxGBSpan(1, 3), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(OutputPBP, wxGBPosition(1, 7), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);

    sizer->Add(Desc3, wxGBPosition(2, 0), wxGBSpan(1, 2), wxALL | wxEXPAND, 0, NULL);

    sizer->Add(Desc4, wxGBPosition(2, 4), wxGBSpan(1, 1), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(DiscTitle, wxGBPosition(3, 0), wxGBSpan(1, 3), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(gdDialogButton, wxGBPosition(3, 3), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(GameTitle, wxGBPosition(3, 4), wxGBSpan(1, 3), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(Desc5, wxGBPosition(4, 0), wxGBSpan(1, 3), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(Desc6, wxGBPosition(4, 3), wxGBSpan(1, 1), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(DiscIDcode, wxGBPosition(5, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(DiscIDnum, wxGBPosition(6, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(GameIDcode, wxGBPosition(5, 3), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(GameIDnum, wxGBPosition(6, 3), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    sizer->Add(ConvertButton, wxGBPosition(7, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(ExtractISOButton, wxGBPosition(7, 3), wxGBSpan(1, 1), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(Desc7, wxGBPosition(9, 0), wxGBSpan(1, 2), wxALL | wxEXPAND, 0, NULL);
    sizer->Add(CompressionSlider, wxGBPosition(10, 0), wxGBSpan(2, 3), wxALL | wxEXPAND | wxALIGN_CENTER, 0, NULL);
    sizer->SetMinSize(600, 400);
    sizer->SetFlexibleDirection(wxBOTH);
    MyPanel->SetSizer(sizer);
    sizer->SetSizeHints(this);


    PSButton->MoveAfterInTabOrder(PSfiletext);
    gdDialogButton->MoveAfterInTabOrder(DiscTitle);
}

void popFrame::ExtraPage() {
    ExtraPanel = ExtraPanel = new wxPanel(MyNotebook, wxID_ANY, wxPoint(-1, -1), wxSize(600, 400), wxTAB_TRAVERSAL);
    ExtraSizer = new wxGridBagSizer(5, 5);
    IconImage = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("ICON0.png"));
    BackImage = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("PIC1.png"));
    InfoImage = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("PIC0.png"));
    BackMusic = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("SND0.at3"));
    IconAnim = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("ICON1.pmf"));
    BootImage = new wxStaticBoxSizer(wxHORIZONTAL, ExtraPanel, wxS("BOOT.png"));
    textIconI = new wxTextCtrl(ExtraPanel, ID_IconImage, wxEmptyString);
    textBackI = new wxTextCtrl(ExtraPanel, ID_BackImage, wxEmptyString);
    textInfoI = new wxTextCtrl(ExtraPanel, ID_InfoImage, wxEmptyString);
    textBackM = new wxTextCtrl(ExtraPanel, ID_BackMusic, wxEmptyString);
    textIconA = new wxTextCtrl(ExtraPanel, ID_IconAnim, wxEmptyString);
    textBootI = new wxTextCtrl(ExtraPanel, ID_BootImage, wxEmptyString);
    buttonIconI = new wxButton(ExtraPanel, ID_buttonIconI, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    buttonBackI = new wxButton(ExtraPanel, ID_buttonBackI, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    buttonInfoI = new wxButton(ExtraPanel, ID_buttonInfoI, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    buttonBackM = new wxButton(ExtraPanel, ID_buttonBackM, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    buttonIconA = new wxButton(ExtraPanel, ID_buttonIconA, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    buttonBootI = new wxButton(ExtraPanel, ID_buttonBootI, wxS("..."), wxDefaultPosition, wxSize(30, 30));
    fileIconI = new wxFileDialog(ExtraPanel, wxS("ICON0.png"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("PNG file(*.png)|*.png;*.PNG|All Files|*"), wxFD_OPEN);
    fileBackI = new wxFileDialog(ExtraPanel, wxS("PIC1.png"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("PNG file(*.png)|*.png;*.PNG|All Files|*"), wxFD_OPEN);
    fileInfoI = new wxFileDialog(ExtraPanel, wxS("PIC0.png"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("PNG file(*.png)|*.png;*.PNG|All Files|*"), wxFD_OPEN);
    fileBackM = new wxFileDialog(ExtraPanel, wxS("SND0.AT3"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("AT3 file(*.at3)|*.at3;*.AT3|All Files|*"), wxFD_OPEN);
    fileIconA = new wxFileDialog(ExtraPanel, wxS("ICON1.pmf"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("PMF file(*.pmf)|*.pmf;*.PMF|All Files|*"), wxFD_OPEN);
    fileBootI = new wxFileDialog(ExtraPanel, wxS("BOOT.png"), configoptions.defaultImgSndFolder, wxEmptyString,
            wxS("PNG file(*.png)|*.png;*.PNG|All Files|*"), wxFD_OPEN);
    IconImage->Add(textIconI, 10, wxEXPAND);
    IconImage->Add(buttonIconI, 1, wxSHAPED);
    BackImage->Add(textBackI, 10, wxEXPAND);
    BackImage->Add(buttonBackI, 1, wxSHAPED);
    InfoImage->Add(textInfoI, 10, wxEXPAND);
    InfoImage->Add(buttonInfoI, 1, wxSHAPED);
    BackMusic->Add(textBackM, 10, wxEXPAND);
    BackMusic->Add(buttonBackM, 1, wxSHAPED);
    IconAnim->Add(textIconA, 10, wxEXPAND);
    IconAnim->Add(buttonIconA, 1, wxSHAPED);
    BootImage->Add(textBootI, 10, wxEXPAND);
    BootImage->Add(buttonBootI, 1, wxSHAPED);
    ExtraSizer->Add(IconImage, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraSizer->Add(BackImage, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraSizer->Add(InfoImage, wxGBPosition(2, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraSizer->Add(BackMusic, wxGBPosition(3, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraSizer->Add(IconAnim, wxGBPosition(4, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraSizer->Add(BootImage, wxGBPosition(5, 0), wxGBSpan(1, 1), wxALL | wxSHAPED, 0, NULL);
    ExtraPanel->SetSizer(ExtraSizer);
}
void popFrame::AboutPage(){
    AboutPanel=new wxPanel(MyNotebook, wxID_ANY,wxDefaultPosition,wxSize(600,400),wxTAB_TRAVERSAL);
    AboutText=new wxStaticText(AboutPanel, ID_AboutText,wxS(""),wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    AboutText->SetLabel(wxS("Credits:\n\nDark_Alex - For the popstation source for conversion of PSX files\n\
SeattleMan(www.sonyindex.com) - The popstation GUI game database\n\
flatwhatson - For the popstrip extract source code which allows extraction of EBOOTS\n\
KingSquitter - His PSX2PSP source code for popsation and popstrip helped immensely\n"));
}

void popFrame::onPSButton(wxCommandEvent& WXUNUSED(event)) {
    if (infileDialog->ShowModal() == wxID_OK) {
        int CurrentDisc = PSfiletext->GetCurrentSelection();//CurrentDisc holds the the disc that is currently selected
        wxString ISOname;
        ISOname.Printf(wxS("Disc %d - "), CurrentDisc + 1);

        //Automatic GameID detection if the no disc has been selected
        if (FileSelection[CurrentDisc].IsEmpty())
            gameidDetection(infileDialog->GetPath(), CurrentDisc);
        FileSelection[CurrentDisc] = infileDialog->GetPath();
        PSfiletext->SetString(CurrentDisc, ISOname + infileDialog->GetFilename());
        PSfiletext->Enable(true);
    }
}

void popFrame::onOutputPBP(wxCommandEvent& WXUNUSED(event)) {
    if (outFolderDialog->ShowModal() == wxID_OK)
        OutputPBP_tc->SetValue(outFolderDialog->GetPath());
}

void popFrame::onConvert(wxCommandEvent& WXUNUSED(event)) {
    int num_of_disc_to_convert = 0;
    wxString temp;
    for (int i = 0; i < 5; ++i)
        if (!FileSelection[i].IsEmpty())
            num_of_disc_to_convert++;

    //Error Checking
    if (num_of_disc_to_convert == 0) {
        wxMessageBox(wxS("No ISO/BIN file selected"));
        return;
    }
    if (!wxFileExists(wxS("BASE.PBP"))) {
        wxMessageBox(wxS("Cannot Convert - NO BASE.PBP"));
        return;
    }
    if (popParm[0].saveDesc.IsEmpty()) {
        wxMessageBox(wxS("Please enter a save title"));
        return;
    }
    if (popParm[0].saveIDcode.IsEmpty()) {
        wxMessageBox(wxS("Please enter a saveID code. Ex.(SLUS)"));
        return;
    }
    if (popParm[0].saveIDnum.IsEmpty()) {
        wxMessageBox(wxS("Please enter a saveID number Ex.(94711)"));
        return;
    }
    if (popParm[0].gameTitle.IsEmpty()) {
        wxMessageBox(wxS("Please enter a game title"));
        return;
    }
    if (popParm[0].gameIDcode.IsEmpty()) {
        wxMessageBox(wxS("Please enter a gameID code. Ex.(SLUS)"));
        return;
    }
    if (popParm[0].gameIDnum.IsEmpty()) {
        wxMessageBox(wxS("Please enter a gameID number Ex.(94711)"));
        return;
    }
    //End of Error Checking

    ConvertIsoInfo convertInfo;
    convertInfo.base = wxS("BASE.PBP");
    if (configoptions.useDataPSP)
        convertInfo.data_psp = wxS("DATA.PSP");

    convertInfo.icon0 = textIconI->GetValue();
    convertInfo.pic1 = textBackI->GetValue();
    convertInfo.pic0 = textInfoI->GetValue();
    convertInfo.snd0 = textBackM->GetValue();
    convertInfo.icon1 = textIconA->GetValue();
    convertInfo.boot = textBootI->GetValue();
    wxFileName outputfile(OutputPBP_tc->GetValue(), wxS("EBOOT.PBP"));
    if (!wxDirExists(outputfile.GetPath()))
        wxMkdir(outputfile.GetPath());
    temp = outputfile.GetFullPath();

    convertInfo.dstPBP = temp;
    convertInfo.saveTitle = popParm[0].saveDesc;
    convertInfo.saveID = popParm[0].saveID;
    convertInfo.tocSize = 0;
    convertInfo.patchCount = 0;
    convertInfo.multiDiscInfo.fileCount = (char) num_of_disc_to_convert;

    convertInfo.srcISO = FileSelection[0];
    convertInfo.gameTitle = popParm[0].gameTitle;
    convertInfo.gameID = popParm[0].gameID;
    convertInfo.compLevel = popParm[0].compressionLevel;
	
    if(configoptions.rmRegion>0){
        wxRegEx removeRegion(wxS("^(.*)\\[.+\\].*$"), wxRE_EXTENDED);
        if(removeRegion.Matches(convertInfo.gameTitle))
             convertInfo.gameTitle=removeRegion.GetMatch(convertInfo.gameTitle,1).Trim();
   }
    //Single Disc Game
    if (num_of_disc_to_convert < 2) {
        convertInfo.isMultiDisc = false;
        if (convertInfo.srcISO.EndsWith(wxS(".pbp")) || convertInfo.srcISO.EndsWith(wxS(".PBP")))
            convertInfo.srcIsPbp = true;
        else
            convertInfo.srcIsPbp = false;

    }        //MultiDisc Game
    else {
        convertInfo.isMultiDisc = true;
        for (int i = 0; i < 5; ++i) {
            temp = FileSelection[i];

            //If a file exist in this slot then proceed to add it to the multidisc info
            if (!temp.IsEmpty())
                convertInfo.multiDiscInfo.srcISO.Add(temp);
            //If the gameTitle for a particular disc is empty then quit and tell the user
            if (popParm[i].gameTitle.IsEmpty()) {
                wxMessageBox(wxS("Please enter a game title"));
                return;
            }
            convertInfo.multiDiscInfo.gameTitle.Add(popParm[i].gameTitle);

            if (popParm[i].gameIDcode.IsEmpty()) {
                wxMessageBox(wxS("Please enter a gameID code Ex.(SLUS)"));
                return;
            }
            if (popParm[i].gameIDnum.IsEmpty()) {
                wxMessageBox(wxS("Please enter a gameID number Ex.(94711)"));
                return;
            }

            convertInfo.multiDiscInfo.gameID.Add(popParm[i].gameID);
            convertInfo.multiDiscInfo.compLevel[i] = popParm[i].compressionLevel;

        }
    }
    popstationConvert(convertInfo);
}

void popFrame::gameidDetection(const wxString source_file, int CurrentDisc) {
    wxFFile infile;
    char buffer[16];
    wxString filedata, gameid_data;//Filedata is a 32 bit window that holds the previous 16 bytes plus the 16 bytes read in
    infile.Open(source_file, wxS("rb"));
    if (infile.IsOpened() == false)
        return;
    wxArrayString prefixlist = gdDialog->PrefixList();
    bool gameid_found = false;
    size_t find_position;
    //Read from input file in increaments 16 bytes until the gameid is found or 16 bytes can no longer be rea
    while (infile.Read(buffer, 16) == 16) {
        
        filedata = filedata.Right(16)+wxString::From8BitData(buffer,16);
        for (size_t i=0; i < prefixlist.GetCount(); i++) {
            find_position = filedata.find(prefixlist[i]);
            if (find_position != wxString::npos) {//The gameid prefix has been found in the iso
                gameid_found = true;
                long pos=find_position - filedata.length();
                infile.Seek(pos, wxFromCurrent);
                memset(buffer,0,16);
                infile.Read(buffer, prefixlist[i].length() + 7);//Reads in the prefix length plus the next 7 characters
                gameid_data = wxString::FromAscii(buffer);
                               
                size_t token = gameid_data.find('.'); //Finds and removes '.' and '_'char if it exist in the gameid
                if (token!=wxString::npos)
                    gameid_data.erase(token, 1);
                token=gameid_data.find('_');
                if(token!=wxString::npos)
            	     gameid_data.erase(token,1);
                break;
            }
            
        }
        if (gameid_found){
                        
            gdDialog->SearchID(gameid_data,popParm[CurrentDisc]);
            if (CurrentDisc == 0) {
                GameTitle->SetValue(popParm[CurrentDisc].saveDesc);
                GameIDcode->SetValue(popParm[CurrentDisc].saveIDcode);
                GameIDnum->SetValue(popParm[CurrentDisc].saveIDnum);
                if (OutputPBP_tc->IsEmpty())
                    OutputPBP_tc->SetValue(popParm[CurrentDisc].saveDesc + wxS(" ") + popParm[CurrentDisc].saveID);
            }
            DiscTitle->SetValue(popParm[CurrentDisc].gameTitle);
            DiscIDcode->SetValue(popParm[CurrentDisc].gameIDcode);
            DiscIDnum->SetValue(popParm[CurrentDisc].gameIDnum);
            break;
        }
    }
        
    infile.Close();
}

void popFrame::onGDialog(wxCommandEvent& WXUNUSED(event)) {
    int CurrentDisc = PSfiletext->GetCurrentSelection();
    if (gdDialog->ShowModal() == wxID_OK) {
        popParm[CurrentDisc].saveIDcode = gdDialog->getSaveIDcode();
        popParm[CurrentDisc].saveIDnum = gdDialog->getSaveIDnum();
        popParm[CurrentDisc].saveID = gdDialog->getSaveFolder();
        popParm[CurrentDisc].saveDesc = gdDialog->getSaveDesc();
        popParm[CurrentDisc].region = gdDialog->getRegion();
        popParm[CurrentDisc].gameTitle = gdDialog->getGameTitle();
        popParm[CurrentDisc].gameIDcode = gdDialog->getGameIDcode();
        popParm[CurrentDisc].gameIDnum = gdDialog->getGameIDnum();
        popParm[CurrentDisc].gameID = gdDialog->getScannerID();


        if (CurrentDisc == 0) {
            GameTitle->SetValue(popParm[CurrentDisc].saveDesc);
            GameIDcode->SetValue(popParm[CurrentDisc].saveIDcode);
            GameIDnum->SetValue(popParm[CurrentDisc].saveIDnum);
            if (OutputPBP_tc->IsEmpty())
                OutputPBP_tc->SetValue(popParm[CurrentDisc].saveDesc + wxS(" ") + popParm[CurrentDisc].saveID);
        }
        DiscTitle->SetValue(popParm[CurrentDisc].gameTitle);
        DiscIDcode->SetValue(popParm[CurrentDisc].gameIDcode);
        DiscIDnum->SetValue(popParm[CurrentDisc].gameIDnum);
    }

}

void popFrame::onKeyPress(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_DOWN && PSfiletext->GetId()==windowWithFocus) {
        int CurrentDisc = PSfiletext->GetCurrentSelection();
        CurrentDisc = (1 + CurrentDisc) % 5;
        PSfiletext->SetSelection(CurrentDisc);
        DiscTitle->SetValue(popParm[CurrentDisc].gameTitle);
        DiscIDcode->SetValue(popParm[CurrentDisc].gameIDcode);
        DiscIDnum->SetValue(popParm[CurrentDisc].gameIDnum);
        CompressionSlider->SetValue(popParm[CurrentDisc].compressionLevel);
    } else if (event.GetKeyCode() == WXK_UP && PSfiletext->GetId()==windowWithFocus) {
        int CurrentDisc = PSfiletext->GetCurrentSelection();
        CurrentDisc--;
        if (CurrentDisc < 0)
            CurrentDisc = 4;
        PSfiletext->SetSelection(CurrentDisc);

        DiscTitle->SetValue(popParm[CurrentDisc].gameTitle);
        DiscIDcode->SetValue(popParm[CurrentDisc].gameIDcode);
        DiscIDnum->SetValue(popParm[CurrentDisc].gameIDnum);
        CompressionSlider->SetValue(popParm[CurrentDisc].compressionLevel);
    } else if (event.GetKeyCode() == WXK_DELETE && PSfiletext->GetId()==windowWithFocus) {
        int CurrentDisc = PSfiletext->GetCurrentSelection();
        if (CurrentDisc > 0) {
            int answer = wxMessageBox(wxS("Would you like to remove ") + FileSelection[CurrentDisc], wxS("Disc Removal"), wxYES_NO | wxNO_DEFAULT);
            if (answer == wxYES) {
                FileSelection[CurrentDisc] = wxEmptyString;
                PSfiletext->SetString(CurrentDisc, wxString::Format(wxS("Disc %i - No File Selected"), CurrentDisc + 1));
                DiscTitle->SetValue(wxEmptyString);
                DiscIDcode->SetValue(wxEmptyString);
                DiscIDnum->SetValue(wxEmptyString);
                CompressionSlider->SetValue(configoptions.compressionLevel);
                gameData tempinfo = {wxS(""),wxS(""),wxS(""),wxS(""),wxS(""),wxS(""),wxS(""),wxS(""),wxS(""), configoptions.compressionLevel};
                popParm[CurrentDisc] = tempinfo;
            }
        }
    }
    event.Skip();

}

void popFrame::onDiscChange(wxCommandEvent& WXUNUSED(event)) {
    int CurrentDisc = PSfiletext->GetCurrentSelection();
    DiscTitle->SetValue(popParm[CurrentDisc].gameTitle);
    DiscIDcode->SetValue(popParm[CurrentDisc].gameIDcode);
    DiscIDnum->SetValue(popParm[CurrentDisc].gameIDnum);
    CompressionSlider->SetValue(popParm[CurrentDisc].compressionLevel);
}

void popFrame::onTextEnter(wxCommandEvent& (event)) {
    int eventID = event.GetId();
    int CurrentDisc = PSfiletext->GetCurrentSelection();
    if (eventID == DiscTitle->GetId())
        popParm[CurrentDisc].gameTitle = DiscTitle->GetValue();
    else if (eventID == DiscIDcode->GetId())
        popParm[CurrentDisc].gameIDcode = DiscIDcode->GetValue();
    else if (eventID == DiscIDnum->GetId())
        popParm[CurrentDisc].gameIDnum = DiscIDnum->GetValue();
    else if (eventID == GameTitle->GetId())//SaveDesc
        popParm[0].saveDesc = GameTitle->GetValue();
    else if (eventID == GameIDcode->GetId())//CurrentDisc=0 means that the main disc is selected
        popParm[0].saveIDcode = GameIDcode->GetValue();
    else if (eventID == GameIDnum->GetId())
        popParm[0].saveIDnum = GameIDnum->GetValue();
    popParm[CurrentDisc].gameID = popParm[CurrentDisc].gameIDcode + popParm[CurrentDisc].gameIDnum;
    popParm[CurrentDisc].saveID = popParm[CurrentDisc].saveIDcode + popParm[CurrentDisc].saveIDnum;
}

void popFrame::onCompressChange(wxScrollEvent& event) {
    int CurrentDisc = PSfiletext->GetCurrentSelection();
    popParm[CurrentDisc].compressionLevel = CompressionSlider->GetValue();
}

void popFrame::onIconImage(wxCommandEvent& event) {
    if (fileIconI->ShowModal() == wxID_OK)
        textIconI->SetValue(fileIconI->GetPath());
    //Sets the Directory of the other dialogs to the one just open
    //Found out it is inconvinent to move to a remote directory every time
    if (!textIconI->GetValue().IsEmpty()) {
        fileBackI->SetDirectory(fileIconI->GetDirectory());
        fileInfoI->SetDirectory(fileIconI->GetDirectory());
        fileBackM->SetDirectory(fileIconI->GetDirectory());
        fileIconA->SetDirectory(fileIconI->GetDirectory());
        fileBootI->SetDirectory(fileIconI->GetDirectory());
    }
}

void popFrame::onBackImage(wxCommandEvent& event) {
    if (fileBackI->ShowModal() == wxID_OK)
        textBackI->SetValue(fileBackI->GetPath());
    if (!textBackI->GetValue().IsEmpty()) {
        fileIconI->SetDirectory(fileBackI->GetDirectory());
        fileInfoI->SetDirectory(fileBackI->GetDirectory());
        fileBackM->SetDirectory(fileBackI->GetDirectory());
        fileIconA->SetDirectory(fileBackI->GetDirectory());
        fileBootI->SetDirectory(fileBackI->GetDirectory());
    }
}

void popFrame::onInfoImage(wxCommandEvent& event) {
    if (fileInfoI->ShowModal() == wxID_OK)
        textInfoI->SetValue(fileInfoI->GetPath());
    if (!textInfoI->GetValue().IsEmpty()) {
        fileIconI->SetDirectory(fileInfoI->GetDirectory());
        fileBackI->SetDirectory(fileInfoI->GetDirectory());
        fileBackM->SetDirectory(fileInfoI->GetDirectory());
        fileIconA->SetDirectory(fileInfoI->GetDirectory());
        fileBootI->SetDirectory(fileInfoI->GetDirectory());
    }

}

void popFrame::onBackMusic(wxCommandEvent& event) {
    if (fileBackM->ShowModal() == wxID_OK)
        textBackM->SetValue(fileBackM->GetPath());
    if (!textBackM->GetValue().IsEmpty()) {
        fileIconI->SetDirectory(fileBackM->GetDirectory());
        fileBackI->SetDirectory(fileBackM->GetDirectory());
        fileInfoI->SetDirectory(fileBackM->GetDirectory());
        fileIconA->SetDirectory(fileBackM->GetDirectory());
        fileBootI->SetDirectory(fileBackM->GetDirectory());
    }
}

void popFrame::onIconAnim(wxCommandEvent& event) {
    if (fileIconA->ShowModal() == wxID_OK)
        textIconA->SetValue(fileIconA->GetPath());
    if (!textIconA->GetValue().IsEmpty()) {
        fileIconI->SetDirectory(fileIconA->GetDirectory());
        fileBackI->SetDirectory(fileIconA->GetDirectory());
        fileInfoI->SetDirectory(fileIconA->GetDirectory());
        fileBackM->SetDirectory(fileIconA->GetDirectory());
        fileBootI->SetDirectory(fileIconA->GetDirectory());
    }
}

void popFrame::onBootImage(wxCommandEvent& event) {
    if (fileBootI->ShowModal() == wxID_OK)
        textBootI->SetValue(fileBootI->GetPath());
    if (!textBootI->GetValue().IsEmpty()) {
        fileIconI->SetDirectory(fileBootI->GetDirectory());
        fileBackI->SetDirectory(fileBootI->GetDirectory());
        fileInfoI->SetDirectory(fileBootI->GetDirectory());
        fileBackM->SetDirectory(fileBootI->GetDirectory());
        fileIconA->SetDirectory(fileBootI->GetDirectory());
    }
}

void popFrame::onExtract(wxCommandEvent& event) {
    int num_of_disc_to_convert = 0;
    wxString srcPBP, outISO;
    for (int i = 0; i < 5; ++i)
        if (!FileSelection[i].IsEmpty())
            num_of_disc_to_convert++;
    if (num_of_disc_to_convert == 0) {
        wxMessageBox(wxS("No PBP file to extract"));
        return;
    }
    srcPBP = FileSelection[0];
    if (!srcPBP.EndsWith(wxS(".pbp")) && !srcPBP.EndsWith(wxS(".PBP"))) {
        wxMessageBox(wxS("Not a PBP file"));
        return;
    }
    if (!OutputPBP_tc->IsEmpty())
        outISO = OutputPBP_tc->GetValue();
    else
        outISO = srcPBP + wxS(".iso");

    popstripExtract(srcPBP, outISO);

}

void popFrame::focusFind(wxFocusEvent& event){
	windowWithFocus=event.GetId();
	event.Skip();
}
