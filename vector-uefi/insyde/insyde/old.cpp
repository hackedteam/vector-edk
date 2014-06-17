void scan_module(void *buffer)
{
	uint32_t romSize = 0;
	char *ptr = (char *) buffer;

	memcpy(&romSize, buffer, 3);

	printf("ROM Size: %08x\n", romSize);


	while(romSize > 0x22)
	{
		HEADER_BLOCK *module = (HEADER_BLOCK *) ptr;

		if (module->size >= 0x10000000 && module->size < (0x10000000 + romSize) && 
			module->MZ_Signature == MZ_SIGNATURE)	// dos MZ signature
		{
			wchar_t *name = reinterpret_cast<wchar_t *>(ptr + 4 + (module->size & 0xffffff));

			printf("\tSize: %08x\tName: %S\n", module->size & 0xffffff, name);

			char fname[128];

			sprintf(fname, "%s\\%S", outputdir, name);


			FILE *f = fopen(fname, "wb");

			fwrite(ptr + 4, module->size & 0xffffff, 1, f);

			fclose(f);
			//find_lzma(ptr+4, module->size & 0xffffff);

			ptr = ptr + 4 + (module->size & 0xffffff);
			romSize = romSize - 4 - (module->size & 0xffffff);
		}
		else
		{	// move to next block
			ptr++;
			romSize--;
		}

	}

}

void find_lzma(void *buffer, int size)
{
	static const char LZMA_PATTERN[3] = { 0x5d, 0x00, 0x00 };

	char *search = (char *) buffer;

	while(size > 2)
	{
		if (search[0] == 0x5d 
			&& search[0] < 0xe1 
			&& search[4] < 0x20 
			&& (memcmp (search + 10 , "\x00\x00\x00", 3) == 0
			|| (memcmp (search + 5, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8) == 0)))
		{	// found!
			std::cout << "\tLZMA pattern at " << std::hex << diff_pos(search, buffer);
			LZMA_HEADER *lzma = (LZMA_HEADER *) search;
/*
			extern "C" MY_STDAPI LzmaUncompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  *srcLen,
				const unsigned char *props, size_t propsSize);

			void *dst = malloc(0x100000);

			size_t destLen;

			LzmaUncompress(dst, &destLen, search++, &*/

			lzmadec_stream strm;
			lzmadec_info info;

			memset(&strm, 0, sizeof(lzmadec_stream));
			memset(&info, 0, sizeof(lzmadec_info));

			lzmadec_init(&strm);
		
			lzmadec_buffer_info(&info, (unsigned char*) search, 13);

			if (info.uncompressed_size == 0)
			{
				size--;
				search++;
				continue;
			}
			void *ptr = malloc((int) info.uncompressed_size);

			if (ptr != NULL)
			{
				strm.next_in = (uint8_t *) search;
				strm.avail_in = size;
				strm.next_out = (uint8_t *) ptr;
				strm.avail_out = (size_t) info.uncompressed_size;

				lzmadec_decode(&strm, 1);

				char fname[128];
				sprintf(fname, "%s\\%x.bin", outputdirectory, (uint32_t) search);
				FILE *f = fopen(fname, "wb");
				fwrite(ptr, (int) info.uncompressed_size, 1, f);
				fclose(f);

				free(ptr);

				scan_module(ptr);
			}
			std::cout << " .. size " << info.uncompressed_size << std::endl;
		}

		size--;
		search++;
	}
}
