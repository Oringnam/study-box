#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dos.h>
#include <conio.h>

// 변환함수 정의.
void Convert16to2(char *Hex_array);
int Convert2to10(char *binary);
// 실행함수 정의
void icache();
void dcache();
void define();
void fifo();


FILE *fp;
char *Hex_array; // 파일서 읽어온 16진수를 문자열로 읽기 위한 포인터 변수

				 ////////////////////////////////////////////////////////////////
int operand; // (0일때 read data, 1 write data)  <- data cache , 2 instruction fetch <- instruction cache  
int iBlockoffset = 0, iIndexsize, iIndexaddress = 0, Point, dBlockoffset = 0, dIndexsize, dIndexaddress = 0;
int n, m, ib, db, ia = 1, da = 1, mode = 2;	//	설정 값

int iaccess = 0, imiss = 0, daccess = 0, dmiss = 0;	// 출력 값
double imissratio, dmissratio;

char binary[33];//16 -> 2진수로 변환 저장 공간
							///////////////////////////////////////////////////////////////


							//	cache에 저장되는 임시배열 구조체
typedef struct cache {
	char **tag;
	char valid;
} cache;
cache *iarr, *darr;

int main()
{
	char str_file[20] = { 0 };
	int i, j;

	// 설정 입력 받기

	printf("File : ");
	scanf("%s", str_file);
	printf("Instruction cache size (KB) : ");
	scanf("%d", &n);
	printf("Instruction cache block size (B) : ");
	scanf("%d", &ib);
	printf("Data cache size (KB) : ");
	scanf("%d", &m);
	printf("Data cache block size (B) : ");
	scanf("%d", &db);
	printf("\n");

	// 각종 정의 //
	define();
	// 배열 크기 정의
	iarr = (cache*)malloc(sizeof(cache)*iIndexsize);	//	0 ~ indexsize-1
	for (i = 0; i < iIndexsize; i++)
	{
		iarr[i].tag = (char**)malloc(sizeof(char*)*ia); // 방의 갯수만큼 생성 0~ia-1
		for (j = 0; j < ia; j++)
			iarr[i].tag[j] = (char*)malloc(sizeof(char)*(33 - iIndexaddress - iBlockoffset));
		iarr[i].valid = 'X';
	}	// iarr[i] 초기화
	darr = (cache*)malloc(sizeof(cache)*dIndexsize);	//	0 ~ indexsize-1		
	for (i = 0; i < dIndexsize; i++)
	{
		darr[i].tag = (char**)malloc(sizeof(char*)*da);
		for (j = 0; j < da; j++)
			darr[i].tag[j] = (char*)malloc(sizeof(char)*(33 - dIndexaddress - dBlockoffset));
		darr[i].valid = 'X';
	}	// darr[i] 초기화


		/*  트레이스 파일 읽기  */
		//	 못읽은 경우
	if ((fp = fopen(str_file, "r")) == NULL) {
		printf("file open error");
		exit(1);
	}

	Hex_array = (char *)malloc(sizeof(char) * 11);

	// 작업
	while (feof(fp) == 0)		//파일 끝까지 문자열 불러오기.
	{
		fscanf(fp, "%d", &operand);
		fscanf(fp, "%s", Hex_array); //명령어를 문자열로 읽음

		Convert16to2(Hex_array); //16진수 문자열을 2진수 형식의 문자열로 변경, binary[] 생성

		if (operand == 1 || operand == 0)
		{
			Point = Convert2to10(binary, dBlockoffset, dIndexaddress);	//	2진법을 10진법으로 cache의 위치 지정
			dcache(Hex_array);
		}
		if (operand == 2)
		{
			Point = Convert2to10(binary, iBlockoffset, iIndexaddress);	//	2진법을 10진법으로 cache의 위치 지정
			icache(Hex_array);
		}
	}

	// 출력
	printf("Instruction cache\n");
	imissratio = (double)imiss / iaccess;
	printf("Access : %d\n", iaccess);
	printf("Miss: %d\n", imiss);
	printf("Miss ratio : %lf\n", imissratio);
	printf("\nData cache\n");
	dmissratio = (double)dmiss / daccess;
	printf("Access : %d\n", daccess);
	printf("Miss: %d\n", dmiss);
	printf("Miss ratio : %lf\n", dmissratio);


	// free() 반환
	for (i = 0; i < iIndexsize; i++)
	{
		for (j = 0; j < ia; j++)
		{
			free(iarr[i].tag[j]);
		}
		free(iarr[i].tag);
	}
	free(iarr);
	for (i = 0; i < dIndexsize; i++)
	{
		for (j = 0; j < da; j++)
		{
			free(darr[i].tag[j]);
		}
		free(darr[i].tag);
	}
	free(darr);
	free(Hex_array);
	return 0;
}

// 정의함수
void define()
{
	int tmp;

	iIndexsize = n * 1024 / (ib*ia);	// index size 정의
	tmp = iIndexsize;
	while (ib != 1)
	{
		ib /= 2;
		iBlockoffset++;
	}
	while (tmp != 1)
	{
		tmp /= 2;
		iIndexaddress++;
	}		//	index 주소 크기 정의

	dIndexsize = m * 1024 / (db*da);	// index size 정의
	tmp = dIndexsize;
	while (db != 1)
	{
		db /= 2;
		dBlockoffset++;
	}
	while (tmp != 1)
	{
		tmp /= 2;
		dIndexaddress++;
	}		//	index 주소 크기 정의
}

void icache(char *Hex_array)
{
	int i, j, flag, flagt = 3;

	/*	valid bit 확인	*/
	flag = 1;
	if (iarr[Point].valid == 'X') flag = 0;
	/*					*/

	// 캐시에 인덱스가 없다 --> miss
	if (flag == 0) {
		for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
			iarr[Point].tag[0][j] = binary[j];	// 첫 연관위치에 tag 저장		
		}
		iarr[Point].tag[0][j] = '\0';	// 마지막 널문자 입력 --> string 함수 쓰기 편하게
		iarr[Point].valid = 'Y';	// valid bit -> Y
		imiss++;
	}
	// 캐쉬에 인덱스가 있을 경우 --> tag 체크
	if (flag == 1) {
		// 기존 값들 tag 비교		
		for (i = 0; i < ia; i++) // 첫번재(0) 빼고 두번째(1) 부터
		{
			flagt = 1;
			for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
				if (iarr[Point].tag[i][j] != binary[j])	flagt = 0;
			}
			if (flagt == 1) break;
		}
		//	flagt=1 ==> 태그값이 있다. 
		//	flagt=0 ==> 태그값이 없다.

		if (flagt == 0) {
			// 저장 방식
			// 첫번째 위치(0)에 tag 저장
			for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
				iarr[Point].tag[0][j] = binary[j];
			}
			iarr[Point].tag[0][j] = '\0';
			imiss++;
		}
	}
	iaccess++;
}
// dcache 함수
void dcache(char *Hex_array)
{
	int i, j, flag, flagt = 3;

	/*	valid bit 확인	*/
	flag = 1;
	if (darr[Point].valid == 'X') flag = 0;
	/*					*/

	// valid bit No --> miss
	if (flag == 0) {
		for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
			darr[Point].tag[0][j] = binary[j];	// 첫 연관위치에 tag 저장		
		}
		darr[Point].tag[0][j] = '\0';	// 마지막 널문자 입력 --> string 함수 쓰기 편하게
		darr[Point].valid = 'Y';	// valid bit -> Y
		dmiss++;
	}

	// valid bit Yes --> tag 체크
	if (flag == 1) {
		// 기존 값들 tag 비교		
		for (i = 0; i < ia; i++) // 첫번재(0) 빼고 두번째(1) 부터
		{
			flagt = 1;
			for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
				if (darr[Point].tag[i][j] != binary[j])	flagt = 0;
			}
			if (flagt == 1) break;
		}
		//	flagt=1 ==> 태그값이 있다. i번째 방에
		//	flagt=0 ==> 태그값이 없다.

		if (flagt == 0) {
			// 저장 방식
			// 첫번째 위치(0)에 tag 저장
			for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
				darr[Point].tag[0][j] = binary[j];
			}
			darr[Point].tag[0][j] = '\0';
			dmiss++;
		}
	}
	daccess++;
}
// 2진수 10진수 변환함수(정수로)
int Convert2to10(char *binary, int Blockoffset, int Indexaddress)
{
	int i, ret = 0, k = 1;

	for (i = 32 - Blockoffset - 1; i >= 32 - Blockoffset - Indexaddress; i--)
	{
		if (binary[i] % 2 == 0) ret += 0;
		else ret += k;
		k *= 2;
	}
	return ret;
}
//16진수 2진수 변환함수.
void Convert16to2(char *Hex_array)
{
	int res = 0;
	char temp;
	unsigned long Hex = strtoul(Hex_array, NULL, 16);

	for (int i = 0; i < 32; i++)
	{
		res = Hex & (0x01 << (31 - i));
		if (res == 0)
		{
			temp = '0';
			binary[i] = temp;
		}
		else
		{
			temp = '1';
			binary[i] = temp;
		}
	}
}

