#define _CRT_SECURE_NO_WARNINGS
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <Math.h>
#define TABLE_SIZE 100

typedef struct Complex {
	float real, imag;
}Complex;

typedef struct {
	char* key;
	Complex number;
	struct Node* next;
} Node;

typedef struct {
	unsigned int size; // ������� �������� ��������
	Node** entries; //�������� �� ����� ���������
} HashTable;

char* ReadString(); // ������ ������
int ReadInt(int start, int end, char* text); // ������ ���� �����
float ReadFloat(char* text); // ������ ����� � ����. ������
Node* ht_create_node(HashTable* hashtable, const char* key, const float real, float imag);
HashTable* ht_create(void); // ��������� ������
void ht_set(HashTable* hashtable, const char* key, const float real, const float imag);
Node* ht_get(HashTable* hashtable, const char* key); // ��������� �������� � ������� �� ������
int ht_del(HashTable* hashtable, const char* key); // ��������� ��������
void ht_free(HashTable* hashtable); // ��������� �������
void ht_output(HashTable* hashtable); // ������� � �������
void ht_output_file(HashTable* hashtable); // ������� � ����
char* randomString(int len);
void show_menu(HashTable* head);

int main() {

	HashTable* ht = ht_create(); // ��������� ������
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	show_menu(ht);

	return 0;
}
// ��������������� �������
unsigned int hash(const char* key, int ht_size) {
	/* This is the djb2 string hash function */

	unsigned int result = 5381;
	unsigned char* p;

	p = (unsigned char*)key;

	while (*p != '\0') {
		result = (result << 5) + result + *p;
		++p;
	}

	return result % ht_size;
}

Node* ht_create_node(HashTable* hashtable, const char* key, const float real, float imag) {
	// �������� ������
	Node* entry = malloc(sizeof(Node) * 1);
	entry->key = malloc(strlen(key) + 1);

	strcpy(entry->key, key);
	entry->number.real = real;
	entry->number.imag = imag;
	// �������� �� ��������� NULL
	entry->next = NULL;
	hashtable->size++;
	return entry;
}

HashTable* ht_create(void) {
	// �������� ������ �� �������
	HashTable* hashtable = malloc(sizeof(HashTable) * 1);
	hashtable->entries = malloc(sizeof(Node*) * TABLE_SIZE);
	hashtable->size = 0;
	// ����������� null �� �������������
	for (int i = 0; i < TABLE_SIZE; ++i) {
		hashtable->entries[i] = NULL;
	}

	return hashtable;
}
int ht_check_empty(HashTable* hashtable) {
	int check_var = 0;
	for (int i = 0; i < TABLE_SIZE; ++i) {
		Node* entry = hashtable->entries[i];

		if (entry == NULL) {
			continue;
		}

		check_var++;
		for (;;) {
			check_var++;
			if (entry->next == NULL) {
				break;
			}

			entry = entry->next;
		}
	}
	if (check_var == 0) {
		return 0;
	}
	return 1;
}
void ht_set(HashTable* hashtable, const char* key, const float real, const float imag) {
	unsigned int slot = hash(key, TABLE_SIZE);

	// �������� ��������
	Node* entry = hashtable->entries[slot];

	// ���� ������
	if (entry == NULL) {
		hashtable->entries[slot] = ht_create_node(hashtable, key, real, imag);
		return;
	}

	Node* prev = NULL;

	// ����������� �� ������
	while (entry != NULL) {
		// ���������� ����
		if (strcmp(entry->key, key) == 0) {
			// ��������, �������� ��������
			entry->number.real = real;
			entry->number.imag = imag;
			return;
		}

		// ����� �� ����������
		prev = entry;
		entry = prev->next;
	}

	// ����� �������� ����������, ������ �����
	prev->next = ht_create_node(hashtable, key, real, imag);
}

Node* ht_get(HashTable* hashtable, const char* key) {
	unsigned int slot = hash(key, TABLE_SIZE);

	// ������� ������ �������� ����
	Node* entry = hashtable->entries[slot];

	// ���� ����� 
	if (entry == NULL) {
		return NULL;
	}

	while (entry != NULL) {
		// ��������� �������� ���� ��������
		if (strcmp(entry->key, key) == 0) {
			return entry;
		}

		// ���������� �� ���������� ���� �������
		entry = entry->next;
	}

	// �� ��������
	return NULL;
}

int ht_del(HashTable* hashtable, const char* key) {
	unsigned int bucket = hash(key, TABLE_SIZE);

	// ������� ������ ��������
	Node* entry = hashtable->entries[bucket];

	if (entry == NULL) {
		return 0;
	}

	Node* prev = NULL;
	int idx = 0;

	// ����� ����� ������ ���� �� �������� ��� ���� �� ����������
	while (entry != NULL) {
		// ���������� ����
		if (strcmp(entry->key, key) == 0) {
			// ������ ������� � ���� ����������
			if (entry->next == NULL && idx == 0) {
				hashtable->entries[bucket] = NULL;
			}

			// ������ ������� � � ���������
			if (entry->next != NULL && idx == 0) {
				hashtable->entries[bucket] = entry->next;
			}

			// �������
			if (entry->next == NULL && idx != 0) {
				prev->next = NULL;
			}

			// ������� �������
			if (entry->next != NULL && idx != 0) {
				prev->next = entry->next;
			}

			// ��������� ������
			free(entry->key);
			free(entry);

			hashtable->size--;
			return 1;
		}

		// ����� �� ����������
		prev = entry;
		entry = prev->next;

		++idx;
	}
}
void ht_free(HashTable* hashtable)
{
	Node* tmp;
	unsigned int i;

	if (hashtable == NULL) {
		return;
	}

	for (i = 0; i < TABLE_SIZE; ++i) {
		if (hashtable->entries[i] != NULL) {
			/* ��������� �� ������ � ��������� ������. */
			while (hashtable->entries[i] != NULL) {
				tmp = hashtable->entries[i]->next;
				free(hashtable->entries[i]->key);
				free(hashtable->entries[i]);
				hashtable->entries[i] = tmp;
			}
			free(hashtable->entries[i]);
		}
	}
	free(hashtable->entries);
	free(hashtable);
}
void ht_output(HashTable* hashtable) {
	for (int i = 0; i < TABLE_SIZE; ++i) {
		Node* entry = hashtable->entries[i];

		if (entry == NULL) {
			continue;
		}

		printf("slot[%d]: ", i);

		for (;;) {
			printf("%.2f + %.2f * i - %s | ", entry->number.real, entry->number.imag, entry->key);

			if (entry->next == NULL) {
				break;
			}

			entry = entry->next;
		}

		printf("\n");
	}
}
void ht_output_file(HashTable* hashtable) {
	FILE* output = fopen("output.txt", "w+");
	if (!output) {
		printf("�� ������� ������� ����!\n");
		return;
	}
	for (int i = 0; i < TABLE_SIZE; ++i) {
		Node* entry = hashtable->entries[i];

		if (entry == NULL) {
			continue;
		}

		fprintf(output, "slot[%d]: ", i);

		for (;;) {
			fprintf(output, "%.2f + %.2f * i - %s | ", entry->number.real, entry->number.imag, entry->key);

			if (entry->next == NULL) {
				break;
			}

			entry = entry->next;
		}

		fprintf(output, "\n");
	}
	printf("��������� �������� � ����!!!\n");
	fclose(output);
}
char* ReadString()
{
	char Simvol;
	char* Local_Name = (char*)malloc(sizeof(char));
	int len = 0;
	printf("������ ���� ��������: ");
	do {

		scanf_s("%c", &Simvol);
		if (len == 30 || Simvol == '\0' || Simvol == '\n')
		{
			break;
		}
		if (Simvol != '\n' && Simvol != '\0')
		{
			Local_Name[len] = Simvol;
			len++;
			Local_Name = (char*)realloc(Local_Name, (1 + len) * sizeof(char));
		}
	} while (Simvol != '\n' || len < 1);
	Local_Name[len] = '\0';
	return Local_Name;
}
char* randomString(int len) {
	const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
	char* rstr = malloc((len + 1) * sizeof(char));
	int i;
	for (i = 0; i < len; i++) {
		rstr[i] = alphabet[rand() % strlen(alphabet)];
	}
	rstr[len] = '\0';
	return rstr;
}
int ReadInt(int start, int end, char* text) {
	int check_input, number;
	do {
		printf("%s", text);
		check_input = scanf("%d", &number);
		while (getchar() != '\n');
	} while (!check_input || number > end || number < start);
	return number;
}
float ReadFloat(char* text) {
	int check_input;
	float number;
	do {
		printf("%s", text);
		check_input = scanf("%f", &number);
		while (getchar() != '\n');
	} while (!check_input);
	return number;
}
void show_menu(HashTable* head) {
	int check_input, way, input_size;
	float real, imag;
	char* keyword;

	printf("\n0) �����\n");
	printf("1) ������ �������\n");
	printf("2) ��������� �������\n");
	printf("3) �������� ������� � �������\n");
	printf("4) �������� ������� �������� � �������\n");
	printf("5) �������� �������\n");
	printf("6) ����� � �������\n");
	way = ReadInt(0, 6, "����: ");
	switch (way)
	{
	case 0:
		way = ReadInt(0, 1, "�� ������� �� ������ �����?\n(1 - ���, 0 - ͳ): ");
		if (way == 1) {
			ht_free(head);
			return -1;
		}
		else {
			show_menu(head);
		}
		break;
	case 1:
		input_size = ReadInt(0, 1000000, "������ �������� �� ������ ������: ");
		way = ReadInt(1, 2, "1 - ��� ���������� ����������� ����������\n2 - ��� ����� ������������\n");
		if (way == 1) {
			for (int i = 0; i < input_size; ++i) {
				char* keyword = randomString(1 + rand()%10);
				ht_set(head, keyword, rand() % 100, rand() % 100);
			}
		}
		else {
			for (int i = 0; i < input_size; ++i) {
				real = ReadFloat("�������: ");
				imag = ReadFloat("�����: ");
				keyword = ReadString();
				ht_set(head, keyword, real, imag);
				printf("--------------------------\n");
			}
		}
		show_menu(head);
		break;
	case 2:
		if (!ht_check_empty(head)) {
			printf("������� �����!!!������� ��������!\n");
			show_menu(head);
		}
		way = ReadInt(0, 1, "������� �: \n (���� - 1, ������� - 0): ");
		if (way == 1) {
			ht_output_file(head);
		}
		else {
			ht_output(head);
		}
		show_menu(head);
		break;
	case 3:
		if (!ht_check_empty(head)) {
			printf("������� �����!!!������� ��������!\n");
			show_menu(head);
		}
		keyword = ReadString();
		check_input = ht_del(head, keyword);
		if (check_input) {
			printf("������� ��������!\n");
		}
		else {
			printf("�� ������� ��������!\n");
		}
		show_menu(head);
		break;
	case 4:
		if (!ht_check_empty(head)) {
			printf("������� �����!!!������� ��������!\n");
			show_menu(head);
		}
		printf("ʳ������ �������� ��������: %d", head->size);
		show_menu(head);
		break;
	case 5:
		ht_free(head);
		printf("������� ��������!\n");
		way = ReadInt(0, 1, "�� ������ ���������� ������? - (1 - ���, 0 - �): ");
		if (way) {
			head = ht_create();
		}
		else {
			exit(0);
		}
		show_menu(head);
		break;
	case 6:
		keyword = ReadString();
		Node* result = ht_get(head, keyword);
		if (result == NULL) {
			printf("******************************\n");
			printf("����� �� ��� ����������!!!\n");
			printf("******************************\n");
			show_menu(head);
		}
		else {
			printf("******************************\n");
			printf("%.2f + (%.2f * i) - ����: %s\n", result->number.real, result->number.imag, result->key);
			printf("******************************\n");
			show_menu(head);
		}
		free(result);
		show_menu(head);
	default:
		break;
	}
	return 0;
}
