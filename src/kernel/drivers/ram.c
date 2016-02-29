#include "kernel.h"
#include "x86/x86.h"
#include "hal.h"
#include "time.h"
#include "string.h"

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
	{
	0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
  0xa0, 0x80, 0x04, 0x08, 0x34, 0x00, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x03, 0x00, 0x28, 0x00,
  0x0f, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x04, 0x08, 0x00, 0x80, 0x04, 0x08, 0xc4, 0x01, 0x00, 0x00,
  0xc4, 0x01, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0xc4, 0x01, 0x00, 0x00, 0xc4, 0x91, 0x04, 0x08,
  0xc4, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x51, 0xe5, 0x74, 0x64,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x8d, 0x4c, 0x24, 0x04, 0x83, 0xe4, 0xf0, 0xff,
  0x71, 0xfc, 0x55, 0x89, 0xe5, 0x53, 0xbb, 0x6b, 0xca, 0x5f, 0x6b, 0x51,
  0xeb, 0x17, 0x8d, 0x76, 0x00, 0x8d, 0xbc, 0x27, 0x00, 0x00, 0x00, 0x00,
  0xa1, 0xc4, 0x91, 0x04, 0x08, 0x83, 0xc0, 0x01, 0xa3, 0xc4, 0x91, 0x04,
  0x08, 0x8b, 0x0d, 0xc4, 0x91, 0x04, 0x08, 0x89, 0xc8, 0xf7, 0xeb, 0x89,
  0xc8, 0xc1, 0xf8, 0x1f, 0xc1, 0xfa, 0x16, 0x29, 0xc2, 0x69, 0xd2, 0x80,
  0x96, 0x98, 0x00, 0x39, 0xd1, 0x75, 0xd5, 0x83, 0xec, 0x0c, 0x68, 0x2f,
  0x81, 0x04, 0x08, 0xe8, 0x28, 0x00, 0x00, 0x00, 0x83, 0xc4, 0x10, 0xeb,
  0xc3, 0x66, 0x90, 0x90, 0x55, 0x89, 0xe5, 0x53, 0x8b, 0x55, 0x14, 0x8b,
  0x4d, 0x10, 0x8b, 0x45, 0x08, 0x8b, 0x5d, 0x0c, 0xcd, 0x80, 0x5b, 0x5d,
  0xc3, 0x8d, 0x74, 0x26, 0x00, 0x8d, 0xbc, 0x27, 0x00, 0x00, 0x00, 0x00,
  0x55, 0x89, 0xe5, 0xff, 0x75, 0x08, 0x6a, 0x00, 0xe8, 0xd3, 0xff, 0xff,
  0xff, 0xc9, 0xc3, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72,
  0x6c, 0x64, 0x21, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x7a, 0x52, 0x00, 0x01, 0x7c, 0x08, 0x01,
  0x1b, 0x0c, 0x04, 0x04, 0x88, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x1c, 0x00, 0x00, 0x00, 0xa0, 0xff, 0xff, 0xff, 0x15, 0x00, 0x00, 0x00,
  0x00, 0x41, 0x0e, 0x08, 0x85, 0x02, 0x42, 0x0d, 0x05, 0x41, 0x83, 0x03,
  0x4f, 0xc3, 0x41, 0xc5, 0x0c, 0x04, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x9c, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x41, 0x0e, 0x08, 0x85, 0x02, 0x42, 0x0d, 0x05, 0x4b, 0xc5, 0x0c,
  0x04, 0x04, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
  0xfc, 0xfe, 0xff, 0xff, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x44, 0x0c, 0x01,
  0x00, 0x47, 0x10, 0x05, 0x02, 0x75, 0x00, 0x43, 0x10, 0x03, 0x02, 0x75,
  0x7c, 0x46, 0x0f, 0x03, 0x75, 0x78, 0x06, 0x00, 0x47, 0x43, 0x43, 0x3a,
  0x20, 0x28, 0x47, 0x4e, 0x55, 0x29, 0x20, 0x35, 0x2e, 0x33, 0x2e, 0x30,
  0x00, 0x1c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x04, 0x08, 0x2f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00,
  0x00, 0x02, 0x00, 0xb5, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xa0, 0x80, 0x04, 0x08, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xb1, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x04, 0x01, 0x14, 0x00, 0x00, 0x00, 0x0c, 0x05, 0x00,
  0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x81, 0x04, 0x08, 0x2f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x9b, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x69, 0x00, 0x00, 0x00, 0x00, 0x81, 0x04, 0x08, 0x15, 0x00, 0x00,
  0x00, 0x01, 0x9c, 0x69, 0x00, 0x00, 0x00, 0x03, 0x69, 0x64, 0x00, 0x01,
  0x04, 0x69, 0x00, 0x00, 0x00, 0x02, 0x91, 0x00, 0x04, 0x05, 0x72, 0x65,
  0x74, 0x00, 0x01, 0x05, 0x69, 0x00, 0x00, 0x00, 0x01, 0x50, 0x06, 0xa3,
  0x00, 0x00, 0x00, 0x01, 0x06, 0x70, 0x00, 0x00, 0x00, 0x03, 0x91, 0x00,
  0x9f, 0x00, 0x07, 0x04, 0x05, 0x69, 0x6e, 0x74, 0x00, 0x08, 0x04, 0x69,
  0x00, 0x00, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x0b, 0x69, 0x00,
  0x00, 0x00, 0x20, 0x81, 0x04, 0x08, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x9c,
  0xa7, 0x00, 0x00, 0x00, 0x03, 0x62, 0x75, 0x66, 0x00, 0x01, 0x0b, 0xa7,
  0x00, 0x00, 0x00, 0x02, 0x91, 0x00, 0x09, 0x2d, 0x81, 0x04, 0x08, 0x25,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0xad, 0x00, 0x00, 0x00, 0x0a, 0x01,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6b, 0x00, 0x00, 0x00, 0x04, 0x00,
  0x8c, 0x00, 0x00, 0x00, 0x04, 0x01, 0x14, 0x00, 0x00, 0x00, 0x0c, 0xc9,
  0x00, 0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x02, 0xd1, 0x00, 0x00, 0x00,
  0x01, 0x04, 0x48, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08, 0x5d, 0x00,
  0x00, 0x00, 0x01, 0x9c, 0x48, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x80, 0x04,
  0x08, 0x63, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x05, 0x69, 0x6e, 0x74,
  0x00, 0x05, 0x78, 0x00, 0x01, 0x03, 0x5e, 0x00, 0x00, 0x00, 0x05, 0x03,
  0xc4, 0x91, 0x04, 0x08, 0x06, 0x48, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x00,
  0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x01, 0x11, 0x01,
  0x25, 0x0e, 0x13, 0x0b, 0x03, 0x0e, 0x1b, 0x0e, 0x11, 0x01, 0x12, 0x06,
  0x10, 0x17, 0x00, 0x00, 0x02, 0x2e, 0x01, 0x3f, 0x19, 0x03, 0x0e, 0x3a,
  0x0b, 0x3b, 0x0b, 0x27, 0x19, 0x49, 0x13, 0x11, 0x01, 0x12, 0x06, 0x40,
  0x18, 0x97, 0x42, 0x19, 0x01, 0x13, 0x00, 0x00, 0x03, 0x05, 0x00, 0x03,
  0x08, 0x3a, 0x0b, 0x3b, 0x0b, 0x49, 0x13, 0x02, 0x18, 0x00, 0x00, 0x04,
  0x18, 0x00, 0x00, 0x00, 0x05, 0x34, 0x00, 0x03, 0x08, 0x3a, 0x0b, 0x3b,
  0x0b, 0x49, 0x13, 0x02, 0x18, 0x00, 0x00, 0x06, 0x34, 0x00, 0x03, 0x0e,
  0x3a, 0x0b, 0x3b, 0x0b, 0x49, 0x13, 0x02, 0x18, 0x00, 0x00, 0x07, 0x24,
  0x00, 0x0b, 0x0b, 0x3e, 0x0b, 0x03, 0x08, 0x00, 0x00, 0x08, 0x0f, 0x00,
  0x0b, 0x0b, 0x49, 0x13, 0x00, 0x00, 0x09, 0x89, 0x82, 0x01, 0x00, 0x11,
  0x01, 0x31, 0x13, 0x00, 0x00, 0x0a, 0x24, 0x00, 0x0b, 0x0b, 0x3e, 0x0b,
  0x03, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x11, 0x01, 0x25, 0x0e, 0x13, 0x0b,
  0x03, 0x0e, 0x1b, 0x0e, 0x55, 0x17, 0x11, 0x01, 0x10, 0x17, 0x00, 0x00,
  0x02, 0x2e, 0x01, 0x3f, 0x19, 0x03, 0x0e, 0x3a, 0x0b, 0x3b, 0x0b, 0x49,
  0x13, 0x87, 0x01, 0x19, 0x11, 0x01, 0x12, 0x06, 0x40, 0x18, 0x97, 0x42,
  0x19, 0x01, 0x13, 0x00, 0x00, 0x03, 0x89, 0x82, 0x01, 0x00, 0x11, 0x01,
  0x31, 0x13, 0x00, 0x00, 0x04, 0x24, 0x00, 0x0b, 0x0b, 0x3e, 0x0b, 0x03,
  0x08, 0x00, 0x00, 0x05, 0x34, 0x00, 0x03, 0x08, 0x3a, 0x0b, 0x3b, 0x0b,
  0x49, 0x13, 0x3f, 0x19, 0x02, 0x18, 0x00, 0x00, 0x06, 0x35, 0x00, 0x49,
  0x13, 0x00, 0x00, 0x07, 0x2e, 0x00, 0x3f, 0x19, 0x3c, 0x19, 0x6e, 0x0e,
  0x03, 0x0e, 0x3a, 0x0b, 0x3b, 0x0b, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x01, 0xfb, 0x0e, 0x0d,
  0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x73, 0x79, 0x73, 0x63, 0x61, 0x6c, 0x6c, 0x2e, 0x63, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x81, 0x04, 0x08, 0x15, 0x4d,
  0xd8, 0xd8, 0x3d, 0x9f, 0x02, 0x02, 0x00, 0x01, 0x01, 0x44, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x01, 0x01, 0xfb, 0x0e, 0x0d,
  0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x74, 0x65, 0x73, 0x74, 0x30, 0x2e, 0x63, 0x00, 0x00, 0x00, 0x00,
  0x73, 0x79, 0x73, 0x63, 0x61, 0x6c, 0x6c, 0x2e, 0x68, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x02, 0xa0, 0x80, 0x04, 0x08, 0x15, 0xd8, 0x56,
  0xce, 0xc4, 0x08, 0xc9, 0x02, 0x12, 0x00, 0x01, 0x01, 0x63, 0x68, 0x61,
  0x72, 0x00, 0x73, 0x79, 0x73, 0x63, 0x61, 0x6c, 0x6c, 0x2e, 0x63, 0x00,
  0x70, 0x75, 0x74, 0x73, 0x00, 0x47, 0x4e, 0x55, 0x20, 0x43, 0x31, 0x31,
  0x20, 0x35, 0x2e, 0x33, 0x2e, 0x30, 0x20, 0x2d, 0x6d, 0x33, 0x32, 0x20,
  0x2d, 0x6d, 0x74, 0x75, 0x6e, 0x65, 0x3d, 0x67, 0x65, 0x6e, 0x65, 0x72,
  0x69, 0x63, 0x20, 0x2d, 0x6d, 0x61, 0x72, 0x63, 0x68, 0x3d, 0x70, 0x65,
  0x6e, 0x74, 0x69, 0x75, 0x6d, 0x70, 0x72, 0x6f, 0x20, 0x2d, 0x67, 0x67,
  0x64, 0x62, 0x20, 0x2d, 0x4f, 0x32, 0x20, 0x2d, 0x66, 0x6e, 0x6f, 0x2d,
  0x62, 0x75, 0x69, 0x6c, 0x74, 0x69, 0x6e, 0x20, 0x2d, 0x66, 0x6e, 0x6f,
  0x2d, 0x73, 0x74, 0x61, 0x63, 0x6b, 0x2d, 0x70, 0x72, 0x6f, 0x74, 0x65,
  0x63, 0x74, 0x6f, 0x72, 0x20, 0x2d, 0x66, 0x6e, 0x6f, 0x2d, 0x6f, 0x6d,
  0x69, 0x74, 0x2d, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x2d, 0x70, 0x6f, 0x69,
  0x6e, 0x74, 0x65, 0x72, 0x20, 0x2d, 0x66, 0x67, 0x6e, 0x75, 0x38, 0x39,
  0x2d, 0x69, 0x6e, 0x6c, 0x69, 0x6e, 0x65, 0x00, 0x73, 0x79, 0x73, 0x63,
  0x61, 0x6c, 0x6c, 0x00, 0x61, 0x72, 0x67, 0x73, 0x00, 0x2f, 0x68, 0x6f,
  0x6d, 0x65, 0x2f, 0x64, 0x6f, 0x67, 0x65, 0x2f, 0x6f, 0x73, 0x2d, 0x6c,
  0x61, 0x62, 0x73, 0x2f, 0x6f, 0x73, 0x2d, 0x6c, 0x61, 0x62, 0x31, 0x2f,
  0x74, 0x65, 0x73, 0x74, 0x34, 0x00, 0x74, 0x65, 0x73, 0x74, 0x30, 0x2e,
  0x63, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0xa0, 0x80, 0x04, 0x08, 0xfd,
  0x80, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2f, 0x81, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x40, 0x81, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x91, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0b, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0xf1, 0xff, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xf1, 0xff, 0x13, 0x00, 0x00, 0x00,
  0xc4, 0x91, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00, 0x11, 0x00, 0x04, 0x00,
  0x15, 0x00, 0x00, 0x00, 0x00, 0x81, 0x04, 0x08, 0x15, 0x00, 0x00, 0x00,
  0x12, 0x00, 0x01, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x20, 0x81, 0x04, 0x08,
  0x0f, 0x00, 0x00, 0x00, 0x12, 0x00, 0x01, 0x00, 0x22, 0x00, 0x00, 0x00,
  0xc4, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x04, 0x00,
  0x2e, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08, 0x5d, 0x00, 0x00, 0x00,
  0x12, 0x00, 0x01, 0x00, 0x33, 0x00, 0x00, 0x00, 0xc4, 0x91, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x04, 0x00, 0x3a, 0x00, 0x00, 0x00,
  0xc8, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x04, 0x00,
  0x00, 0x73, 0x79, 0x73, 0x63, 0x61, 0x6c, 0x6c, 0x2e, 0x63, 0x00, 0x74,
  0x65, 0x73, 0x74, 0x30, 0x2e, 0x63, 0x00, 0x78, 0x00, 0x73, 0x79, 0x73,
  0x63, 0x61, 0x6c, 0x6c, 0x00, 0x70, 0x75, 0x74, 0x73, 0x00, 0x5f, 0x5f,
  0x62, 0x73, 0x73, 0x5f, 0x73, 0x74, 0x61, 0x72, 0x74, 0x00, 0x6d, 0x61,
  0x69, 0x6e, 0x00, 0x5f, 0x65, 0x64, 0x61, 0x74, 0x61, 0x00, 0x5f, 0x65,
  0x6e, 0x64, 0x00, 0x00, 0x2e, 0x73, 0x79, 0x6d, 0x74, 0x61, 0x62, 0x00,
  0x2e, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62, 0x00, 0x2e, 0x73, 0x68, 0x73,
  0x74, 0x72, 0x74, 0x61, 0x62, 0x00, 0x2e, 0x74, 0x65, 0x78, 0x74, 0x00,
  0x2e, 0x72, 0x6f, 0x64, 0x61, 0x74, 0x61, 0x00, 0x2e, 0x65, 0x68, 0x5f,
  0x66, 0x72, 0x61, 0x6d, 0x65, 0x00, 0x2e, 0x62, 0x73, 0x73, 0x00, 0x2e,
  0x63, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x2e, 0x64, 0x65, 0x62,
  0x75, 0x67, 0x5f, 0x61, 0x72, 0x61, 0x6e, 0x67, 0x65, 0x73, 0x00, 0x2e,
  0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x69, 0x6e, 0x66, 0x6f, 0x00, 0x2e,
  0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x61, 0x62, 0x62, 0x72, 0x65, 0x76,
  0x00, 0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x6c, 0x69, 0x6e, 0x65,
  0x00, 0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x73, 0x74, 0x72, 0x00,
  0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x72, 0x61, 0x6e, 0x67, 0x65,
  0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08,
  0xa0, 0x00, 0x00, 0x00, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
  0x2f, 0x81, 0x04, 0x08, 0x2f, 0x01, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x40, 0x81, 0x04, 0x08, 0x40, 0x01, 0x00, 0x00,
  0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xc4, 0x91, 0x04, 0x08,
  0xc4, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x38, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xc4, 0x01, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd5, 0x01, 0x00, 0x00,
  0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x15, 0x02, 0x00, 0x00, 0x24, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x5c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x39, 0x03, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x04, 0x00, 0x00,
  0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xb9, 0x04, 0x00, 0x00, 0xd6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x81, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x8f, 0x05, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x07, 0x00, 0x00,
  0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xa0, 0x05, 0x00, 0x00, 0x50, 0x01, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00,
  0x0e, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0x06, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00	
	},	// the first file '0'
	{"Hello Beautiful World!\n"},		// the second file '1'
	{
		0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
  0xa0, 0x80, 0x04, 0x08, 0x34, 0x00, 0x00, 0x00, 0xb4, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x03, 0x00, 0x28, 0x00,
  0x0e, 0x00, 0x0b, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x04, 0x08, 0x00, 0x80, 0x04, 0x08, 0x18, 0x01, 0x00, 0x00,
  0x18, 0x01, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08,
  0x18, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x51, 0xe5, 0x74, 0x64,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x55, 0x89, 0xe5, 0x53, 0xbb, 0x6b, 0xca, 0x5f,
  0x6b, 0x8d, 0xb4, 0x26, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x0d, 0x18, 0x91,
  0x04, 0x08, 0x89, 0xc8, 0xf7, 0xeb, 0x89, 0xc8, 0xc1, 0xf8, 0x1f, 0xc1,
  0xfa, 0x16, 0x29, 0xc2, 0x69, 0xd2, 0x80, 0x96, 0x98, 0x00, 0x39, 0xd1,
  0x75, 0x07, 0xb8, 0x01, 0x00, 0x00, 0x00, 0xcd, 0x80, 0xa1, 0x18, 0x91,
  0x04, 0x08, 0x83, 0xc0, 0x01, 0xa3, 0x18, 0x91, 0x04, 0x08, 0xeb, 0xcc,
  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7a, 0x52, 0x00,
  0x01, 0x7c, 0x08, 0x01, 0x1b, 0x0c, 0x04, 0x04, 0x88, 0x01, 0x00, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x9c, 0xff, 0xff, 0xff,
  0x44, 0x00, 0x00, 0x00, 0x00, 0x41, 0x0e, 0x08, 0x85, 0x02, 0x42, 0x0d,
  0x05, 0x41, 0x83, 0x03, 0x47, 0x43, 0x43, 0x3a, 0x20, 0x28, 0x47, 0x4e,
  0x55, 0x29, 0x20, 0x35, 0x2e, 0x33, 0x2e, 0x30, 0x00, 0x1c, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xa0, 0x80, 0x04, 0x08, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x04, 0x01, 0x29, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xb0, 0x00, 0x00, 0x00, 0x01,
  0x02, 0x3a, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08, 0x44, 0x00, 0x00,
  0x00, 0x01, 0x9c, 0x03, 0x04, 0x05, 0x69, 0x6e, 0x74, 0x00, 0x04, 0x78,
  0x00, 0x01, 0x01, 0x50, 0x00, 0x00, 0x00, 0x05, 0x03, 0x18, 0x91, 0x04,
  0x08, 0x05, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x01, 0x25, 0x0e,
  0x13, 0x0b, 0x03, 0x0e, 0x1b, 0x0e, 0x55, 0x17, 0x11, 0x01, 0x10, 0x17,
  0x00, 0x00, 0x02, 0x2e, 0x00, 0x3f, 0x19, 0x03, 0x0e, 0x3a, 0x0b, 0x3b,
  0x0b, 0x49, 0x13, 0x87, 0x01, 0x19, 0x11, 0x01, 0x12, 0x06, 0x40, 0x18,
  0x97, 0x42, 0x19, 0x00, 0x00, 0x03, 0x24, 0x00, 0x0b, 0x0b, 0x3e, 0x0b,
  0x03, 0x08, 0x00, 0x00, 0x04, 0x34, 0x00, 0x03, 0x08, 0x3a, 0x0b, 0x3b,
  0x0b, 0x49, 0x13, 0x3f, 0x19, 0x02, 0x18, 0x00, 0x00, 0x05, 0x35, 0x00,
  0x49, 0x13, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1e,
  0x00, 0x00, 0x00, 0x01, 0x01, 0xfb, 0x0e, 0x0d, 0x00, 0x01, 0x01, 0x01,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x74, 0x65, 0x73,
  0x74, 0x30, 0x2e, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x02,
  0xa0, 0x80, 0x04, 0x08, 0x13, 0x4c, 0x02, 0x2a, 0x13, 0x77, 0xc9, 0x02,
  0x02, 0x00, 0x01, 0x01, 0x74, 0x65, 0x73, 0x74, 0x30, 0x2e, 0x63, 0x00,
  0x2f, 0x68, 0x6f, 0x6d, 0x65, 0x2f, 0x64, 0x6f, 0x67, 0x65, 0x2f, 0x6f,
  0x73, 0x2d, 0x6c, 0x61, 0x62, 0x73, 0x2f, 0x6f, 0x73, 0x2d, 0x6c, 0x61,
  0x62, 0x31, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x32, 0x00, 0x47, 0x4e, 0x55,
  0x20, 0x43, 0x31, 0x31, 0x20, 0x35, 0x2e, 0x33, 0x2e, 0x30, 0x20, 0x2d,
  0x6d, 0x33, 0x32, 0x20, 0x2d, 0x6d, 0x74, 0x75, 0x6e, 0x65, 0x3d, 0x67,
  0x65, 0x6e, 0x65, 0x72, 0x69, 0x63, 0x20, 0x2d, 0x6d, 0x61, 0x72, 0x63,
  0x68, 0x3d, 0x70, 0x65, 0x6e, 0x74, 0x69, 0x75, 0x6d, 0x70, 0x72, 0x6f,
  0x20, 0x2d, 0x67, 0x67, 0x64, 0x62, 0x20, 0x2d, 0x4f, 0x32, 0x20, 0x2d,
  0x66, 0x6e, 0x6f, 0x2d, 0x62, 0x75, 0x69, 0x6c, 0x74, 0x69, 0x6e, 0x20,
  0x2d, 0x66, 0x6e, 0x6f, 0x2d, 0x73, 0x74, 0x61, 0x63, 0x6b, 0x2d, 0x70,
  0x72, 0x6f, 0x74, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x20, 0x2d, 0x66, 0x6e,
  0x6f, 0x2d, 0x6f, 0x6d, 0x69, 0x74, 0x2d, 0x66, 0x72, 0x61, 0x6d, 0x65,
  0x2d, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x20, 0x2d, 0x66, 0x67,
  0x6e, 0x75, 0x38, 0x39, 0x2d, 0x69, 0x6e, 0x6c, 0x69, 0x6e, 0x65, 0x00,
  0x6d, 0x61, 0x69, 0x6e, 0x00, 0xa0, 0x80, 0x04, 0x08, 0xe4, 0x80, 0x04,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xe4, 0x80, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0a, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xf1, 0xff,
  0x09, 0x00, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00,
  0x11, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x17, 0x00, 0x00, 0x00,
  0xa0, 0x80, 0x04, 0x08, 0x44, 0x00, 0x00, 0x00, 0x12, 0x00, 0x01, 0x00,
  0x1c, 0x00, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x03, 0x00, 0x23, 0x00, 0x00, 0x00, 0x1c, 0x91, 0x04, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x74, 0x65, 0x73,
  0x74, 0x30, 0x2e, 0x63, 0x00, 0x78, 0x00, 0x5f, 0x5f, 0x62, 0x73, 0x73,
  0x5f, 0x73, 0x74, 0x61, 0x72, 0x74, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00,
  0x5f, 0x65, 0x64, 0x61, 0x74, 0x61, 0x00, 0x5f, 0x65, 0x6e, 0x64, 0x00,
  0x00, 0x2e, 0x73, 0x79, 0x6d, 0x74, 0x61, 0x62, 0x00, 0x2e, 0x73, 0x74,
  0x72, 0x74, 0x61, 0x62, 0x00, 0x2e, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74,
  0x61, 0x62, 0x00, 0x2e, 0x74, 0x65, 0x78, 0x74, 0x00, 0x2e, 0x65, 0x68,
  0x5f, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x00, 0x2e, 0x62, 0x73, 0x73, 0x00,
  0x2e, 0x63, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74, 0x00, 0x2e, 0x64, 0x65,
  0x62, 0x75, 0x67, 0x5f, 0x61, 0x72, 0x61, 0x6e, 0x67, 0x65, 0x73, 0x00,
  0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x69, 0x6e, 0x66, 0x6f, 0x00,
  0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x61, 0x62, 0x62, 0x72, 0x65,
  0x76, 0x00, 0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x6c, 0x69, 0x6e,
  0x65, 0x00, 0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x73, 0x74, 0x72,
  0x00, 0x2e, 0x64, 0x65, 0x62, 0x75, 0x67, 0x5f, 0x72, 0x61, 0x6e, 0x67,
  0x65, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xa0, 0x80, 0x04, 0x08,
  0xa0, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x21, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0xe4, 0x80, 0x04, 0x08, 0xe4, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x18, 0x91, 0x04, 0x08, 0x18, 0x01, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x01, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x39, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x29, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x01, 0x00, 0x00,
  0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x9f, 0x01, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x62, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf1, 0x01, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x02, 0x00, 0x00,
  0xb5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xe1, 0x02, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x2c, 0x04, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x02, 0x00, 0x00,
  0x10, 0x01, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x04, 0x04, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	}	// the third file '2'
};
static uint8_t *disk = (void*)file;

pid_t RAM;

static void ram_driver_thread(void);
static void read_ram_request(Msg *m);

void init_ram(void){
	PCB *p = create_kthread(ram_driver_thread);
	RAM = p->pid;
	wakeup(p);
	hal_register("ram", RAM, 0);
}

static void
ram_driver_thread(void){
	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else{
			switch(m.type){
				case DEV_READ:
					read_ram_request(&m);
					break;
				default:
					assert(0);
			}
		}
	}
}

static void read_ram_request(Msg *m){
	uint8_t *buf = m->buf;
	off_t offset = m->offset;
	size_t len = m->len;
	int i;
	for(i = 0; i < len; ++i)
		buf[i] = disk[offset + i];
	m->ret = m->len;
	pid_t dest = m->src;
	m->src = current->pid;
	send(dest, m);
}
