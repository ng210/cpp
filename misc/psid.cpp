#include "psid.h"
#include "utils/file.h"

PSidHeader* PSidHeader::load(UINT8* buffer) {
/******************************************************************************
 Load the header of a PSID file
 INPUT
  - fp: FILE reference
******************************************************************************/
	PSidHeader header = new PSidHeader();

	header

	fseek(fp, 0, SEEK_END);
	header->dataLength = (WORD)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	int bytesread = fread(header, sizeof(BYTE), PSIDHEADERSIZE, fp);
	if ((bytesread == 0) || (*(int*)&header->magicId != 0x44495350))
	{
		return 0;
	}
	header->dataOffset = _byteswap_ushort(header->dataOffset);
	header->version = _byteswap_ushort(header->version);
	header->initAddr = _byteswap_ushort(header->initAddr);
	header->loadAddr = _byteswap_ushort(header->loadAddr);
	header->playAddr = _byteswap_ushort(header->playAddr);
	header->songs = _byteswap_ushort(header->songs);
	header->startSong = _byteswap_ushort(header->startSong);
	if (header->loadAddr == 0)
	{
		if (header->version == 0x01)
		{
			fseek(fp, offsetof(_PSidHeader, flags), SEEK_SET);
		}
		fread(&header->loadAddr, sizeof(WORD), 1, fp);
		header->dataOffset += 2;
	}
	header->dataLength -= header->dataOffset;
	return header;
}

PSidHeader* PSidHeader::load(const char* fileName) {
/******************************************************************************
 Load the header of a PSID file
 INPUT
  - fileName: name of file
******************************************************************************/
	FILE* fp;
	if ((fp = fopen(fileName, "rb")) == 0)
	{
		return 0;
	}
	PSidHeader* header = PSidHeader::load(fp);
	fclose(fp);
	return header;
}
