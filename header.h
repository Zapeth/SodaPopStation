#ifndef HEADER_H
#define HEADER_H
typedef struct{
    wxString saveIDcode;
    wxString saveIDnum;
    wxString saveID;
    wxString saveDesc;
    wxString region;
    wxString gameTitle;
    wxString gameIDcode;
    wxString gameIDnum;
    wxString gameID;
    int compressionLevel;
} gameData;

typedef struct{
    int compressionLevel;//Default Compression Level
    wxString customIcon0;
    wxString customPic0;
    wxString customPic1;
    wxString customBoot;
    int useDataPSP;
    int custImgPBP;
    int rmRegion;//Removes Region from title
    wxString defaultInputFolder;
    wxString defaultOutputFolder;
    wxString defaultImgSndFolder;
    wxString icon0file;
    wxString icon1file;
    wxString pic0file;
    wxString pic1file;
    wxString snd0file;
    wxString bootfile;

} settings;

typedef struct{
    wxArrayString gameIDcode;
    wxArrayString gameIDnum;
    wxArrayString saveIDcode;
    wxArrayString saveIDnum;
    wxArrayString saveFolder;//The saveID
    wxArrayString saveDesc;
    wxArrayString region;
    wxArrayString gameTitle;
    wxArrayString videoFormat;
    wxArrayString scannerID;//The gameID
} gameDataList;

#endif
