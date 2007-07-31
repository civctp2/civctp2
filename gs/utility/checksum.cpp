



























#include "c3.h"

#include "Checksum.h"


#define k_A_INIT 0x01234567
#define k_B_INIT 0x89abcdef
#define k_C_INIT 0xfedcba98
#define k_D_INIT 0x76543210








CheckSum::CheckSum()

{
    
   m_stateA = k_A_INIT; 
   m_stateB = k_B_INIT; 
   m_stateC = k_C_INIT; 
   m_stateD = k_D_INIT;

   m_p1 = (uint8 *)m_buffer; 
   m_end_ptr = (uint8 *) &(m_buffer[16]);  
   m_pad_end_ptr = (uint8 *) &(m_buffer[15]);
   m_total = 0;  
}








inline uint32 Checksum::F(const uint32 x, const uint32 y, const uint32 z) const 

{ 
   return (x & y) | ((~x) & z); 
}








inline uint32 Checksum::G(const uint32 x, const uint32 y, const uint32 z) const 

{ 
   return (x & z) | (y & (~z));
}








inline uint32 Checksum::H(const uint32 x, const uint32 y, const uint32 z) const 

{
   return x ^ y ^ z;
}








inline uint32 Checksum::I(const uint32 x, const uint32 y, const uint32 z) const 

{ 
   return y ^ (x | (~z)); 
}








uint32 Checksum::CircleShift (uint32 val, const uint32 s) const

{
   uint32 tmp = val >> (32 - s); 
   tmp |= val << s; 

   return tmp; 
}








void Checksum::FF (uint32 &i, const uint32 j, const uint32 k, const uint32 l, 
                   const uint32 val, const uint32 s, const uint32 t) const 

{ 
   i = j + CircleShift((i + F(j,k,l) + val + t), s);
}








void Checksum::GG(uint32 &i, const uint32 j, const uint32 k, const uint32 l, const uint32 val, 
        const uint32 s, const uint32 t) const 

{ 
   i = j + CircleShift((i + G(j,k,l) + val + t), s);
}








void Checksum::HH(uint32 &i, const uint32 j, const uint32 k, const uint32 l, const uint32 val, 
        const uint32 s, const uint32 t) const 

{ 
   i = j + CircleShift((i + H(j,k,l) + val + t), s);
}








void Checksum::II(uint32 &i, const uint32 j, const uint32 k, const uint32 l, const uint32 val, 
        const uint32 s, const uint32 t) const 

{ 
   i = j + CircleShift((i + I(j,k,l) + val + t), s);
}









void Checksum::Hash ()

{ 
   uint32 a, b, c, d;

   Assert(m_p1 == m_end_ptr);

   a = m_stateA; 
   b = m_stateB; 
   c = m_stateC; 
   d = m_stateD; 

   FF(a, b, c, d, m_buffer[0], 7,  0xd76aa478); 
   FF(d, a, b, c, m_buffer[1], 12, 0xe8c7b756);
   FF(c, d, a, b, m_buffer[2], 17, 0x242070db);
   FF(b, c, d, a, m_buffer[3], 22, 0xc1bdceee); 

   FF(a, b, c, d, m_buffer[4], 7,  0xf57c0faf);
   FF(d, a, b, c, m_buffer[5], 12, 0x4787c62a);
   FF(c, d, a, b, m_buffer[6], 17, 0xa8304613);
   FF(b, c, d, a, m_buffer[7], 22, 0xfd469501); 

   FF(a, b, c, d, m_buffer[8], 7,  0x698098d8);
   FF(d, a, b, c, m_buffer[9], 12, 0x8b44f7af);
   FF(c, d, a, b, m_buffer[10], 17, 0xffff5bb1); 
   FF(b, c, d, a, m_buffer[11], 22, 0x895cd7be); 

   FF(a, b, c, d, m_buffer[12], 7,  0x6b901122); 
   FF(d, a, b, c, m_buffer[13], 12, 0xfd987193);
   FF(c, d, a, b, m_buffer[14], 17, 0xa679438e);
   FF(b, c, d, a, m_buffer[15], 22, 0x49b40821); 

   GG(a, b, c, d, m_buffer[1], 5,   0xf61e2562); 
   GG(d, a, b, c, m_buffer[6], 9,   0xc040b340); 
   GG(c, d, a, b, m_buffer[11], 14, 0x265e5a51); 
   GG(b, c, d, a, m_buffer[0], 20,  0xe9b6c7aa); 

   GG(a, b, c, d, m_buffer[5], 5,   0xd62f105d); 
   GG(d, a, b, c, m_buffer[10], 9,  0x02441453); 
   GG(c, d, a, b, m_buffer[15], 14, 0xd8a1e681); 
   GG(b, c, d, a, m_buffer[4], 20,  0xe7d3fbc8); 

   GG(a, b, c, d, m_buffer[9], 5,  0x21e1cde6);
   GG(d, a, b, c, m_buffer[14], 9, 0xc33707d6); 
   GG(c, d, a, b, m_buffer[3], 14, 0xf4d50d87); 
   GG(b, c, d, a, m_buffer[8], 20, 0x455a14ed);

   GG(a, d, c, d, m_buffer[13], 5,  0xa9e3e905); 
   GG(d, a, b, c, m_buffer[2], 9,   0xfcefa3f8); 
   GG(c, d, a, b, m_buffer[7], 14,  0x676f02d9); 
   GG(b, c, d, a, m_buffer[12], 20, 0x8d2a4c8a); 

   HH(a, b, c, d, m_buffer[5],   4, 0xfffa3942);
   HH(d, a, b, c, m_buffer[8],  11, 0x8771f681);
   HH(c, d, a, b, m_buffer[11], 16, 0x6d9d6122); 
   HH(b, c, d, a, m_buffer[14], 23, 0xfde5380c); 

   HH(a, b, c, d, m_buffer[1], 4,   0xa4beea44); 
   HH(d, a, b, c, m_buffer[4], 11,  0x4bdecfa9); 
   HH(c, d, a, b, m_buffer[7], 16,  0xf6bb4b60); 
   HH(b, c, d, a, m_buffer[10], 23, 0xbebfbc70); 

   HH(a, b, c, d, m_buffer[13], 4, 0x289b7ec6); 
   HH(d, a, b, c, m_buffer[0], 11, 0xeaa127fa); 
   HH(c, d, a, b, m_buffer[3], 16, 0xd4ef3085);
   HH(b, c, d, a, m_buffer[6], 23, 0x04881d05); 

   HH(a, b, c, d, m_buffer[9], 4,   0xd9d4d039); 
   HH(d, a, b, c, m_buffer[12], 11, 0xe6db99e5); 
   HH(c, d, a, b, m_buffer[15], 16, 0x1fa27cf8); 
   HH(b, c, d, a, m_buffer[2], 23,  0xc4ac5665); 

   II(a, b, c, d, m_buffer[0], 6,   0xf4292244); 
   II(d, a, b, c, m_buffer[7], 10,  0x432aff97); 
   II(c, d, a, b, m_buffer[14], 15, 0xab9423a7); 
   II(b, c, d, a, m_buffer[5], 21,  0xfc93a039); 

   II(a, b, c, d, m_buffer[12], 6,  0x655b59c3); 
   II(d, a, b, c, m_buffer[3], 10,  0x8f0ccc92); 
   II(c, d, a, b, m_buffer[10], 15, 0xffeff47d);
   II(b, c, d, a, m_buffer[1], 21,  0x85845dd1); 

   II(a, b, c, d, m_buffer[8], 6,   0x6fa87e4f); 
   II(d, a, b, c, m_buffer[15], 10, 0xfe2ce6e0); 
   II(c, d, a, b, m_buffer[6], 15,  0xa3014314);
   II(b, c, d, a, m_buffer[13], 21, 0x4e0811a1); 

   II(a, b, c, d, m_buffer[4], 6,   0xf7537e82);
   II(d, a, b, c, m_buffer[11], 10, 0xbd3af235); 
   II(c, d, a, b, m_buffer[2], 15,  0x2ad7d2bb); 
   II(b, c, d, a, m_buffer[9], 21,  0xeb86d391);

   m_stateA += a; 
   m_stateB += b; 
   m_stateC += c; 
   m_stateD += d; 
}













void Checksum::AddData(void * val, const sint32 byte_count) 

{ 
   sint32 i;
   uint8 *p2; 

   p2 = (uint8 *) val; 
   m_total += byte_count; 

   for (i=0; i<byte_count; i++) {
      *m_p1 = *p2; 
      m_p1++; 
      p2++;
      if (m_p1 == m_end_ptr) { 
         Hash(); 
         m_p1 = (uint8 *)m_buffer;
      }
   }
}











void Checksum::Done(uint32 &a, uint32 &b, uint32 &c, uint32 &d)

{ 
   uint32 i;

   if (m_p1 != m_end_ptr) { 
      *m_p1 = (uint8) 0x80; 
      m_p1++; 

      while ((m_p1 != m_end_ptr) && (m_p1 != m_pad_end_ptr)){ 
         *m_p1 = 0; 
         m_p1++; 
      } 

      if (m_p1 == m_pad_end_ptr) { 
         m_buffer[15] = m_total;
      } else { 
         Hash(); 

         for (i=0; i<15; i++) { 
            m_buffer[i] = 0; 
         } 
         m_buffer[15] = m_total;
      }
   } 
   m_p1 = m_end_ptr; 
   Hash(); 

   a = m_stateA; 
   b = m_stateB; 
   c = m_stateC; 
   d = m_stateD;
}
