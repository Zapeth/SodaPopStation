#ifndef __CONFIG_H
#define __CONFIG_H

#include "header.h"
#include "wx/wx.h"
#include "wx/fileconf.h"
#include "wx/spinctrl.h"

class configPanel: public wxPanel
{
	public:
		configPanel(wxWindow* parent, wxWindowID id, const wxPoint&  pos, const wxSize&  size, long style);
		~configPanel();
		settings Read();
		void Write(wxCommandEvent& event);
		void configCheck();

	DECLARE_EVENT_TABLE()
	private:
		settings configoptions;
		wxFileConfig *configfile;
		wxButton *update;
		wxCheckBox *DataPSP,*CustImgPBP,*rmRegion;
		wxSpinCtrl *compressionLevel;
		wxTextCtrl *CustomIcon0,*CustomPic0,*CustomPic1,*defaultInputFolder,*defaultOutputFolder,*defaultImgSndFolder,
			*icon0file,*icon1file,*pic0file,*pic1file,*Snd0file,*Bootfile;
		
		enum{
			ID_UpdateOptions=1000,
			ID_compressionLevel,
			ID_CustomIcon0,
			ID_CustomPic0,
			ID_CustomPic1,
			ID_CustomBoot,
			ID_DataPSP,
			ID_CustImgPBP,
			ID_rmRegion,
			ID_defaultInputFolder,
			ID_defaultOutputFolder,
			ID_defaultImgSndFolder,
			ID_icon0file,
			ID_icon1file,
			ID_Pic0file,
			ID_Pic1file,
			ID_Snd0file,
			ID_Bootfile
		};
};
#endif
