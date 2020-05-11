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
	unsigned int size; // кількість існуючих елементів
	Node** entries; //вказівник на масив вказівників
} HashTable;

char* ReadString(); // читаємо строку
int ReadInt(int start, int end, char* text); // читаємо ціле число
float ReadFloat(char* text); // читаємо число з плав. точкою
Node* ht_create_node(HashTable* hashtable, const char* key, const float real, float imag);
HashTable* ht_create(void); // створення дерева
void ht_set(HashTable* hashtable, const char* key, const float real, const float imag);
Node* ht_get(HashTable* hashtable, const char* key); // отримання значення з таблиці за ключем
int ht_del(HashTable* hashtable, const char* key); // видалення елемента
void ht_free(HashTable* hashtable); // видалення таблиці
void ht_output(HashTable* hashtable); // вивести у консоль
void ht_output_file(HashTable* hashtable); // вивести у файл
char* randomString(int len);
void show_menu(HashTable* head);

int main() {

	HashTable* ht = ht_create(); // створюємо дерево
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	show_menu(ht);

	return 0;
}
// мультиплікаційна функція
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
	// виділяємо память
	Node* entry = malloc(sizeof(Node) * 1);
	entry->key = malloc(strlen(key) + 1);

	strcpy(entry->key, key);
	entry->number.real = real;
	entry->number.imag = imag;
	// показник на наступний NULL
	entry->next = NULL;
	hashtable->size++;
	return entry;
}

HashTable* ht_create(void) {
	// виділяємо память під таблицю
	HashTable* hashtable = malloc(sizeof(HashTable) * 1);
	hashtable->entries = malloc(sizeof(Node*) * TABLE_SIZE);
	hashtable->size = 0;
	// встановюємо null за замовчуванням
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

	// спробуємо вставити
	Node* entry = hashtable->entries[slot];

	// слот пустий
	if (entry == NULL) {
		hashtable->entries[slot] = ht_create_node(hashtable, key, real, imag);
		return;
	}

	Node* prev = NULL;

	// проходимось по списку
	while (entry != NULL) {
		// перевіряємо ключ
		if (strcmp(entry->key, key) == 0) {
			// знайдено, заміняємо значення
			entry->number.real = real;
			entry->number.imag = imag;
			return;
		}

		// йдемо до наступного
		prev = entry;
		entry = prev->next;
	}

	// кінець ланцюжка досягнутий, додаємо новий
	prev->next = ht_create_node(hashtable, key, real, imag);
}

Node* ht_get(HashTable* hashtable, const char* key) {
	unsigned int slot = hash(key, TABLE_SIZE);

	// пробуємо знайти відповіний слот
	Node* entry = hashtable->entries[slot];

	// нема слоту 
	if (entry == NULL) {
		return NULL;
	}

	while (entry != NULL) {
		// повернуть значення якщо знайдено
		if (strcmp(entry->key, key) == 0) {
			return entry;
		}

		// переходимо до наступного якщо можливо
		entry = entry->next;
	}

	// не знайдено
	return NULL;
}

int ht_del(HashTable* hashtable, const char* key) {
	unsigned int bucket = hash(key, TABLE_SIZE);

	// пробуємо знайти відповідне
	Node* entry = hashtable->entries[bucket];

	if (entry == NULL) {
		return 0;
	}

	Node* prev = NULL;
	int idx = 0;

	// ідемо через кожний поки не знайдемо або поки не закінчиться
	while (entry != NULL) {
		// перевіряємо ключ
		if (strcmp(entry->key, key) == 0) {
			// перший елемент і немає наступного
			if (entry->next == NULL && idx == 0) {
				hashtable->entries[bucket] = NULL;
			}

			// перший елемент і є наступний
			if (entry->next != NULL && idx == 0) {
				hashtable->entries[bucket] = entry->next;
			}

			// останній
			if (entry->next == NULL && idx != 0) {
				prev->next = NULL;
			}

			// середній елемент
			if (entry->next != NULL && idx != 0) {
				prev->next = entry->next;
			}

			// звільняємо память
			free(entry->key);
			free(entry);

			hashtable->size--;
			return 1;
		}

		// ідемо до наступного
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
			/* Проходимо по дереву і звільнюємо память. */
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
		printf("Не вдалося відкрити файл!\n");
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
	printf("Результат записано в файл!!!\n");
	fclose(output);
}
char* ReadString()
{
	char Simvol;
	char* Local_Name = (char*)malloc(sizeof(char));
	int len = 0;
	printf("Вкажіть ключ елемента: ");
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

	printf("\n0) Вихід\n");
	printf("1) Додати елемент\n");
	printf("2) Проказати таблицю\n");
	printf("3) Видалити елемент з таблиці\n");
	printf("4) Показати кількість елементів в таблиці\n");
	printf("5) Видалити таблицю\n");
	printf("6) Пошук в таблиці\n");
	way = ReadInt(0, 6, "Меню: ");
	switch (way)
	{
	case 0:
		way = ReadInt(0, 1, "Ви впевнені що хочети вийти?\n(1 - Так, 0 - Ні): ");
		if (way == 1) {
			ht_free(head);
			return -1;
		}
		else {
			show_menu(head);
		}
		break;
	case 1:
		input_size = ReadInt(0, 1000000, "Скільки елементів ви хочете додати: ");
		way = ReadInt(1, 2, "1 - для заповнення випадковими значеннями\n2 - для вводу користувачем\n");
		if (way == 1) {
			for (int i = 0; i < input_size; ++i) {
				char* keyword = randomString(1 + rand()%10);
				ht_set(head, keyword, rand() % 100, rand() % 100);
			}
		}
		else {
			for (int i = 0; i < input_size; ++i) {
				real = ReadFloat("Реальна: ");
				imag = ReadFloat("Уявна: ");
				keyword = ReadString();
				ht_set(head, keyword, real, imag);
				printf("--------------------------\n");
			}
		}
		show_menu(head);
		break;
	case 2:
		if (!ht_check_empty(head)) {
			printf("Таблиця пуста!!!Додайте елементи!\n");
			show_menu(head);
		}
		way = ReadInt(0, 1, "Вивести у: \n (файл - 1, консоль - 0): ");
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
			printf("Таблиця пуста!!!Додайте елементи!\n");
			show_menu(head);
		}
		keyword = ReadString();
		check_input = ht_del(head, keyword);
		if (check_input) {
			printf("Елемент видалено!\n");
		}
		else {
			printf("Не вдалося видалити!\n");
		}
		show_menu(head);
		break;
	case 4:
		if (!ht_check_empty(head)) {
			printf("Таблиця пуста!!!Додайте елементи!\n");
			show_menu(head);
		}
		printf("Кількість існуючих елементів: %d", head->size);
		show_menu(head);
		break;
	case 5:
		ht_free(head);
		printf("Таблиця видалена!\n");
		way = ReadInt(0, 1, "Ви хочете продовжити роботу? - (1 - так, 0 - ні): ");
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
			printf("Пошук не дав результату!!!\n");
			printf("******************************\n");
			show_menu(head);
		}
		else {
			printf("******************************\n");
			printf("%.2f + (%.2f * i) - ключ: %s\n", result->number.real, result->number.imag, result->key);
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
