#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dos.h>
#include <conio.h>

// ��ȯ�Լ� ����.
void Convert16to2(char *Hex_array);
int Convert2to10(char *binary);
// �����Լ� ����
void icache();
void dcache();
void define();
void fifo();


FILE *fp;
char *Hex_array; // ���ϼ� �о�� 16������ ���ڿ��� �б� ���� ������ ����

				 ////////////////////////////////////////////////////////////////
int operand; // (0�϶� read data, 1 write data)  <- data cache , 2 instruction fetch <- instruction cache  
int iBlockoffset = 0, iIndexsize, iIndexaddress = 0, Point, dBlockoffset = 0, dIndexsize, dIndexaddress = 0;
int n, m, ib, db, ia = 1, da = 1, mode = 2;	//	���� ��

int iaccess = 0, imiss = 0, daccess = 0, dmiss = 0;	// ��� ��
double imissratio, dmissratio;

char binary[33];//16 -> 2������ ��ȯ ���� ����
							///////////////////////////////////////////////////////////////


							//	cache�� ����Ǵ� �ӽù迭 ����ü
typedef struct cache {
	char **tag;
	char valid;
} cache;
cache *iarr, *darr;

int main()
{
	char str_file[20] = { 0 };
	int i, j;

	// ���� �Է� �ޱ�

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

	// ���� ���� //
	define();
	// �迭 ũ�� ����
	iarr = (cache*)malloc(sizeof(cache)*iIndexsize);	//	0 ~ indexsize-1
	for (i = 0; i < iIndexsize; i++)
	{
		iarr[i].tag = (char**)malloc(sizeof(char*)*ia); // ���� ������ŭ ���� 0~ia-1
		for (j = 0; j < ia; j++)
			iarr[i].tag[j] = (char*)malloc(sizeof(char)*(33 - iIndexaddress - iBlockoffset));
		iarr[i].valid = 'X';
	}	// iarr[i] �ʱ�ȭ
	darr = (cache*)malloc(sizeof(cache)*dIndexsize);	//	0 ~ indexsize-1		
	for (i = 0; i < dIndexsize; i++)
	{
		darr[i].tag = (char**)malloc(sizeof(char*)*da);
		for (j = 0; j < da; j++)
			darr[i].tag[j] = (char*)malloc(sizeof(char)*(33 - dIndexaddress - dBlockoffset));
		darr[i].valid = 'X';
	}	// darr[i] �ʱ�ȭ


		/*  Ʈ���̽� ���� �б�  */
		//	 ������ ���
	if ((fp = fopen(str_file, "r")) == NULL) {
		printf("file open error");
		exit(1);
	}

	Hex_array = (char *)malloc(sizeof(char) * 11);

	// �۾�
	while (feof(fp) == 0)		//���� ������ ���ڿ� �ҷ�����.
	{
		fscanf(fp, "%d", &operand);
		fscanf(fp, "%s", Hex_array); //��ɾ ���ڿ��� ����

		Convert16to2(Hex_array); //16���� ���ڿ��� 2���� ������ ���ڿ��� ����, binary[] ����

		if (operand == 1 || operand == 0)
		{
			Point = Convert2to10(binary, dBlockoffset, dIndexaddress);	//	2������ 10�������� cache�� ��ġ ����
			dcache(Hex_array);
		}
		if (operand == 2)
		{
			Point = Convert2to10(binary, iBlockoffset, iIndexaddress);	//	2������ 10�������� cache�� ��ġ ����
			icache(Hex_array);
		}
	}

	// ���
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


	// free() ��ȯ
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

// �����Լ�
void define()
{
	int tmp;

	iIndexsize = n * 1024 / (ib*ia);	// index size ����
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
	}		//	index �ּ� ũ�� ����

	dIndexsize = m * 1024 / (db*da);	// index size ����
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
	}		//	index �ּ� ũ�� ����
}

void icache(char *Hex_array)
{
	int i, j, flag, flagt = 3;

	/*	valid bit Ȯ��	*/
	flag = 1;
	if (iarr[Point].valid == 'X') flag = 0;
	/*					*/

	// ĳ�ÿ� �ε����� ���� --> miss
	if (flag == 0) {
		for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
			iarr[Point].tag[0][j] = binary[j];	// ù ������ġ�� tag ����		
		}
		iarr[Point].tag[0][j] = '\0';	// ������ �ι��� �Է� --> string �Լ� ���� ���ϰ�
		iarr[Point].valid = 'Y';	// valid bit -> Y
		imiss++;
	}
	// ĳ���� �ε����� ���� ��� --> tag üũ
	if (flag == 1) {
		// ���� ���� tag ��		
		for (i = 0; i < ia; i++) // ù����(0) ���� �ι�°(1) ����
		{
			flagt = 1;
			for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
				if (iarr[Point].tag[i][j] != binary[j])	flagt = 0;
			}
			if (flagt == 1) break;
		}
		//	flagt=1 ==> �±װ��� �ִ�. 
		//	flagt=0 ==> �±װ��� ����.

		if (flagt == 0) {
			// ���� ���
			// ù��° ��ġ(0)�� tag ����
			for (j = 0; j < 32 - iBlockoffset - iIndexaddress; j++) {
				iarr[Point].tag[0][j] = binary[j];
			}
			iarr[Point].tag[0][j] = '\0';
			imiss++;
		}
	}
	iaccess++;
}
// dcache �Լ�
void dcache(char *Hex_array)
{
	int i, j, flag, flagt = 3;

	/*	valid bit Ȯ��	*/
	flag = 1;
	if (darr[Point].valid == 'X') flag = 0;
	/*					*/

	// valid bit No --> miss
	if (flag == 0) {
		for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
			darr[Point].tag[0][j] = binary[j];	// ù ������ġ�� tag ����		
		}
		darr[Point].tag[0][j] = '\0';	// ������ �ι��� �Է� --> string �Լ� ���� ���ϰ�
		darr[Point].valid = 'Y';	// valid bit -> Y
		dmiss++;
	}

	// valid bit Yes --> tag üũ
	if (flag == 1) {
		// ���� ���� tag ��		
		for (i = 0; i < ia; i++) // ù����(0) ���� �ι�°(1) ����
		{
			flagt = 1;
			for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
				if (darr[Point].tag[i][j] != binary[j])	flagt = 0;
			}
			if (flagt == 1) break;
		}
		//	flagt=1 ==> �±װ��� �ִ�. i��° �濡
		//	flagt=0 ==> �±װ��� ����.

		if (flagt == 0) {
			// ���� ���
			// ù��° ��ġ(0)�� tag ����
			for (j = 0; j < 32 - dBlockoffset - dIndexaddress; j++) {
				darr[Point].tag[0][j] = binary[j];
			}
			darr[Point].tag[0][j] = '\0';
			dmiss++;
		}
	}
	daccess++;
}
// 2���� 10���� ��ȯ�Լ�(������)
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
//16���� 2���� ��ȯ�Լ�.
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

