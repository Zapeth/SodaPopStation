#include "config.h"


BEGIN_EVENT_TABLE(configPanel, wxPanel)
EVT_BUTTON(ID_UpdateOptions, configPanel::Write)

END_EVENT_TABLE()

configPanel::configPanel(wxWindow* parent, wxWindowID id, const wxPoint&  pos, const wxSize&  size, long style)
	:wxPanel(parent,id,pos,size,style){
		configfile=new wxFileConfig(wxEmptyString,wxEmptyString,wxS("settings.ini"),wxEmptyString,
				wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_RELATIVE_PATH);
		wxConfigBase::Set(configfile);
		configfile->SetRecordDefaults(true);
		Read();
}
configPanel::~configPanel(){
	
}
settings configPanel::Read() {
bool IsRead;
IsRead=configfile->Read(wxS("compressionLevel"),&configoptions.compressionLevel,9);
IsRead=configfile->Read(wxS("customIcon0"),&configoptions.customIcon0,wxEmptyString);
IsRead=configfile->Read(wxS("customPic0"),&configoptions.customPic0,wxEmptyString);
IsRead=configfile->Read(wxS("customPic1"),&configoptions.customPic1,wxEmptyString);
IsRead=configfile->Read(wxS("customBoot"),&configoptions.customBoot,wxEmptyString);
IsRead=configfile->Read(wxS("useDataPSP"),&configoptions.useDataPSP,true);
IsRead=configfile->Read(wxS("custom-images-for-PBP"),&configoptions.custImgPBP,true);
IsRead=configfile->Read(wxS("remove-region"),&configoptions.rmRegion,true);
IsRead=configfile->Read(wxS("input-ISO_PBP-folder"),&configoptions.defaultInputFolder,wxEmptyString);
IsRead=configfile->Read(wxS("output-ISO_PBP-folder"),&configoptions.defaultOutputFolder,wxEmptyString);
IsRead=configfile->Read(wxS("image-sound-folder"),&configoptions.defaultImgSndFolder,wxEmptyString);
IsRead=configfile->Read(wxS("icon0"),&configoptions.icon0file,wxEmptyString);
IsRead=configfile->Read(wxS("icon1"),&configoptions.icon1file,wxEmptyString);
IsRead=configfile->Read(wxS("pic0"),&configoptions.pic0file,wxEmptyString);
IsRead=configfile->Read(wxS("pic1"),&configoptions.pic0file,wxEmptyString);
IsRead=configfile->Read(wxS("snd0"),&configoptions.snd0file,wxEmptyString);
IsRead=configfile->Read(wxS("boot"),&configoptions.bootfile,wxEmptyString);
return configoptions;
}
void configPanel::Write(wxCommandEvent& event){
settings options;
bool IsWritten;
IsWritten=configfile->Write(wxS("compressionLevel"),options.compressionLevel);
IsWritten=configfile->Write(wxS("customIcon0"),options.customIcon0);
IsWritten=configfile->Write(wxS("customPic0"),options.customPic0);
IsWritten=configfile->Write(wxS("customPic1"),options.customPic1);
IsWritten=configfile->Write(wxS("customBoot"),options.customBoot);
IsWritten=configfile->Write(wxS("useDataPSP"),options.useDataPSP);
IsWritten=configfile->Write(wxS("custom-images-for-PBP"),options.custImgPBP);
IsWritten=configfile->Write(wxS("remove-region"),options.rmRegion);
IsWritten=configfile->Write(wxS("input-ISO_PBP-folder"),options.defaultInputFolder);
IsWritten=configfile->Write(wxS("output-ISO_PBP-folder"),options.defaultOutputFolder);
IsWritten=configfile->Write(wxS("image-sound-folder"),options.defaultImgSndFolder);
IsWritten=configfile->Write(wxS("icon0"),options.icon0file);
IsWritten=configfile->Write(wxS("icon1"),options.icon1file);
IsWritten=configfile->Write(wxS("pic0"),options.pic0file);
IsWritten=configfile->Write(wxS("pic1"),options.pic1file);
IsWritten=configfile->Write(wxS("snd0"),options.snd0file);
IsWritten=configfile->Write(wxS("boot"),options.bootfile);	
configfile->Flush();
Read();
}
void configPanel::configCheck() {
    if (configoptions.compressionLevel < 0 || configoptions.compressionLevel > 9)
        configoptions.compressionLevel = 0;
    if (configoptions.useDataPSP != 0)
        configoptions.useDataPSP = 1;
    if (!wxDirExists(configoptions.defaultInputFolder))
        configoptions.defaultInputFolder = wxEmptyString;
    if (!wxDirExists(configoptions.defaultOutputFolder))
        configoptions.defaultOutputFolder = wxEmptyString;
    if (!wxDirExists(configoptions.defaultImgSndFolder))
        configoptions.defaultImgSndFolder = wxEmptyString;
}
