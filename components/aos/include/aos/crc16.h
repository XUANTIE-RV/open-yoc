#ifndef LIBS_CRC_H
#define LIBS_CRC_H

/**
 * crc16 - compute the CRC-16 for the data buffer
 * @crc:    previous CRC value
 * @buffer: data pointer
 * @len:    number of bytes in the buffer
 *
 * Returns the updated CRC value.
 */
unsigned short crc16(unsigned short crc, unsigned char const *buffer, int len);

#endif
