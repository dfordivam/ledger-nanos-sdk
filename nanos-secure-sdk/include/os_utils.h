#pragma once

#include "arch.h"

/**
 * Helper to perform compilation time assertions
 */
#if !defined(SYSCALL_GENERATE) &&                                              \
    (defined(__clang__) ||                                                     \
     (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 6))))
#define CCASSERT(id, predicate) _Static_assert(predicate, #id)
#else
#define CCASSERT(id, predicate) __x_CCASSERT_LINE(predicate, id, __LINE__)
#define __x_CCASSERT_LINE(predicate, file, line)                               \
  __xx_CCASSERT_LINE(predicate, file, line)
#define __xx_CCASSERT_LINE(predicate, file, line)                              \
  typedef char CCASSERT_##file##_line_##line[((predicate) ? 1 : -1)]
#endif

#define U2(hi, lo) ((((hi)&0xFFu) << 8) | ((lo)&0xFFu))
#define U4(hi3, hi2, lo1, lo0)                                                 \
  ((((hi3)&0xFFu) << 24) | (((hi2)&0xFFu) << 16) | (((lo1)&0xFFu) << 8) |      \
   ((lo0)&0xFFu))
static inline uint16_t U2BE(const uint8_t *buf, size_t off) {
  return (buf[off] << 8) | buf[off + 1];
}
static inline uint32_t U4BE(const uint8_t *buf, size_t off) {
  return (((uint32_t)buf[off]) << 24) | (buf[off + 1] << 16) |
         (buf[off + 2] << 8) | buf[off + 3];
}
static inline uint16_t U2LE(const uint8_t *buf, size_t off) {
  return (buf[off + 1] << 8) | buf[off];
}
static inline uint32_t U4LE(const uint8_t *buf, size_t off) {
  return (((uint32_t)buf[off + 3]) << 24) | (buf[off + 2] << 16) |
         (buf[off + 1] << 8) | buf[off];
}

static inline void U2BE_ENCODE(uint8_t *buf, size_t off, uint32_t value) {
  buf[off + 0] = (value >> 8) & 0xFF;
  buf[off + 1] = value & 0xFF;
}
static inline void U4BE_ENCODE(uint8_t *buf, size_t off, uint32_t value) {
  buf[off + 0] = (value >> 24) & 0xFF;
  buf[off + 1] = (value >> 16) & 0xFF;
  buf[off + 2] = (value >> 8) & 0xFF;
  buf[off + 3] = value & 0xFF;
}
static inline void U2LE_ENCODE(uint8_t *buf, size_t off, uint32_t value) {
  buf[off + 1] = (value >> 8) & 0xFF;
  buf[off + 0] = value & 0xFF;
}
static inline void U4LE_ENCODE(uint8_t *buf, size_t off, uint32_t value) {
  buf[off + 3] = (value >> 24) & 0xFF;
  buf[off + 2] = (value >> 16) & 0xFF;
  buf[off + 1] = (value >> 8) & 0xFF;
  buf[off + 0] = value & 0xFF;
}

void u4be_encode(unsigned char *buffer, unsigned int offset,
                 unsigned int value);
void u4le_encode(unsigned char *buffer, unsigned int offset,
                 unsigned int value);

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))
#define INARRAY(elementptr, array)                                             \
  ((unsigned int)elementptr >= (unsigned int)array &&                          \
   (unsigned int)elementptr < ((unsigned int)array) + sizeof(array))

/* ----------------------------------------------------------------------- */
/* -                            OS FUNCTIONS                             - */
/* ----------------------------------------------------------------------- */
#define os_swap_u16(u16)                                                       \
  ((((unsigned short)(u16) << 8) & 0xFF00U) |                                  \
   (((unsigned short)(u16) >> 8) & 0x00FFU))

#define os_swap_u32(u32)                                                       \
  (((unsigned long int)(u32) >> 24) |                                          \
   (((unsigned long int)(u32) << 8) & 0x00FF0000UL) |                          \
   (((unsigned long int)(u32) >> 8) & 0x0000FF00UL) |                          \
   ((unsigned long int)(u32) << 24))

void *os_memmove(void *dst, const void *src, unsigned int length);
#define os_memcpy os_memmove

void os_memset(void *dst, unsigned char c, unsigned int length);
char os_memcmp(const void WIDE *buf1, const void WIDE *buf2,
               unsigned int length);
void os_memset4(void *dst, unsigned int initval, unsigned int nbintval);
void os_xor(void *dst, void WIDE *src1, void WIDE *src2, unsigned int length);

// Secure memory comparison
char os_secure_memcmp(void *src1, void *src2, unsigned int length);

// Alignement-related
#define UPPER_ALIGN(adr, align, type)                                          \
  (type)((type)((type)(adr) +                                                  \
                (type)((type)((type)MAX((type)(align), (type)1UL)) -           \
                       (type)1UL)) &                                           \
         (type)(~(type)((type)((type)MAX(((type)align), (type)1UL)) -          \
                        (type)1UL)))
#define LOWER_ALIGN(adr, align, type)                                          \
  ((type)(adr) &                                                               \
   (type)((type) ~(type)(((type)MAX((type)(align), (type)1UL)) - (type)1UL)))

#ifdef macro_offsetof
#define offsetof(type, field) ((unsigned int)&(((type *)NULL)->field))
#endif
