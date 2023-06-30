/*-------------------------------------------------------------------------
 SASEBO-W Smart Card OS  AES argorithm
 
 File name   : aes.c
 Version     : 1.0
 Created     : APR/02/2012
 Last update : MAY/08/2012
 Desgined by : Toshihiro Katashita
 
 
 Copyright (C) 2012 AIST
 
 By using this code, you agree to the following terms and conditions.
 
 This code is copyrighted by AIST ("us").
 
 Permission is hereby granted to copy, reproduce, redistribute or
 otherwise use this code as long as: there is no monetary profit gained
 specifically from the use or reproduction of this code, it is not sold,
 rented, traded or otherwise marketed, and this copyright notice is
 included prominently in any copy made.
 
 We shall not be liable for any damages, including without limitation
 direct, indirect, incidental, special or consequential damages arising
 from the use of this code.
 
 When you publish any results arising from the use of this code, we will
 appreciate it if you can cite our webpage.
 (http://www.aist.go.jp/aist_e/research_results/publications/synthesiology_e/vol3_no1/vol03_01_p86_p95.pdf)
 -------------------------------------------------------------------------*/ 

#include "aes.h" 



#define nop10()	{}


#define nop30() {}

#define nop100() {}

/*
 * Prototypes(函数原型，函数声明)
 */
static void add_round_key(void);
static inline uint8_t mul2(uint8_t);
static void sub_byte(void);
static void shift_rows(void);
static void aes_round(void);
static inline void mix_column(uint8_t col);
static void mix_columns(void);
static void key_expansion(void);

/* inv:反，求逆*/
static inline uint8_t div2(uint8_t);
static void inv_sub_byte(void);
static void inv_shift_rows(void);
static void inv_aes_round(void);
static inline void inv_mix_column(uint8_t col);
static void inv_mix_columns(void);
static void inv_key_expansion(void);


const uint8_t sbox[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};


const uint8_t inv_sbox[256] = {
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d 
};


#define SBOX(X) sbox[(X)]
#define INV_SBOX(X) inv_sbox[(X)]

/*
 * Private Variables
 */
static uint8_t *s;
static uint8_t rkey[16];
static uint8_t rcon;
static uint8_t aeskey[16];
#ifdef AES_DECRYPT_ENABLE
static uint8_t aesinvkey[16];
#endif /* ?AES_DECRYPT_ENABLE */

 
void
/*看s-box存储在哪，程序存储器，还是公用存储器*/
aes_init(void)
{
	uint8_t i;
}

/**
 * Set AES-128bit Key
 */
void aes_setkey_128(const uint8_t *key)
{
	uint8_t i;

#ifdef AES_128BIT_ENABLE
//#warning
//#warning *********************
//#warning * AES_128BIT_ENABLE *
//#warning *********************
//#warning 
	for (i = 0; i < 16; i++) {
		rkey[i] = key[i];
		aeskey[i] = key[i];
	}
#else
	for (i = 0; i <= 8; i++) {
		rkey[i] = i;
		aeskey[i] = i;
	}
	for (i = 9; i < 16; i++) {
		rkey[i] = key[i];
		aeskey[i] = key[i];
	}	
#endif

#ifdef AES_DECRYPT_ENABLE
	rcon = 1;
	for (i = 0; i < 10; i++) {
		key_expansion();
	}
	for (i = 0; i < 16; i++) {
		aesinvkey[i] = rkey[i];
	}
#endif /* ?AES_DECRYPT_ENABLE */
}

/**
 * AES-128bit encrypt
 */

/*软件实现AES加密*/
void aes_encrypt_128(uint8_t *c, const uint8_t *m)
{
	uint8_t i;

	s = c;
	for (i = 0; i < 16; i++) {
		s[i] = m[i];       //明文转换为状态
		rkey[i] = aeskey[i];//第0轮的轮密钥就是原密钥
	}
	rcon = 1;      


	nop100();
	
	
/*前九轮*/
	for (i = 0; i < 9; i++) {
		aes_round();
		
		
		nop30();
		
		mix_columns();		
		key_expansion();
		
		nop100();
		
	}
/*第十轮*/
	aes_round();
	
	
	nop30();

	key_expansion();
	
	
	nop30();

	add_round_key();
	
	nop100();
	

}
/*=========================感觉硬件实现和软件实现的AES加密反了==============================*/
/*硬件实现AES加密*/
void aes_encrypt_128_hardware_like(uint8_t *c, const uint8_t *m)
{
	uint8_t i;

	s = c;
	for (i = 0; i < 16; i++) {
		s[i] = m[i];
		rkey[i] = aeskey[i];
	}
	rcon = 1;



	nop100();
	
	add_round_key();
	
	
	nop100();
	
/*前九轮*/
	for (i = 0; i < 9; i++) {
		sub_byte();
		
		nop30();

		shift_rows();

		nop30();

		mix_columns();
		
		
		nop30();

		key_expansion();
		
		
		nop30();

		add_round_key();
		
	
		nop100();
	}
/*第十轮*/
	sub_byte();
	
	
	nop30();

	shift_rows();
	
	nop30();

	key_expansion();
	
	nop30();

	add_round_key();

	nop100();

}


#ifdef AES_DECRYPT_ENABLE
/**
 * AES-128bit decrypt
 */
void aes_decrypt_128(uint8_t *m, const uint8_t *c)
{
	uint8_t i;

	s = m;
	for (i = 0; i < 16; i++) {
		s[i] = c[i];
		rkey[i] = aesinvkey[i];
	}
	rcon = 0x6c;              


	add_round_key();
	inv_key_expansion();
	for (i = 0; i < 9; i++) {
		inv_aes_round();
		inv_key_expansion();
		inv_mix_columns();
	}
	inv_aes_round();

}

void aes_decrypt_128_hardware_like(uint8_t *m, const uint8_t *c)
{
	uint8_t i;

	s = m;
	for (i = 0; i < 16; i++) {
		s[i] = c[i];
		rkey[i] = aesinvkey[i];
	}
	rcon = 0x6c;


	add_round_key();
	inv_key_expansion();
	inv_shift_rows();
	inv_sub_byte();
	for (i = 0; i < 9; i++) {
		add_round_key();
		inv_key_expansion();
		inv_mix_columns();
		inv_shift_rows();
		inv_sub_byte();
	}
	add_round_key();	

}


#endif /* ?AES_DECRYPT_ENABLE */

/*
 * Private Functions
 */

/*轮密钥加*/
static void add_round_key(void)
{
	uint8_t i;
	for (i = 0; i < 16; i++) {
		s[i] ^= rkey[i];
	}
}

/* //这个mul2函数有问题，结果不对？！，用后面C语言实现的同名函数代替
static inline uint8_t
mul2(uint8_t a)
{
	register unsigned char x asm("r3");
	uint8_t tmp = a & 0x80;

	if (tmp == 0x80) {	// Never use "if (tmp) " to not generate BRBS. 
		__asm__ __volatile__
		(
			"nop\n\t"
			"ldi r26, 0x1b\n\t"
			"mov r3, r26\n\t"
		::);
	} else {
		__asm__ __volatile__
		(
			"nop\n\t"
			"nop\n\t"
			"ldi r26, 0x00\n\t"
			"mov r3, r26\n\t"
		::);
	}
	return (a << 1) ^ x;
}
*/

// GF(28) 多项式
#define BPOLY 0x1B // Lower 8 BOOLs of (x^8 + x^4 + x^3 + x + 1), ie. (x^4 + x^3 + x + 1).

/*在GF(28)域的 乘2 运算*/
static inline uint8_t  mul2(uint8_t a)
{
	if ((a & 0x80) == 0)
	{
		a = a << 1;
	}
	else
	{
		a = (a << 1) ^ BPOLY;
	}
	
	return a;
}

/*字节替换*/
static void sub_byte(void)
{
	uint8_t i;
	for (i = 0; i < 16; i++) {
		s[i] = SBOX(s[i]);
	}	
}

/*位移行变换*/
static void shift_rows(void)
{
	/* { s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7],
	     s[8], s[9], s[a], s[b], s[c], s[d], s[e], s[f] } =
	   { s[0], s[5], s[a], s[f], s[4], s[9], s[e], s[3],
	     s[8], s[d], s[2], s[7], s[c], s[1], s[6], s[b] }; */
	uint8_t tmp;

	tmp = s[1];
	s[1] = s[5];
	s[5] = s[9];
	s[9] = s[13];
	s[13] = tmp;

	tmp = s[2];
	s[2] = s[10];
	s[10] = tmp;
	tmp = s[6];
	s[6] = s[14];
	s[14] = tmp;

	tmp = s[3];
	s[3] = s[15];
	s[15] = s[11];
	s[11] = s[7];
	s[7] = tmp;
}
/*硬件实现：轮变换，包括轮密钥加、S盒查找、位移行变换
硬件实现的S盒查找是4个字节并行实现的，而软件实现的S盒查找的16个字节是顺序执行的
*/
static void aes_round(void)
{
	uint8_t tmp;

	s[0]  = SBOX(s[0]  ^ rkey[0] );
	s[4]  = SBOX(s[4]  ^ rkey[4] );
	s[8]  = SBOX(s[8]  ^ rkey[8] );
	s[12] = SBOX(s[12] ^ rkey[12]);

	tmp   = SBOX(s[1]  ^ rkey[1] );
	s[1]  = SBOX(s[5]  ^ rkey[5] );
	s[5]  = SBOX(s[9]  ^ rkey[9] );
	s[9]  = SBOX(s[13] ^ rkey[13]);
	s[13] = tmp;

	tmp   = SBOX(s[2]  ^ rkey[2] );
	s[2]  = SBOX(s[10] ^ rkey[10]);
	s[10] = tmp;
	tmp   = SBOX(s[6]  ^ rkey[6] );
	s[6]  = SBOX(s[14] ^ rkey[14]);
	s[14] = tmp;

	tmp   = SBOX(s[3]  ^ rkey[3] );
	s[3]  = SBOX(s[15] ^ rkey[15]);
	s[15] = SBOX(s[11] ^ rkey[11]);
	s[11] = SBOX(s[7]  ^ rkey[7] );
	s[7]  = tmp;
}

/*对状态的一列进行混合列变换*/
static inline void mix_column(uint8_t col)
{
	uint8_t *p = &s[col * 4];//对第0列：指针指向s[0];对第一列，指针指向s[4]
	uint8_t p0, p1, p2, p3, ps;

	p0 = p[0];
	p1 = p[1];
	p2 = p[2];
	p3 = p[3];
	ps = p0 ^ p1 ^ p2 ^ p3;
	p[0] ^= mul2(p0 ^ p1) ^ ps;
	p[1] ^= mul2(p1 ^ p2) ^ ps;
	p[2] ^= mul2(p2 ^ p3) ^ ps;
	p[3] ^= mul2(p3 ^ p0) ^ ps;
}

static void mix_columns(void)
{
	mix_column(0);
	mix_column(1);
	mix_column(2);
	mix_column(3);
}

/*密钥扩展*/
static void key_expansion(void)
{
	rkey[0] ^= SBOX(rkey[13]) ^ rcon;  //rcon为轮常量
	rkey[1] ^= SBOX(rkey[14]);
	rkey[2] ^= SBOX(rkey[15]);
	rkey[3] ^= SBOX(rkey[12]);
	rkey[4] ^= rkey[0];
	rkey[5] ^= rkey[1];
	rkey[6] ^= rkey[2];
	rkey[7] ^= rkey[3];
	rkey[8] ^= rkey[4];
	rkey[9] ^= rkey[5];
	rkey[10] ^= rkey[6];
	rkey[11] ^= rkey[7];
	rkey[12] ^= rkey[8];
	rkey[13] ^= rkey[9];
	rkey[14] ^= rkey[10];
	rkey[15] ^= rkey[11];
	rcon = mul2(rcon);
}

#ifdef AES_DECRYPT_ENABLE
static inline uint8_t div2(uint8_t a)
{
	return (a >> 1) ^ ((a & 0x01) ? 0x8d: 0);
}

static void
inv_sub_byte(void)
{
	uint8_t i;
	for (i = 0; i < 16; i++) {
		s[i] = INV_SBOX(s[i]);
	}	
}

static void inv_shift_rows(void)
{
	/* { s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7],
	     s[8], s[9], s[a], s[b], s[c], s[d], s[e], s[f] } =
	   { s[0], s[5], s[a], s[f], s[4], s[9], s[e], s[3],
	     s[8], s[d], s[2], s[7], s[c], s[1], s[6], s[b] }; */
	uint8_t tmp;

	tmp = s[13];
	s[13] = s[9];
	s[9] = s[5];
	s[5] = s[1];
	s[1] = tmp;

	tmp = s[2];
	s[2] = s[10];
	s[10] = tmp;
	tmp = s[6];
	s[6] = s[14];
	s[14] = tmp;

	tmp = s[7];
	s[7] = s[11];
	s[11] = s[15];
	s[15] = s[3];
	s[3] = tmp;
}

static void inv_aes_round(void)
{
	uint8_t tmp;

	s[0]  = INV_SBOX(s[0])  ^ rkey[0];
	s[4]  = INV_SBOX(s[4])  ^ rkey[4];
	s[8]  = INV_SBOX(s[8])  ^ rkey[8];
	s[12] = INV_SBOX(s[12]) ^ rkey[12];

	tmp   = s[13];
	s[13] = INV_SBOX(s[9])  ^ rkey[13];
	s[9]  = INV_SBOX(s[5])  ^ rkey[9];
	s[5]  = INV_SBOX(s[1])  ^ rkey[5];
	s[1]  = INV_SBOX(tmp)   ^ rkey[1];

	tmp   = s[2];
	s[2]  = INV_SBOX(s[10]) ^ rkey[2];
	s[10] = INV_SBOX(tmp)   ^ rkey[10];
	tmp   = s[6];
	s[6]  = INV_SBOX(s[14]) ^ rkey[6];
	s[14] = INV_SBOX(tmp)   ^ rkey[14];

	tmp   = s[7];
	s[7]  = INV_SBOX(s[11]) ^ rkey[7];
	s[11] = INV_SBOX(s[15]) ^ rkey[11];
	s[15] = INV_SBOX(s[3])  ^ rkey[15];
	s[3]  = INV_SBOX(tmp)   ^ rkey[3];
}

static inline void inv_mix_column(uint8_t col)
{
	uint8_t *p = &s[col * 4];
	uint8_t p0, p1, p2, p3, ps, ps2, a, b;

	p0 = p[0];
	p1 = p[1];
	p2 = p[2];
	p3 = p[3];
	ps = p0 ^ p1 ^ p2 ^ p3;
	ps2 = mul2(ps);
	a = mul2(mul2(ps2 ^ p0 ^ p2)) ^ ps;
	b = mul2(mul2(ps2 ^ p1 ^ p3)) ^ ps;
	p[0] ^= mul2(p0 ^ p1) ^ a;
	p[1] ^= mul2(p1 ^ p2) ^ b;
	p[2] ^= mul2(p2 ^ p3) ^ a;
	p[3] ^= mul2(p3 ^ p0) ^ b;
}

static void inv_mix_columns(void)
{
	inv_mix_column(0);
	inv_mix_column(1);
	inv_mix_column(2);
	inv_mix_column(3);
}

static void inv_key_expansion(void)
{
	rcon = div2(rcon);
	rkey[15] ^= rkey[11];
	rkey[14] ^= rkey[10];
	rkey[13] ^= rkey[9];
	rkey[12] ^= rkey[8];
	rkey[11] ^= rkey[7];
	rkey[10] ^= rkey[6];
	rkey[9] ^= rkey[5];
	rkey[8] ^= rkey[4];
	rkey[7] ^= rkey[3];
	rkey[6] ^= rkey[2];
	rkey[5] ^= rkey[1];
	rkey[4] ^= rkey[0];
	rkey[3] ^= sbox[rkey[12]];
	rkey[2] ^= sbox[rkey[15]];
	rkey[1] ^= sbox[rkey[14]];
	rkey[0] ^= sbox[rkey[13]] ^ rcon;
}

#endif /* ?AES_DECRYPT_ENABLE */
