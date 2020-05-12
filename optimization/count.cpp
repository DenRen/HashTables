#include <cstdio>

unsigned long crc_table_opt1[256] = {};

void CRC32_tablefill () {
	unsigned long crc = 0;

	for (int i = 0; i < 256; i++) {
		crc = i;
		for (int j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
		crc_table_opt1[i] = crc;
	};
}

int main () {
    CRC32_tablefill();
    for (int i = 0; i < 256; ++i)
        printf ("%lu, ", crc_table_opt1[i]);
    return 0;
}