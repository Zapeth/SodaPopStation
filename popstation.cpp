#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>
#include <stdarg.h>
#include "wx/wx.h"
#include "wx/progdlg.h"
#include "wx/ffile.h"
#include "popstation.h"
#include "popstrip.h"
#include "data.h"

ConvertIsoInfo convertInfo;
wxProgressDialog* progbar;
bool cancelConvert = false;
int nextPatchPos;

char buffer[1048576];
char buffer2[0x9300];

int* ib =  (int*)buffer;

int pic0 = 0, pic1 = 0, icon0 = 0, icon1 = 0, snd = 0, toc = 0, boot = 0, prx = 0;
int sfo_size, pic0_size, pic1_size, icon0_size, icon1_size, snd_size, toc_size, boot_size, prx_size;
int start_dat = 0, min, sec, frm;

unsigned int psp_header[0x30 / 4];//Size 12
unsigned int base_header[0x28 / 4];//Size 10
unsigned int header[0x28 / 4];//Size 10
unsigned int dummy[6];

int bufferSize;
unsigned int totSize = 0;
INDEX *iso_index;
unsigned int blockCount;//Holds the number of blocks if the source is PBP

double progress_perc=0;

int convertIso();
int convertIsoMD();

int popstationErrorExit(const wxString msg);

void popstationConvert(ConvertIsoInfo info) {
    convertInfo = info;
    cancelConvert = false;
    if (convertInfo.patchCount > 0) {
        nextPatchPos = 0;
    }
    if (convertInfo.isMultiDisc==false) {
        convertIso();
    }
    else {
        convertIsoMD();
    }
}

unsigned int getsize(wxFFile &f) {
    return f.Length();
}

void patchData(char *buffer, int size, int pos) {
    while (1) {
        if (nextPatchPos >= convertInfo.patchCount) break;
        if ((pos <= convertInfo.patchData[nextPatchPos].dataPosition) && ((pos + size) >= convertInfo.patchData[nextPatchPos].dataPosition)) {
            buffer[convertInfo.patchData[nextPatchPos].dataPosition - pos] = convertInfo.patchData[nextPatchPos].newData;
            nextPatchPos++;
        } else break;
    }
}

int popstationErrorExit(const wxString msg) {

    //PostMessage(convertInfo.callback,WM_CONVERT_ERROR,0,0);
    
    wxMessageBox(msg, wxS("Popstation - Error"), wxOK | wxICON_ERROR);
    return -1;
}

int deflateCompress(z_stream *z, void *inbuf, int insize, void *outbuf, int outsize, int level) {
    int res;

    z->zalloc = Z_NULL;
    z->zfree = Z_NULL;
    z->opaque = Z_NULL;

    if (deflateInit2(z, level, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        return -1;

    z->next_out = (Bytef*) outbuf;
    z->avail_out = outsize;
    z->next_in = (Bytef*) inbuf;
    z->avail_in = insize;

    if (deflate(z, Z_FINISH) != Z_STREAM_END) {
        return -1;
    }

    res = outsize - z->avail_out;

    if (deflateEnd(z) != Z_OK)
        return -1;

    return res;
}

void SetSFOTitle(char *sfo, wxString title) {
    SFOHeader *header = (SFOHeader *) sfo;
    SFODir *entries = (SFODir *) (sfo + 0x14);
    int i;
    
    for (i = 0; i < header->nitems; i++) {
        wxString temp(sfo+header->fields_table_offs + entries[i].field_offs,wxConvISO8859_1);
        if (temp== wxS("TITLE")) {
            char *tempbuf=sfo+header->values_table_offs + entries[i].val_offs; // Might not be MD ok
            memset(tempbuf,0,entries[i].size);
            strncpy(tempbuf,title.char_str(), entries[i].size);

            if (title.length() + 1 > entries[i].size) {
                entries[i].length = entries[i].size;
            } else {
                entries[i].length = title.length() + 1;
            }
        }
    }
}

void SetSFOCode(char *sfo, wxString code) {
    SFOHeader *header = (SFOHeader *) sfo;
    SFODir *entries = (SFODir *) (sfo + 0x14);
    int i;

    for (i = 0; i < header->nitems; i++) {
        wxString temp(sfo+header->fields_table_offs + entries[i].field_offs,wxConvISO8859_1);
        if (temp==wxS("DISC_ID")) {
            char *tempbuf=sfo+header->values_table_offs + entries[i].val_offs; // Might not be MD ok
            memset(tempbuf,0,entries[i].size);
            strncpy(tempbuf,code.char_str(), entries[i].size);

            if (code.length() + 1 > entries[i].size) {
                entries[i].length = entries[i].size;
            } else {
                entries[i].length = code.length() + 1;
            }
        }
    }
}

int convertIsoMD() {
    wxFFile in, out, base, t;
    int offset;
    unsigned int isosize, isorealsize,x;
    char y;
    int index_offset, p1_offset, p2_offset, m_offset, end_offset;
    IsoIndex *indexes;
    int iso_positions[5];
    int ciso;
    z_stream z;

    int ndiscs;
    wxArrayString inputs;
    wxString output;
    wxString title;
    wxArrayString titles;
    wxString code;
    wxArrayString codes;
    int complevels[5];
    wxString BASE;
    wxString errorstring;
    ndiscs = convertInfo.multiDiscInfo.fileCount;

    inputs.Add(convertInfo.multiDiscInfo.srcISO[0]);
    inputs.Add(convertInfo.multiDiscInfo.srcISO[1]);
    inputs.Add(convertInfo.multiDiscInfo.srcISO[2]);
    inputs.Add(convertInfo.multiDiscInfo.srcISO[3]);
    inputs.Add(convertInfo.multiDiscInfo.srcISO[4]);

    titles.Add(convertInfo.multiDiscInfo.gameTitle[0]);
    titles.Add(convertInfo.multiDiscInfo.gameTitle[1]);
    titles.Add(convertInfo.multiDiscInfo.gameTitle[2]);
    titles.Add(convertInfo.multiDiscInfo.gameTitle[3]);
    titles.Add(convertInfo.multiDiscInfo.gameTitle[4]);

    codes.Add(convertInfo.multiDiscInfo.gameID[0]);
    codes.Add(convertInfo.multiDiscInfo.gameID[1]);
    codes.Add(convertInfo.multiDiscInfo.gameID[2]);
    codes.Add(convertInfo.multiDiscInfo.gameID[3]);
    codes.Add(convertInfo.multiDiscInfo.gameID[4]);

    complevels[0] = convertInfo.multiDiscInfo.compLevel[0];
    complevels[1] = convertInfo.multiDiscInfo.compLevel[1];
    complevels[2] = convertInfo.multiDiscInfo.compLevel[2];
    complevels[3] = convertInfo.multiDiscInfo.compLevel[3];
    complevels[4] = convertInfo.multiDiscInfo.compLevel[4];

    output = convertInfo.dstPBP;
    title = convertInfo.saveTitle;
    code = convertInfo.saveID;
    BASE = convertInfo.base;
    wxProgressDialog progbar(wxS("Converting"), wxS("Starting"),100,NULL,wxPD_APP_MODAL|wxPD_CAN_ABORT);
    
    base.Open(BASE, wxS("rb"));
    if (base.IsOpened()==false) {
    	
    	errorstring=wxS("Cannot open ")+BASE+wxS("\n");
        return 0;//popstationErrorExit(errorstring);
    }

    out.Open(output, wxS("wb"));
    if (out.IsOpened()==false) {
    	
    	errorstring=wxS("Cannot create ")+output+wxS("\n");
        return 0;//popstationErrorExit(errorstring);
    }

    printf("Writing PBP header...\n");

    base.Read(base_header, 0x28);

    if (base_header[0] != 0x50425000) {
    	
    	errorstring=BASE+wxS(" is not a PBP file.\n");
        return popstationErrorExit(errorstring);
    }

    sfo_size = base_header[3] - base_header[2];

    wxString temp=convertInfo.icon0;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        icon0_size = getsize(t);
        icon0 = 1;
        t.Close();
    } else {
        icon0_size = base_header[4] - base_header[3];
    }
    }
    
    temp=convertInfo.icon1;
    if(wxFileExists(temp)){
    t.Open(convertInfo.icon1, wxS("rb"));
    if (t.IsOpened()) {
        icon1_size = getsize(t);
        icon1 = 1;
        t.Close();
    } else {
        icon1_size = 0;
    }
    }
	
    temp=convertInfo.pic0;
    if(wxFileExists(temp)){
    t.Open(convertInfo.pic0, wxS("rb"));
    if (t.IsOpened()==true) {
        pic0_size = getsize(t);
        pic0 = 1;
        t.Close();
    } else {
        pic0_size = 0; //base_header[6] - base_header[5];
    }
    }
    
    temp=convertInfo.pic1;
    if(wxFileExists(temp)){
    t.Open(convertInfo.pic1, wxS("rb"));
    if (t.IsOpened()==true) {
        pic1_size = getsize(t);
        pic1 = 1;
        t.Close();
    } else {
        pic1_size = 0; // base_header[7] - base_header[6];
    }
    }
    
    temp=convertInfo.snd0;
    if(wxFileExists(temp)){
    t.Open(convertInfo.snd0, wxS("rb"));
    if (t.IsOpened()==true) {
        snd_size = getsize(t);
        snd = 1;
        t.Close();
    } else {
        snd = 0;
    }
    }

    temp=convertInfo.boot;
    if(wxFileExists(temp)){
    t.Open(convertInfo.boot, wxS("rb"));
    if (t.IsOpened()==true) {
        boot_size = getsize(t);
        boot = 1;
        t.Close();
    } else {
        boot = 0;
    }
    }

    temp=convertInfo.data_psp;
    if(wxFileExists(temp)){
    t.Open(convertInfo.data_psp, wxS("rb"));
    if (t.IsOpened()==true) {
        prx_size = getsize(t);
        prx = 1;
        t.Close();;
    } else {
        base.Seek(base_header[8]);
        base.Read(psp_header, 0x30);

        prx_size = psp_header[0x2C / 4];
    }
    }

    int curoffs = 0x28;

    header[0] = 0x50425000;
    header[1] = 0x10000;

    header[2] = curoffs;

    curoffs += sfo_size;
    header[3] = curoffs;

    curoffs += icon0_size;
    header[4] = curoffs;

    curoffs += icon1_size;
    header[5] = curoffs;

    curoffs += pic0_size;
    header[6] = curoffs;

    curoffs += pic1_size;
    header[7] = curoffs;

    curoffs += snd_size;
    header[8] = curoffs;

    x = header[8] + prx_size;

    if ((x % 0x10000) != 0) {
        x = x + (0x10000 - (x % 0x10000));
    }

    header[9] = x;

    out.Write(header, 0x28);

    printf("Writing sfo...\n");

    base.Seek(base_header[2],wxFromStart);
    base.Read(buffer, sfo_size);
    SetSFOTitle(buffer, title);
    strcpy(buffer + 0x108, (char*)code.char_str());
    out.Write(buffer, sfo_size);

    printf("Writing icon0.png...\n");

    if (!icon0) {
        base.Seek(base_header[3]);
        base.Read(buffer, icon0_size);
        out.Write(buffer, icon0_size);
    } else {
        t.Open(convertInfo.icon0, wxS("rb"));
        t.Read(buffer, icon0_size);
        out.Write(buffer, icon0_size);
        t.Close();
    }

    if (icon1) {
        printf("Writing icon1.pmf...\n");
        t.Open(convertInfo.icon1, wxS("rb"));
        t.Read(buffer, icon1_size);
        out.Write(buffer, icon1_size);
        t.Close();
    }

    if (!pic0) {
        //fseek(base, base_header[5], SEEK_SET);
        //fread(buffer, 1, pic0_size, base);
        //fwrite(buffer, 1, pic0_size, out);
    } else {
        printf("Writing pic0.png...\n");
        t.Open(convertInfo.pic0, wxS("rb"));
        t.Read(buffer, pic0_size);
        out.Write(buffer, pic0_size);
        t.Close();
       
    }
    if (!pic1) {
        //fseek(base, base_header[6], SEEK_SET);
        //fread(buffer, 1, pic1_size, base);
        //fwrite(buffer, 1, pic1_size, out);
    } else {
        printf("Writing pic1.png...\n");
        t.Open(convertInfo.pic1, wxS("rb"));
        t.Read(buffer, pic1_size);
        out.Write(buffer, pic1_size);
        t.Close();
        
    }

    if (snd) {
        printf("Writing snd0.at3...\n");
        t.Open(convertInfo.snd0, wxS("rb"));
        t.Read(buffer, snd_size);
        out.Write(buffer, snd_size);
        t.Close();
    }

    printf("Writing DATA.PSP...\n");

    if (prx) {
        t.Open(convertInfo.data_psp, wxS("rb"));
        t.Read(buffer, prx_size);
        out.Write(buffer, prx_size);
        t.Close();
       
    } else {
        base.Seek(base_header[8]);
        base.Read(buffer, prx_size);
        out.Write(buffer, prx_size);

    }

    offset = out.Tell();

    for (unsigned int i = 0; i < header[9] - offset; i++) {
        out.Write("\0",1);
    }

    printf("Writing PSTITLE header...\n");

    out.Write("PSTITLEIMG000000", 16);

    // Save this offset position
    p1_offset = out.Tell();;

    WriteInteger(0, 2);
    WriteInteger(0x2CC9C5BC, 1);
    WriteInteger(0x33B5A90F, 1);
    WriteInteger(0x06F6B4B3, 1);
    WriteInteger(0xB25945BA, 1);
    WriteInteger(0, 0x76);

    m_offset = out.Tell();

    memset(iso_positions, 0, sizeof (iso_positions));
    out.Write(iso_positions, sizeof (iso_positions));

    WriteRandom(12);
    WriteInteger(0, 8);

    out.Write("_",1);
    out.Write((char*)code.char_str(),4);
    out.Write("_",1);
    out.Write((char*)code.char_str() + 4,5);

    WriteChar(0, 0x15);

    p2_offset = out.Tell();
    WriteInteger(0, 2);

    out.Write(data3,sizeof(data3));
    out.Write((char*)title.char_str(),title.length());

    WriteChar(0, 0x80 - title.length());
    WriteInteger(7, 1);
    WriteInteger(0, 0x1C);

    //Get size of all isos
   
    totSize = 0;
    for (ciso = 0; ciso < ndiscs; ciso++) {
        in.Open(inputs[ciso], wxS("rb"));
        if (in.IsOpened()==false) {
            
            errorstring=wxS("Cannot open ")+inputs[ciso]+wxS("\n");
            return 0;//popstationErrorExit(errorstring);
        }

        isosize = getsize(in);
        totSize += isosize;
        in.Close();
    }
    //PostMessage(convertInfo.callback,WM_CONVERT_SIZE,0,totSize);//TODO

    unsigned int curSize=0;
    for (ciso = 0; ciso < ndiscs; ciso++) {
        in.Open(inputs[ciso], wxS("rb"));
        if (in.IsOpened()==false) {
            
            errorstring=wxS("Cannot open ")+inputs[ciso]+wxS("\n");
            return 0;//popstationErrorExit(errorstring);
        }

        isosize = getsize(in);
        isorealsize = isosize;

        if ((isosize % 0x9300) != 0) {
            isosize = isosize + (0x9300 - (isosize % 0x9300));
        }

        offset = out.Tell();

        if (offset % 0x8000) {
            x = 0x8000 - (offset % 0x8000);

            WriteChar(0, x);

        }

        iso_positions[ciso] = out.Tell() - header[9];

        printf("Writing header (iso #%d)\n", ciso + 1);

        out.Write("PSISOIMG0000", 12);

        WriteInteger(0, 0xFD);

        memcpy(data1 + 1, (char*)codes[ciso].char_str(), 4);
        memcpy(data1 + 6, (char*)codes[ciso].char_str() + 4, 5);
        out.Write(data1, sizeof (data1));

        WriteInteger(0, 1);

        strcpy((char*) (data2 + 8), (char*)titles[ciso].char_str());
        out.Write(data2, sizeof (data2));

        index_offset = out.Tell();

        printf("Writing indexes (iso #%d)...\n", ciso + 1);

        memset(dummy, 0, sizeof (dummy));

        offset = 0;

        if (complevels[ciso] == 0) {
            x = 0x9300;
        } else {
            x = 0;
        }

        for (unsigned int i = 0; i < isosize / 0x9300; i++) {
            out.Write(&offset, 4);
            out.Write(&x, 4);
            out.Write(dummy, sizeof (dummy));

            if (complevels[ciso] == 0)
                offset += 0x9300;
        }

        offset = out.Tell();

        for (unsigned int i = 0; i < (iso_positions[ciso] + header[9] + 0x100000) - offset; i++) {
            out.Write("\0",1);
        }

        wxPrintf(wxS("Writing iso #%d (%s)...\n"), ciso + 1, inputs[ciso].c_str());

        if (complevels[ciso] == 0) {
            while ((x = in.Read(buffer, 1048576)) > 0) {
                out.Write(buffer, x);
                curSize += x;
                
                progress_perc=((double)(curSize)/totSize)*100;
                progbar.Update(progress_perc*((ciso+1)/ndiscs),wxString::Format(wxS("Writing iso #%d. %.0f%% done converting:  "),ciso, progress_perc));
                if (cancelConvert) {
                    
                    in.Close();
                    out.Close();
                    base.Close();

                    return 0;
                }
            }

            for (unsigned int i = 0; i < (isosize - isorealsize); i++) {
                out.Write("\0",1);
            }
        } else {
            indexes = (IsoIndex *) malloc(sizeof (IsoIndex) * (isosize / 0x9300));

            if (!indexes) {
                
                in.Close();
                out.Close();
                base.Close();
		return popstationErrorExit(wxString::Format(wxS("Cannot alloc memory for indexes!\n")));
            }

            unsigned int i = 0;
            offset = 0;

            while ((x = in.Read(buffer2, 0x9300)) > 0) {
                curSize += x;

                if (x < 0x9300) {
                    memset(buffer2 + x, 0, 0x9300 - x);
                }

                x = deflateCompress(&z, buffer2, 0x9300, buffer, sizeof (buffer), complevels[ciso]);

                if (x < 0) {
                    
                    in.Close();
                    out.Close();
                    base.Close();
                    free(indexes);
                    return popstationErrorExit(wxString::Format(wxS("Error in compression!\n")));
                }

                memset(&indexes[i], 0, sizeof (IsoIndex));

                indexes[i].offset = offset;

                if (x >= 0x9300) /* Block didn't compress */ {
                    indexes[i].length = 0x9300;
                    out.Write(buffer2, 0x9300);
                    offset += 0x9300;
                } else {
                    indexes[i].length = x;
                    out.Write(buffer, x);
                    offset += x;
                }

                progress_perc=((double)(curSize)/totSize)*100;
                progbar.Update(progress_perc*((ciso+1)/ndiscs),wxString::Format(wxS("Writing iso #%d. %.0f%% done converting: "),ciso, progress_perc));
                if (cancelConvert) {
                    
                    in.Close();
                    out.Close();
                    base.Close();
                    return 0;
                }

                i++;
            }

            if (i != (isosize / 0x9300)) {
                
                in.Close();
                out.Close();
                base.Close();
                free(indexes);
                return popstationErrorExit(wxString::Format(wxS("Some error happened.\n")));
            }
        }

        if (complevels[ciso] != 0) {
            offset = out.Tell();

            printf("Updating compressed indexes (iso #%d)...\n", ciso + 1);
            
            out.Seek(index_offset,wxFromStart);
            out.Write(indexes, sizeof (IsoIndex) * (isosize / 0x9300));

            out.Seek(offset,wxFromStart);
            
        }
        in.Close();
    }

    x = out.Tell();

    if ((x % 0x10) != 0) {
        end_offset = x + (0x10 - (x % 0x10));

        for (unsigned int i = 0; i < (end_offset - x); i++) {
            out.Write("0",1);
        }
    } else {
        end_offset = x;
    }

    end_offset -= header[9];

    printf("Writing special data...\n");
    progbar.Update(99,wxString::Format(wxS("Writing special data...")));
    base.Seek(base_header[9] + 12,wxFromStart);
    base.Read(&x, 4);

    x += 0x50000;

    base.Seek(x,wxFromStart);
    base.Read(buffer, 8);

    if (memcmp(buffer, "STARTDAT", 8) != 0) {
    	errorstring=wxS("Cannot find STARTDAT in ")+BASE+wxS(".\n")+wxS("Not a valid PSX eboot.pbp\n");
        
        return popstationErrorExit(errorstring);
    }

    base.Seek(x + 16,wxFromStart);
    base.Read(header, 8);
    base.Seek(x,wxFromStart);
    base.Read(buffer, header[0]);

    if (!boot) {
        out.Write(buffer, header[0]);
        base.Read(buffer, header[1]);
        out.Write(buffer, header[1]);
    } else {
        printf("Writing boot.png...\n");
        progbar.Update(99,wxString::Format(wxS("Writing boot.png...")));
        ib[5] = boot_size;
        out.Write(buffer, header[0]);
        t.Open(convertInfo.boot, wxS("rb"));
        t.Read(buffer, boot_size);
        out.Write(buffer, boot_size);
        t.Close();
        base.Read(buffer, header[1]);
    }

    //fseek(base, x, SEEK_SET);

    while ((x = base.Read(buffer, 1048576) )> 0) {
        out.Write(buffer, x);
    }
    out.Seek(p1_offset,wxFromStart);
    out.Write(&end_offset,4);
    
    end_offset += 0x2d31;
    out.Seek(p2_offset,wxFromStart);
    out.Write(&end_offset,4);

    out.Seek(m_offset,wxFromStart);
    out.Write(iso_positions, sizeof (iso_positions));

    out.Close();
    base.Close();

    progbar.Update(100,wxS("Conversion Complete"));
    progbar.EndModal(wxID_OK);
    printf("Done\n");
    return 0;
}

int convertIso() {
    wxFFile in, out, base, t;
    unsigned int offset, isosize, isorealsize;
    int x;
    int index_offset, p1_offset, p2_offset, end_offset;
    IsoIndex *indexes;
    int curoffs = 0x28;
    
    
    int complevel;
    
    wxString input;
    wxString output;
    wxString BASE;
    z_stream z;
    wxString errorstring;
    BASE = convertInfo.base;
    input = convertInfo.srcISO;
    output = convertInfo.dstPBP;
    
    complevel = convertInfo.compLevel;
    wxProgressDialog progbar(wxS("Converting"), wxS("Starting"),100,NULL,wxPD_APP_MODAL|wxPD_CAN_ABORT);

    in.Open(input, wxS("rb"));
    if (in.IsOpened()==false) {
        if (input[0] == 0){
            
            return popstationErrorExit(wxString::Format(wxS("No input file selected.")));
        }
        else{
            errorstring=wxS("Cannot open ")+input+wxS("\n");
            
            return 0;//popstationErrorExit(errorstring);
        }
    }

    //Check if input is pbp
    if (convertInfo.srcIsPbp) {
        blockCount = popstripInit(iso_index, convertInfo.srcISO);
        if (blockCount <= 0) {
            
            return popstationErrorExit(wxString::Format(wxS("No iso index was found.")));
        }
        isosize = popstripGetIsoSize(iso_index);
    } else {
        isosize = getsize(in);
    }
    isorealsize = isosize;
    
    
    cancelConvert=!progbar.Update(0,wxString::Format(wxS("ISO Size %i"),isosize));
    if ((isosize % 0x9300) != 0) {
        isosize = isosize + (0x9300 - (isosize % 0x9300));
    }

    //printf("isosize, isorealsize %08X  %08X\n", isosize, isorealsize);

    base.Open(BASE, wxS("rb"));
    if (base.IsOpened()==false) {
    	//errorstring=wxS("Cannot open ")+BASE+wxS(".\n");
        
        return 0;//popstationErrorExit(errorstring);
    }

    out.Open(output, wxS("wb"));
    if (out.IsOpened()==false) {
        if (output[0] == 0){
            
            return popstationErrorExit(wxString::Format(wxS("No output file selected.")));
        }
        else{
            errorstring=wxS("Cannot open ")+output+wxS(".\n");
            
            return 0;//popstationErrorExit(errorstring);
        }
    }

    printf("Writing header...\n");

    base.Read(base_header, 0x28);

    if (base_header[0] != 0x50425000) {
    	errorstring=BASE+wxS(" is not a PBP file.\n");
        
        return popstationErrorExit(errorstring);
    }

    sfo_size = base_header[3] - base_header[2];

    wxString temp=convertInfo.icon0;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        icon0_size = getsize(t);
        icon0 = 1;
        t.Close();
    } else {
        icon0_size = base_header[4] - base_header[3];
    }
    }
    
    temp=convertInfo.icon1;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        icon1_size = getsize(t);
        icon1 = 1;
        t.Close();
    } else {
        icon1_size = 0;
    }
    }
	
    temp=convertInfo.pic0;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        pic0_size = getsize(t);
        pic0 = 1;
        t.Close();
    } else {
        pic0_size = 0; //base_header[6] - base_header[5];
    }
    }
    
    temp=convertInfo.pic1;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        pic1_size = getsize(t);
        pic1 = 1;
        t.Close();
    } else {
        pic1_size = 0; // base_header[7] - base_header[6];
    }
    }
    
    temp=convertInfo.snd0;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        snd_size = getsize(t);
        snd = 1;
        t.Close();
    } else {
        snd = 0;
    }
    }

    temp=convertInfo.boot;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        boot_size = getsize(t);
        boot = 1;
        t.Close();
    } else {
        boot = 0;
    }
    }

    temp=convertInfo.data_psp;
    if(wxFileExists(temp)){
    t.Open(temp, wxS("rb"));
    if (t.IsOpened()==true) {
        prx_size = getsize(t);
        prx = 1;
        t.Close();
    } else {
        base.Seek(base_header[8],wxFromStart);
        base.Read(psp_header, 0x30);

        prx_size = psp_header[0x2C / 4];
    }
    }

    header[0] = 0x50425000;
    header[1] = 0x10000;

    header[2] = curoffs;

    curoffs += sfo_size;
    header[3] = curoffs;

    curoffs += icon0_size;
    header[4] = curoffs;

    curoffs += icon1_size;
    header[5] = curoffs;

    curoffs += pic0_size;
    header[6] = curoffs;

    curoffs += pic1_size;
    header[7] = curoffs;

    curoffs += snd_size;
    header[8] = curoffs;

    x = header[8] + prx_size;

    if ((x % 0x10000) != 0) {
        x = x + (0x10000 - (x % 0x10000));
    }

    header[9] = x;

    out.Write(header,0x28);

    printf("Writing sfo...\n");

    base.Seek(base_header[2],wxFromStart);
    base.Read(buffer, sfo_size);
    SetSFOTitle(buffer, convertInfo.saveTitle);
    SetSFOCode(buffer, convertInfo.saveID);
    //strcpy(buffer+0x108, code);
    out.Write(buffer, sfo_size);

    

    if (icon0) {
    	printf("Writing icon0.png...\n");
        t.Open(convertInfo.icon0, wxS("rb"));
        t.Read(buffer, icon0_size);
        out.Write(buffer, icon0_size);
        t.Close();
    }

    if (icon1) {
        printf("Writing icon1.pmf...\n");

        t.Open(convertInfo.icon1, wxS("rb"));
        t.Read(buffer, icon1_size);
        out.Write(buffer, icon1_size);
        t.Close();
    }

    if (pic0) {
        printf("Writing pic0.png...\n");

        t.Open(convertInfo.pic0, wxS("rb"));
        t.Read(buffer, pic0_size);
        out.Write(buffer, pic0_size);
        t.Close();
    }

    if (pic1) {
        printf("Writing pic1.png...\n");

        t.Open(convertInfo.pic1, wxS("rb"));
        t.Read(buffer, pic1_size);
        out.Write(buffer, pic1_size);
        t.Close();
    }

    if (snd) {
        printf("Writing snd0.at3...\n");

        t.Open(convertInfo.snd0, wxS("rb"));
        t.Read(buffer, snd_size);
        out.Write(buffer, snd_size);
        t.Close();
    }

    printf("Writing DATA.PSP...\n");
    if (prx) {
        t.Open(convertInfo.data_psp, wxS("rb"));
        t.Read(buffer, prx_size);
        out.Write(buffer, prx_size);
        t.Close();
    } else {
        base.Seek(base_header[8]);
        base.Read(buffer, prx_size);
        out.Write(buffer, prx_size);
    }

    offset = out.Tell();

    for (unsigned int i = 0; i < header[9] - offset; i++) {
        out.Write("\0",1);
    }

    printf("Writing iso header...\n");

    out.Write("PSISOIMG0000", 12);

    p1_offset = out.Tell();

    x = isosize + 0x100000;
    out.Write(&x, 4);

    x = 0;
    for (unsigned int i = 0; i < 0xFC; i++) {
        out.Write(&x,4);
    }

    memcpy(data1 + 1, convertInfo.gameID.char_str(), 4);
    memcpy(data1 + 6, (char*)convertInfo.gameID.char_str() + 4, 5);
    /*
            offset = isorealsize/2352+150;
            min = offset/75/60;
            sec = (offset-min*60*75)/75;
            frm = offset-(min*60+sec)*75;
            data1[0x41b] = bcd(min);
            data1[0x41c] = bcd(sec);
            data1[0x41d] = bcd(frm);
     */
    if (convertInfo.tocSize > 0) {
        printf("Copying toc to iso header...\n");
        memcpy(data1 + 1024, convertInfo.tocData, convertInfo.tocSize);
    }
    out.Write(data1, sizeof (data1));

    p2_offset =out.Tell();
    x = isosize + 0x100000 + 0x2d31;
    out.Write(&x, 4);

    strcpy((char*) (data2 + 8), convertInfo.gameTitle.char_str());
    out.Write(data2, sizeof (data2));

    index_offset = out.Tell();

    printf("Writing indexes...\n");

    memset(dummy, 0, sizeof (dummy));

    offset = 0;

    if (complevel == 0) {
        x = 0x9300;
    } else {
        x = 0;
    }

    for (unsigned int i = 0; i < isosize / 0x9300; i++) {
        out.Write(&offset, 4);
        out.Write(&x, 4);
        out.Write(dummy, sizeof (dummy));

        if (complevel == 0)
            offset += 0x9300;
    }

    offset = out.Tell();

    for (unsigned int i = 0; i < (header[9] + 0x100000) - offset; i++) {
        out.Write("\0",1);
    }

    printf("Writing iso...\n");

    totSize = 0;
    unsigned int curSize=0;
    if (complevel == 0) {
        unsigned int i = 0;
        if (convertInfo.srcIsPbp) {
            for (i = 0; i < blockCount; i++) {
                bufferSize = popstripReadBlock(iso_index, i, (unsigned char*) buffer2);

                if (convertInfo.patchCount > 0) patchData(buffer2, bufferSize, totSize);
                totSize += bufferSize;
                if (totSize > isorealsize) {
                    bufferSize = bufferSize - (totSize - isorealsize);
                    totSize = isorealsize;
                }

                out.Write(buffer2, bufferSize);
                progress_perc=((double)(i)/blockCount)*100;
                
                cancelConvert=!progbar.Update((int)(progress_perc*99/100),wxString::Format(wxS("%.0f%% done converting: "),progress_perc));
                if (cancelConvert) {
                    
                    in.Close();
                    out.Close();
                    base.Close();
                    if (convertInfo.srcIsPbp) popstripFinal(iso_index);
                    
                    return 0;
                }
            }
        } else {
            while ((x = in.Read(buffer, 1048576)) > 0) {
                if (convertInfo.patchCount > 0) patchData(buffer, x, curSize);
                out.Write(buffer, x);

                curSize += x;

                progress_perc=((double)(curSize)/isosize)*100;
                cancelConvert=!progbar.Update((int)(progress_perc*99/100),wxString::Format(wxS("%.0f%% done converting: "),progress_perc));
                
                if (cancelConvert) {
                    
                    in.Close();
                    out.Close();
                    base.Close();
                    
                    return 0;
                }
            }
        }

        for (unsigned int i = 0; i < (isosize - isorealsize); i++) {
            out.Write("\0",1);
        }
    } else {
        indexes = (IsoIndex *) malloc(sizeof (IsoIndex) * (isosize / 0x9300));

        if (!indexes) {
            
            in.Close();
            out.Close();
            base.Close();
            if (convertInfo.srcIsPbp) popstripFinal(iso_index);
            
            return popstationErrorExit(wxString::Format(wxS("Cannot alloc memory for indexes!\n")));
        }

        unsigned int i = 0;
        //j = 0;
        offset = 0;

        while (true) {

            if (convertInfo.srcIsPbp) {
                if(i >= blockCount)
                    break;
                bufferSize = popstripReadBlock(iso_index, i, (unsigned char*) buffer2);

                totSize += bufferSize;
                if (totSize > isorealsize) {
                    bufferSize = bufferSize - (totSize - isorealsize);
                    totSize = isorealsize;
                }
                x = bufferSize;
            } else {
                x = in.Read(buffer2, 0x9300);
            }
            if (x == 0) 
                break;
            if (convertInfo.patchCount > 0) patchData(buffer2, x, curSize);

            curSize += x;
            
            //This time the input file is used to figure out the progress based on how many bytes read
            progress_perc=((double)(curSize)/isosize)*100;
            cancelConvert=!progbar.Update((int)(progress_perc*99/100),wxString::Format(wxS("%.0f%% done converting: "),progress_perc));
            
            if (cancelConvert) {

            	in.Close();
                out.Close();
                base.Close();
                free(indexes);
                if (convertInfo.srcIsPbp) popstripFinal(iso_index);
                
                return 0;
            }

            if (x < 0x9300) {
                memset(buffer2 + x, 0, 0x9300 - x);
            }

            x = deflateCompress(&z, buffer2, 0x9300, buffer, sizeof (buffer), complevel);

            if (x < 0) {
                
                in.Close();
                out.Close();
                base.Close();
                free(indexes);
                if (convertInfo.srcIsPbp) popstripFinal(iso_index);
                
                return popstationErrorExit(wxString::Format(wxS("Error in compression!\n")));
            }

            memset(&indexes[i], 0, sizeof (IsoIndex));

            indexes[i].offset = offset;

            if (x >= 0x9300) /* Block didn't compress */ {
                indexes[i].length = 0x9300;
                out.Write(buffer2, 0x9300);
                offset += 0x9300;
            } else {
                indexes[i].length = x;
                out.Write(buffer, x);
                offset += x;
            }
            
            i++;
        }

        if (i != (isosize / 0x9300)) {
            
            in.Close();
            out.Close();
            base.Close();
            free(indexes);
            
            return popstationErrorExit(wxString::Format(wxS("Some error happened.\n")));
        }

        x = out.Tell();

        if ((x % 0x10) != 0) {
            end_offset = x + (0x10 - (x % 0x10));

            for (int i = 0; i < (end_offset - x); i++) {
                out.Write("0",1);
            }
        } else {
            end_offset = x;
        }

        end_offset -= header[9];
    }

    printf("Writing special data...\n");
    progbar.Update(99,wxString::Format(wxS("Writing special data...")));
    base.Seek(base_header[9] + 12,wxFromStart);
    base.Read(&x, 4);

    x += 0x50000;

    base.Seek(x,wxFromStart);
    base.Read(buffer, 8);

    if (memcmp(buffer, "STARTDAT", 8) != 0) {
    	errorstring=wxS("Cannot find STARTDAT in ")+BASE+wxS("\nNot a valid PSX eboot.pbp\n");
        
        return popstationErrorExit(errorstring);
    }

    base.Seek(x + 16,wxFromStart);
    base.Read(header, 8);
    base.Seek(x,wxFromStart);
    base.Read(buffer, header[0]);

    if (!boot) {
        out.Write(buffer, header[0]);
        base.Read(buffer, header[1]);
        out.Write(buffer, header[1]);
    } else {
        printf("Writing boot.png...\n");
        progbar.Update(99,wxString::Format(wxS("Writing boot.png...")));
        ib[5] = boot_size;
        out.Write(buffer, header[0]);
        t.Open(convertInfo.boot, wxS("rb"));
        t.Read(buffer, boot_size);
        out.Write(buffer, boot_size);
        t.Close();
        base.Read(buffer, header[1]);
    }

    while ((x = base.Read(buffer, 1048576)) > 0) {
        out.Write(buffer, x);
    }

    if (complevel != 0) {
        printf("Updating compressed indexes...\n");
        progbar.Update(99,wxString::Format(wxS("Updating compressed indexes...")));

        out.Seek(p1_offset,wxFromStart);
        out.Write(&end_offset, 4);

        end_offset += 0x2d31;
        out.Seek(p2_offset,wxFromStart);
        out.Write(&end_offset, 4);

        out.Seek(index_offset,wxFromStart);
        out.Write(indexes, sizeof (IsoIndex) * (isosize / 0x9300));
    }

    in.Close();
    out.Close();
    base.Close();
    if (convertInfo.srcIsPbp) popstripFinal(iso_index);

    
    progbar.Update(100,wxS("Conversion Complete"));
    
    printf("Done\n");
    return 0;
}
