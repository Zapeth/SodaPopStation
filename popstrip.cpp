#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include "wx/ffile.h"
#include "wx/msgdlg.h"
#include "popstrip.h"

FileInfo extractInfo;

void popstripExtract(wxString input_pbp, wxString output_iso) {
    extractInfo.srcPBP=input_pbp;
    extractInfo.dstISO=output_iso;
    if(ExtractIso()==0)
    	wxMessageBox(wxS("ISO Extracted Successfully"));
    else{
    	/* Commented out because a destiniation may be a file that the user did not input into output box
    	wxRemoveFile(extractInfo.dstISO);
    	*/
    	wxMessageBox(wxS("Error: ISO was not extracted successfully"), wxS("ERROR"), wxICON_ERROR);
    	}
    
}

int ExtractIso() {
    INDEX *iso_index;
    wxFFile iso_stream;
    unsigned char buffer[16 * ISO_BLOCK_SIZE];
    int blockCount = 0;
    int isoSize = 0;
    int bufferSize;

    int totSize = 0;


    // Open the output ISO file
    iso_stream.Open(extractInfo.dstISO, wxS("wb"));
    if (iso_stream.IsOpened()==false) {
        wxPrintf(wxS("ERROR: Could not Create %s for writing"), extractInfo.dstISO.c_str());
        return -1;
    }



    // Store the number of indexes we've read in
    blockCount = popstripInit(iso_index, extractInfo.srcPBP);
    if (blockCount == 0) {
        printf("No iso index was found."); //TODO
        iso_stream.Close();
        return -1;
    }
    isoSize = popstripGetIsoSize(iso_index);
    //PostMessage(extractInfo.callback,WM_EXTRACT_SIZE,0,isoSize);//TODO
    // For each of the indexes
    for (int i = 0; i < blockCount; ++i) {
        // The ISO size is contained in the data referenced in index #2

        // Record the total bytes that we've written (including what's in the output buffer)
        bufferSize = popstripReadBlock(iso_index, i, buffer);
        totSize += bufferSize;
        // If the total bytes is larger than the ISO size, reduce the output buffer to the correct length
        // This makes sure that the output ISO is the same size as the original, removing the padding added by popstation
        if (totSize > isoSize) {
            bufferSize = bufferSize - (totSize - isoSize);
            totSize = isoSize;
        }

        // Write the output buffer to the extracted ISO file
        iso_stream.Write(buffer,bufferSize);
        //PostMessage(extractInfo.callback,WM_EXTRACT_PROGRESS,0,totSize);
        //if (cancelExtract) break;
    }

    // Close the output file
    iso_stream.Close();
    popstripFinal(iso_index);
    //PostMessage(extractInfo.callback,WM_EXTRACT_DONE,0,0);
    return 0;
}

unsigned int popstripReadBlock(INDEX *iso_index, int blockNo, unsigned char *out_buffer) {
    wxFFile pbp_stream;
    unsigned char *in_buffer;
    z_stream z;
    int psar_offset;
    int this_offset;
    int out_length;

    pbp_stream.Open(extractInfo.srcPBP, wxS("rb"));
    if (pbp_stream.IsOpened()==false) {
        wxPrintf(wxS("Unable to open \"%s\"."), extractInfo.srcPBP.c_str());
        return 0;
    }
    // Read in the offset of the PSAR file
    pbp_stream.Seek(HEADER_PSAR_OFFSET);
    pbp_stream.Read(&psar_offset,4);

    // Go to the offset specified in the index
    this_offset = psar_offset + PSAR_ISO_OFFSET + iso_index[blockNo].offset;
    pbp_stream.Seek(this_offset);
    
    // Check if this block isn't compressed
    if (iso_index[blockNo].length == 16 * ISO_BLOCK_SIZE) {

        // It's not compressed, make an exact copy
        pbp_stream.Read(out_buffer, 16 * ISO_BLOCK_SIZE);

        // Output size is a full block
        out_length = 16 * ISO_BLOCK_SIZE;

        // If the block is compressed
    } else {

        // Allocate memory for our input buffer
        in_buffer = (unsigned char*) malloc(iso_index[blockNo].length);
        if(!in_buffer){
        	wxMessageBox(wxS("Unable to allocate memory"),wxS("ERROR"));
        }

        // Read in the number of bytes specified in the index
        pbp_stream.Read(in_buffer, iso_index[blockNo].length);

        // Set up the zlib inflation
        z.zalloc = Z_NULL;
        z.zfree = Z_NULL;
        z.opaque = Z_NULL;
        z.avail_in = 0;
        z.next_in = Z_NULL;
        inflateInit2(&z, -15);

        // Set up the input stream
        z.avail_in = iso_index[blockNo].length;
        z.next_in = in_buffer;

        // Set up the output stream
        z.avail_out = 16 * ISO_BLOCK_SIZE;
        z.next_out = out_buffer;

        // Inflate the data from the PBP
        inflate(&z, Z_NO_FLUSH);

        // Output size should be a full block
        out_length = 16 * ISO_BLOCK_SIZE - z.avail_out;

        // Clean up the input buffer
        free(in_buffer);

        // Clean up the zlib inflation
        inflateEnd(&z);
    }
    pbp_stream.Close();
    return out_length;

}
/*
 * @ returns number of blocks in PBP file
 */
unsigned int popstripInit(INDEX* &iso_index, wxString input_pbp) {
    wxFFile pbp_stream;


    int psar_offset;
    int this_offset;
    int offset;
    int length;
    int dummy[6];
    int count;


    iso_index = (INDEX*) malloc(sizeof (INDEX) * MAX_INDEXES);


    // Open the PBP file
    extractInfo.srcPBP=input_pbp;
    pbp_stream.Open(extractInfo.srcPBP,wxS("rb"));

    if (pbp_stream.IsOpened()==false) {
        wxPrintf(wxS("ERROR: Couldn't open '%s' for reading\n"), input_pbp.c_str());
        return 0;
    }


    // Read in the offset of the PSAR file
    pbp_stream.Seek(HEADER_PSAR_OFFSET);
    pbp_stream.Read(&psar_offset,4);

    // Go to the location of the ISO indexes in the PSAR
    pbp_stream.Seek(psar_offset + PSAR_INDEX_OFFSET);

    // Store the current location in the PBP
    this_offset = pbp_stream.Tell();

    // Reset the counter variable
    count = 0;

    // Read indexes until the start of the ISO file
    while (this_offset < psar_offset + PSAR_ISO_OFFSET) {

        // Read in the block offset from the index
        pbp_stream.Read(&offset, 4);
        // Read in the block length from the index
        pbp_stream.Read(&length, 4);
        // Read in the dummy bytes
        pbp_stream.Read(&dummy, 4 * 6);

        // Record our current location in the PBP
        this_offset = pbp_stream.Tell();

        // Check if this looks like a valid offset
        if (offset != 0 || length != 0) {

            // Store the block offset
            iso_index[count].offset = offset;
            // Store the block length
            iso_index[count].length = length;
            count++;
        }
        if (count >= MAX_INDEXES) {
            free(iso_index);
            pbp_stream.Close();
            return 0;
        }
    }
    pbp_stream.Close();
    // Store the number of indexes we've read in
    return count;

}

unsigned int popstripGetIsoSize(INDEX* iso_index) {
    unsigned char out_buffer[16 * ISO_BLOCK_SIZE];
    int iso_length = 0;
    // The ISO size is contained in the data referenced in index #2
    // If we've just read in index #2, grab the ISO size from the output buffer

    popstripReadBlock(iso_index, 1, out_buffer);
    iso_length = (out_buffer[104] + (out_buffer[105] << 8) + (out_buffer[106] << 16) + (out_buffer[107] << 24)) * ISO_BLOCK_SIZE;


    return iso_length;

}

void popstripFinal(INDEX* &iso_index) {
    free(iso_index);
}
