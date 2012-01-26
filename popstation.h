inline void WriteInteger(wxFFile& out, unsigned int num, int count)
{
	
	for ( int i = 0; i < count; i++) 
		out.Write(&num,4);
}

inline void WriteChar(wxFFile& out, unsigned char num, int count)
{
	for ( int i = 0; i < count; i++)
		out.Write(&num,1);
}

inline void WriteRandom(wxFFile& out, int count)
{
	for (int i = 0; i < count; i++)
	{ 
		unsigned int num = rand(); 
		out.Write(&num,4); 
	}
}


struct  SFOHeader
{
	unsigned int signature;
	unsigned int version;
	unsigned int fields_table_offs;
	unsigned int values_table_offs;
	int nitems;
};

struct SFODir
{
	unsigned short field_offs;
	unsigned char  unk;
	unsigned char  type; // 0x2 -> string, 0x4 -> number
	unsigned int length;
	unsigned int size;
	unsigned short val_offs;
	unsigned short unk4;
};

struct IsoIndex
{
	unsigned int offset;
	unsigned int length;
	unsigned int dummy[6];
};

struct PatchData{
	char newData;
	int dataPosition;
} ;

struct MultiDiscInfo{
	int fileCount;
	wxArrayString srcISO;
	wxArrayString gameTitle;
	wxArrayString gameID;
	int compLevel[5];
	
} ;

struct ConvertIsoInfo{
	wxString base;
	wxString data_psp;
	wxString srcISO;
  wxString dstPBP;
	wxString pic0;
	wxString pic1;
	wxString icon0;
	wxString icon1;
	wxString snd0;
	wxString boot;
	bool srcIsPbp;
  bool isMultiDisc;
	wxString gameTitle;
	wxString saveTitle;
	wxString gameID;
	wxString saveID;
	int compLevel;

	int tocSize;
	void* tocData;

	MultiDiscInfo multiDiscInfo;

	int patchCount;
	PatchData *patchData;
} ;

int convertIso();
int convertIsoMD();
void popstationConvert(ConvertIsoInfo info);
