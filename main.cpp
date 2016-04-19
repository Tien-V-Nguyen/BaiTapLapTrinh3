#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <openssl\evp.h>
#include <openssl\sha.h>
#include <openssl\rand.h>

//#define TEST 0

unsigned char* readFile(char* filePath, long &buffLen);
unsigned char* dec2Hex_BigEndian(int chr);
unsigned char* string2Hex_BigEndian(unsigned char* str);
unsigned char* padHash_1024(unsigned char*, unsigned char*);
void hashFile_SHA256(char* filePath);

int main() {

	hashFile_SHA256("clip_cousera.mp4");

	system("pause");
	return 0;
}

unsigned char* readFile(char* filePath, long &buffLen) {
	FILE *file;
	unsigned char* buff;
	void *temp;
	temp = calloc(17000000, 1);

	if (!(file = fopen(filePath, "rb"))) {
		printf("Khong ton tai file!");
		return NULL;
	}
	else {
		buffLen = fread(temp, 1, 17000000, file);
		buff = (unsigned char*)temp;
	}
#ifdef TEST
	printf("%d --\n", buffLen);
#endif

	return buff;
}

unsigned char* dec2Hex_BigEndian(int chr) {
	unsigned char* c;
	c = new unsigned char[2];
	bool b[8];
	
	for (int i = 7; i >= 0; i--) {
		b[i] = chr % 2;
		chr = chr / 2;
	}

	int c1 = b[0] * 8 + b[1] * 4 + b[2] * 2 + b[3] * 1;
	int c2 = b[4] * 8 + b[5] * 4 + b[6] * 2 + b[7] * 1;

	switch (c1) {
		case 0: c[0] = '0'; break;
		case 1: c[0] = '1'; break;
		case 2: c[0] = '2'; break;
		case 3: c[0] = '3'; break;
		case 4: c[0] = '4'; break;
		case 5: c[0] = '5'; break;
		case 6: c[0] = '6'; break;
		case 7: c[0] = '7'; break;
		case 8: c[0] = '8'; break;
		case 9: c[0] = '9'; break;
		case 10: c[0] = 'a'; break;
		case 11: c[0] = 'b'; break;
		case 12: c[0] = 'c'; break;
		case 13: c[0] = 'd'; break;
		case 14: c[0] = 'e'; break;
		case 15: c[0] = 'f'; break;
	}

	switch (c2) {
		case 0: c[1] = '0'; break;
		case 1: c[1] = '1'; break;
		case 2: c[1] = '2'; break;
		case 3: c[1] = '3'; break;
		case 4: c[1] = '4'; break;
		case 5: c[1] = '5'; break;
		case 6: c[1] = '6'; break;
		case 7: c[1] = '7'; break;
		case 8: c[1] = '8'; break;
		case 9: c[1] = '9'; break;
		case 10: c[1] = 'a'; break;
		case 11: c[1] = 'b'; break;
		case 12: c[1] = 'c'; break;
		case 13: c[1] = 'd'; break;
		case 14: c[1] = 'e'; break;
		case 15: c[1] = 'f'; break;
	}
	c[2] = 0;
	return c;

}

unsigned char* string2Hex_BigEndian(unsigned char* str) {
	unsigned char** hex;
	unsigned char* h;

	h = new unsigned char[strlen((char*)str) * 2];

	hex = new unsigned char*[strlen((char*)str)];
	for (int i = 0; i < strlen((char*)str); i++) {
		hex[i] = new unsigned char[2];
		hex[i] = dec2Hex_BigEndian((int)(str[i]));
	}

	int i;
	for (i = 0; i < 2*strlen((char*)str); i++) {
		if (i % 2 == 0) {
			h[i] = hex[i/2][0];
		}
		else {
			h[i] = hex[i/2][1];
		}
#ifdef TEST
		printf("%d - %d\n", i, h[i]);
#endif
	}
	h[i] = 0;

	return h;
}

unsigned char* padHash_1024(unsigned char* source, unsigned char* hash) {
	unsigned char* padHash;
	padHash = new unsigned char[1024+32];

	for (int i = 0; i < 1024; i++) {
		padHash[i] = source[i];
	}
	for (int i = 1024; i < 1024+32; i++) {
		padHash[i] = hash[i - 1024];
	}
	padHash[1024 + 32] = 0;

	return padHash;
}

void hashFile_SHA256(char* filePath) {

	SHA256_CTX hash;

	unsigned char* buff;
	buff = new unsigned char[16928768];
	for (int i = 0; i < 16928768; i++) buff[i] = 0;
	long buffLen;					//Length of File
	buff = readFile(filePath, buffLen);	//buff* tro den vi tri luu noi dung file
	printf("Size of file %s is %d bytes\n", filePath, buffLen);

	unsigned char** buff1;
	buff1 = new unsigned char*[buffLen / 1024 + 1];
	for (int i = 0; i < buffLen / 1024 + 1; i++) {
		buff1[i] = new unsigned char[1024];
		for (int j = 0; j < 1024; j++) {
			if (i * 1024 + j == buffLen) {
				buff1[i][j] = 0;
				break;
			}
			buff1[i][j] = buff[i * 1024 + j];
		}
	}

	unsigned char** hashResult;
	hashResult = new unsigned char*[buffLen / 1024 + 1];
	for (int i = 0; i < buffLen / 1024 + 1; i++) {
		hashResult[i] = new unsigned char[32];
	}

	unsigned char* result;
	result = new unsigned char[32];

	SHA256_Init(&hash);
	SHA256_Update(&hash, buff1[buffLen / 1024], buffLen % 1024);
	SHA256_Final(result, &hash);
	hashResult[buffLen / 1024] = result;
	hashResult[buffLen / 1024][32] = 0;
	buff1[buffLen / 1024 - 1] = padHash_1024(buff1[buffLen / 1024 - 1], hashResult[buffLen / 1024]);

	for (int i = (int)buffLen / 1024 - 1; i >= 0; i--) {
		SHA256_Init(&hash);
		SHA256_Update(&hash, buff1[i], 1024 + 32);
		SHA256_Final(result, &hash);
		hashResult[i] = result;
		//hashResult[i][32] = 0;

		if (i > 0)
			buff1[i - 1] = padHash_1024(buff1[i - 1], hashResult[i]);

	}

	printf("The hash value h0 of %s is: %s \n", filePath, string2Hex_BigEndian(hashResult[0])); //Tai sao h0 chi co 208 bits???
	
	return;

}