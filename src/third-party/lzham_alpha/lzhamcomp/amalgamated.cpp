#include "lzham_lzbase.cpp"
#include "lzham_lzcomp_internal.cpp"
#include "lzham_lzcomp.cpp"
#include "lzham_lzcomp_state.cpp"
#include "lzham_match_accel.cpp"
#include "lzham_win32_threading.cpp"

// We live in a society
#include "../lzhamdecomp/lzham_assert.cpp"
#include "../lzhamdecomp/lzham_symbol_codec.cpp"
#include "../lzhamdecomp/lzham_mem.cpp"
#include "../lzhamdecomp/lzham_platform.cpp"
#include "../lzhamdecomp/lzham_lzdecompbase.cpp"
#include "../lzhamdecomp/lzham_vector.cpp"
#include "../lzhamdecomp/lzham_checksum.cpp"
//#include "../lzhamdecomp/lzham_polar_codes.cpp"
//#include "../lzhamdecomp/lzham_huffman_codes.cpp"
#include "../lzhamdecomp/lzham_prefix_coding.cpp"

// from lzham_lib.cpp
extern "C" lzham_compress_state_ptr LZHAM_CDECL lzham_compress_init(const lzham_compress_params *pParams)
{
   return lzham::lzham_lib_compress_init(pParams);
}

extern "C" lzham_compress_state_ptr LZHAM_CDECL lzham_compress_reinit(lzham_compress_state_ptr p)
{
   return lzham::lzham_lib_compress_reinit(p);
}

extern "C" lzham_uint32 LZHAM_CDECL lzham_compress_deinit(lzham_compress_state_ptr p)
{
   return lzham::lzham_lib_compress_deinit(p);
}

extern "C" lzham_compress_status_t LZHAM_CDECL lzham_compress(
   lzham_compress_state_ptr p,
   const lzham_uint8 *pIn_buf, size_t *pIn_buf_size, 
   lzham_uint8 *pOut_buf, size_t *pOut_buf_size,
   lzham_bool no_more_input_bytes_flag)
{
   return lzham::lzham_lib_compress(p, pIn_buf, pIn_buf_size, pOut_buf, pOut_buf_size, no_more_input_bytes_flag);
}   

extern "C" lzham_compress_status_t LZHAM_CDECL lzham_compress2(
   lzham_compress_state_ptr p,
   const lzham_uint8 *pIn_buf, size_t *pIn_buf_size, 
   lzham_uint8 *pOut_buf, size_t *pOut_buf_size,
   lzham_flush_t flush_type)
{
   return lzham::lzham_lib_compress2(p, pIn_buf, pIn_buf_size, pOut_buf, pOut_buf_size, flush_type);
}   

extern "C" lzham_compress_status_t LZHAM_CDECL lzham_compress_memory(const lzham_compress_params *pParams, lzham_uint8* pDst_buf, size_t *pDst_len, const lzham_uint8* pSrc_buf, size_t src_len, lzham_uint32 *pAdler32)
{
   return lzham::lzham_lib_compress_memory(pParams, pDst_buf, pDst_len, pSrc_buf, src_len, pAdler32);
}

extern "C" lzham_z_ulong LZHAM_CDECL lzham_z_crc32(lzham_z_ulong crc, const lzham_uint8 *ptr, size_t buf_len)
{
   //return lzham::lzham_lib_z_crc32(crc, ptr, buf_len);
   return lzham::crc32(crc, ptr, buf_len);
}
