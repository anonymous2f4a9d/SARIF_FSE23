/*-------------------------------------------------------------------------
 SASEBO-W Smart Card OS  AES argorithm header
 
 File name   : aes.h
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

#ifndef AES_H_INCLUDE
#define AES_H_INCLUDE

#define AES_128BIT_ENABLE
#define AES_ENCRYPT_ENABLE
#define AES_DECRYPT_ENABLE

typedef unsigned char uint8_t;

/* Define when using program-memory s-box */
//#define PGM_SBOX      (1)
//#define SHARED_MEMORY (1)
//
void aes_init(void);
void aes_setkey_128(const uint8_t *key);

void aes_encrypt_128(uint8_t *c, const uint8_t *m);
void aes_encrypt_128_hardware_like(uint8_t *c, const uint8_t *m);


void aes_decrypt_128(uint8_t *m, const uint8_t *c);
void aes_decrypt_128_hardware_like(uint8_t *m, const uint8_t *c);


#endif /* ?!AES_H_INCLUDE */
