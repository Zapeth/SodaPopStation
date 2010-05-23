#include "config.h"


BEGIN_EVENT_TABLE(configPanel, wxPanel)
EVT_BUTTON(ID_UpdateOptions, configPanel::Write)

END_EVENT_TABLE()

configPanel::configPanel(wxWindow* parent, wxWindowID id, const wxPoint&  pos, const wxSize&  size, long style)
	:wxPanel(parent,id,pos,size,style){
		configfile=(wxFileConfig*)wxConfigBase::Set(new wxFileConfig(wxEmptyString,wxEmptyString,"settings.ini",wxEmptyString,
				wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_RELATIVE_PATH));
		configfile->SetRecordDefaults(true);
		Read();
}
configPanel::~configPanel(){
	
}
settings configPanel::Read() {
bool IsRead;
IsRead=configfile->Read("compressionLevel",&configoptions.compressionLevel,9);
IsRead=configfile->Read("customIcon0",&configoptions.customIcon0,wxEmptyString);
IsRead=configfile->Read("customPic0",&configoptions.customPic0,wxEmptyString);
IsRead=configfile->Read("customPic1",&configoptions.customPic1,wxEmptyString);
IsRead=configfile->Read("customBoot",&configoptions.customBoot,wxEmptyString);
IsRead=configfile->Read("useDataPSP",&configoptions.useDataPSP,true);
IsRead=configfile->Read("custom-images-for-PBP",&configoptions.custImgPBP,true);
IsRead=configfile->Read("remove-region",&configoptions.rmRegion,true);
IsRead=configfile->Read("input-ISO_PBP-folder",&configoptions.defaultInputFolder,wxEmptyString);
IsRead=configfile->Read("output-ISO_PBP-folder",&configoptions.defaultOutputFolder,wxEmptyString);
IsRead=configfile->Read("image-sound-folder",&configoptions.defaultImgSndFolder,wxEmptyString);
IsRead=configfile->Read("icon0",&configoptions.icon0file,wxEmptyString);
IsRead=configfile->Read("icon1",&configoptions.icon1file,wxEmptyString);
IsRead=configfile->Read("pic0",&configoptions.pic0file,wxEmptyString);
IsRead=configfile->Read("pic1",&configoptions.pic0file,wxEmptyString);
IsRead=configfile->Read("snd0",&configoptions.snd0file,wxEmptyString);
IsRead=configfile->Read("boot",&configoptions.bootfile,wxEmptyString);
return configoptions;
}
void configPanel::Write(wxCommandEvent& event){
settings options;
bool IsWritten;
IsWritten=configfile->Write("compressionLevel",options.compressionLevel);
IsWritten=configfile->Write("customIcon0",options.customIcon0);
IsWritten=configfile->Write("customPic0",options.customPic0);
IsWritten=configfile->Write("customPic1",options.customPic1);
IsWritten=configfile->Write("customBoot",options.customBoot);
IsWritten=configfile->Write("useDataPSP",options.useDataPSP);
IsWritten=configfile->Write("custom-images-for-PBP",options.custImgPBP);
IsWritten=configfile->Write("remove-region",options.rmRegion);
IsWritten=configfile->Write("input-ISO_PBP-folder",options.defaultInputFolder);
IsWritten=configfile->Write("output-ISO_PBP-folder",options.defaultOutputFolder);
IsWritten=configfile->Write("image-sound-folder",options.defaultImgSndFolder);
IsWritten=configfile->Write("icon0",options.icon0file);
IsWritten=configfile->Write("icon1",options.icon1file);
IsWritten=configfile->Write("pic0",options.pic0file);
IsWritten=configfile->Write("pic1",options.pic1file);
IsWritten=configfile->Write("snd0",options.snd0file);
IsWritten=configfile->Write("boot",options.bootfile);	
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
