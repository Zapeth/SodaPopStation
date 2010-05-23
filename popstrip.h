//Source Adapted from flatwhatson original popstrip source as well as KingSquitter edited popstrip 



// The maximum possible number of ISO indexes
#define MAX_INDEXES 0x7E00
// The location of the PSAR offset in the PBP header
#define HEADER_PSAR_OFFSET 0x24
// The location of the ISO indexes in the PSAR
#define PSAR_INDEX_OFFSET 0x4000
// The location of the ISO data in the PSAR
#define PSAR_ISO_OFFSET 0x100000
// The size of one "block" of the ISO
#define ISO_BLOCK_SIZE 0x930

// Struct to store an ISO index

struct INDEX {
    int offset;
    int length;
};

struct FileInfo {
    wxString srcPBP;
    wxString dstISO;
};

void popstripExtract(wxString input_pbp, wxString output_iso);
uint popstripInit(INDEX* &iso_index, wxString input_pbp);
uint popstripReadBlock(INDEX *iso_index, int blockNo, unsigned char *out_buffer);
void popstripFinal(INDEX* &iso_index);
uint popstripGetIsoSize(INDEX* iso_index);
int ExtractIso();
