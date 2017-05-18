#include "lib1617.h"

NODO* sentinel = NULL; //Sentinella per le foglie e padre della radice

								//   A	 B	 C	 D	  E	  F	  G	  H	  I	  J  K  L   M   N   O   P   Q  R   S   T   U   V   W   X  Y	  Z	 SPC NULL NL  '
int letter_frequencies[ELEMENTS] = { 81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76, 20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130, 80, 80, 5 };


//PROTOTIPI FUNZIONI AUSILIARIE
int createSentinel();
unsigned short alphabeticalOrder(char *, char *);
void leftRotate(NODO **, NODO *);
void rightRotate(NODO **, NODO *);
void insertFixUp(NODO **, NODO **);
void insertRBT(NODO **, NODO *);

HNode * build_huffman_tree();
HNode * allocates_node(int);
HNode * extract_smaller_one(HNode **);
void fill_table(unsigned int *, HNode *, unsigned int);
int compress_node(NODO *, FILE *, unsigned int *);
void compress_string(char *, FILE *, unsigned int *);

//FUNZIONI
int createSentinel() {
	//Create node
	sentinel = (NODO*)malloc(sizeof(NODO));
	//Check for a safe malloc
	if (sentinel == NULL)
		return -1; //Bad malloc

	//Update record for sentinel
	sentinel->isBlack = true;
	sentinel->parent = NULL;
	sentinel->left = NULL;
	sentinel->right = NULL;
	sentinel->word = NULL;
	sentinel->def = NULL;

	return 0; //Ok
}

/*
 *RETURN 0: n2 is grater than n1
 *RETURN 1: n1 is grater than n2
 *RETURN 2: n1 is n2
*/
unsigned short alphabeticalOrder(char* n1, char* n2) {
	//Check character 
	for (int i = 0; i < MAX_WORD; i++) {
		if (n1[i] < n2[i])
			return 0;
		else if(n1[i] > n2[i])
			return 1;
	}

	return 2;
}

void leftRotate(NODO** root, NODO* x)
{
	NODO* T = *root;
	NODO* y = x->right; //Create new NODO and assign to x.right
	x->right = y->left; //Move left sub-tree (y) on the x sub-tree

	if (y->left != sentinel) y->left->parent = x;

	y->parent = x->parent; //Connect parent of x to y

	if (x->parent == sentinel)
		*root = y;
	else if (x == x->parent->left)
		x->parent->left = y;

	else x->parent->right = y;

	y->left = x; //Move x on the left of y
	x->parent = y;
}

void rightRotate(NODO** root, NODO* y)
{
	NODO* T = *root;
	NODO* x = y->left; //Create new NODO and assign to x.right
	y->left = x->right; //Move left sub-tree (y) on the x sub-tree

	if (x->right != sentinel) x->right->parent = y;

	x->parent = y->parent; //Connect parent of x to y

	if (y->parent == sentinel)
		*root = x;
	else if (y == y->parent->right)
		y->parent->right = x;

	else y->parent->left = x;

	x->right = y; //Move x on the left of y
	y->parent = x;
}

void insertFixUp(NODO** root, NODO** node) {
	NODO* T = *root;
	NODO* z = *node; //Puntatore temporaneo al nodo
	NODO* y = NULL;

	while ( z-> parent != NULL && z->parent->isBlack == false)	{
		if (z->parent == z->parent->parent->left) {
			y = z->parent->parent->left;
			if (y->isBlack == false) {
				z->parent->isBlack = true;
				y->isBlack = true;
				z->parent->parent->isBlack = false;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					z = z->parent;
					leftRotate(root, z);
				}
				z->parent->isBlack = true;
				z->parent->parent->isBlack = false;
				rightRotate(root, z->parent->parent);
			}
		} else {
			y = z->parent->parent->left;
			if (y->isBlack == false) {
				z->parent->isBlack = true;
				y->isBlack = true;
				z->parent->parent->isBlack = false;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rightRotate(root, z);
				}
				z->parent->isBlack = true;
				z->parent->parent->isBlack = false;
				leftRotate(root, z->parent->parent);
			}
		}
	}

	T->isBlack = true;
}

void insertRBT(NODO** root, NODO* node) {
	NODO* y = NULL;
	NODO* x = NULL;
	NODO* T = *root;
	
	y = sentinel;
	x = *root;

	while (x != sentinel && x != NULL) {
		y = x;
		switch (alphabeticalOrder(node->word, x->word))	{
		case 0:
			x = x->left;
			break;
		case 1:
			x = x->right;
			break;
		case 2:
			return;
		default:
			break;
		}
	}
	
	node->parent = y;
	if (y == sentinel)
		*root = node;
	else {
		switch (alphabeticalOrder(node->word, y->word))	{
		case 0:
			y->left = node;
			break;
		case 1:
			y->right = node;
			break;
		case 2:
			return;
		default:
			break;
		}
	}
	node->left = sentinel;
	node->right = sentinel;
	node->isBlack = false;
	insertFixUp(root, &node);
}



HNode * build_huffman_tree() {
	HNode *temp, *nodes_head;
	int i;

	temp = nodes_head = allocates_node(0);				//ALLOCO IL PRIMO NODO DELLA LISTA
	if (temp == NULL)									//CONTROLLO L'ALLOCAZIONE
		return NULL;

	temp->frequencies = letter_frequencies[0];			//FREQUENZA DELLA LETTERA (CONTENUTA NELL'ARRAY)

	for (i = 1; i < ELEMENTS; i++){						//PER OGNI LETTERA DOPO LA PRIMA, ALLOCO UN NODO (CREO UNA LISTA)
		temp->next = allocates_node(i);					//ALLOCO UN NUOVO NODO E LO FACCIO PUNTARE DAL NEXT DEL PRECEDENTE
		temp = temp->next;								//SPOSTO LA VAR TEMP AL NODO APPENA CREATO (O A NULL IN CASO D'ERRORE)
		if (temp == NULL)								//CONTROLLO L'ALLOCAZIONE
			return NULL;
		temp->frequencies = letter_frequencies[i];		//ASSOCIO LA FREQUENZA DELLA LETTERA CONTENUTA NELL'ARRAY ALLA VARIABILE NEL NODO
	}

	while (nodes_head->next != NULL){					//QUANDO HO ALTRI NODI OLTRE LA "RADICE" (SE NON CI SON STATI ERRORI PRECEDENTI CI SARA' SEMPRE ALMENO UN NODO; IN CASO CONTRARIO AVREBBE RESTITUITO NULL)		
		temp = allocates_node(127);						//ALLOCO UN NODO CHE NON CORRISPONDE A NESSUNA LETTERA
		if (temp == NULL)								//CONTROLLO L'ALLOCAZIONE
			return NULL;
		temp->left = extract_smaller_one(&nodes_head);	//ESTRAGGO IL NODO CON LETTERA DI FREQUENZA MINORE DALLA LISTA E LO FACCIO PUNTARE DAL FIGLIO SINISTRO DEL NUOVO NODO
		temp->right = extract_smaller_one(&nodes_head);	//ESTRAGGO IL NODO CON LETTERA DI FREQUENZA MINORE DALLA LISTA E LO FACCIO PUNTARE DAL FIGLIO DESTRO DEL NUOVO NODO
		temp->frequencies = temp->left->frequencies + temp->right->frequencies;	//LA FREQUENZA EQUIVALE ALLA SOMMA DELLE DUE
		temp->next = nodes_head;						//INSERISCO IL NODO NELLA LISTA
		nodes_head = temp;
	}

	return nodes_head;									//RITORNO LA TESTA DELLA LISTA CHE ORA CORRISPONDERA' ALLA RADICE DELL'ALBERO DI HUFFMAN
}

HNode * allocates_node(int i) {
	HNode *node = (HNode *)malloc(sizeof(HNode));		//ALLOCO IL NODO
	if (node == NULL)									//CONTROLLO L'ALLOCAZIONE
		return NULL;

	node->letter = i;									//INDICE DELL'ARRAY CORRISPONDENTE ALLA LETTERA
	node->left = node->right = node->next = NULL;

	return node;										//PUNTATORE AL NODO
}

HNode * extract_smaller_one(HNode **nodes_head) {
	HNode *minimum = *nodes_head;
	HNode *scrolling = *nodes_head, *previous = NULL;

	if (*nodes_head == NULL)							//SE LA LISTA NON CONTIENE ELEMENTI
		return NULL;

	if ((*nodes_head)->next == NULL){					//SE CONTIENE SOLO UN ELEMENTO
		*nodes_head = NULL;								//SVUOTO LA LISTA
		return minimum;									//RITORNO LA RADICE DELL'ALBERO
	}

	while (scrolling->next != NULL){					//SCORRO LA LISTA
		if (scrolling->next->frequencies < minimum->frequencies){	//SE TROVO UN ELEMENTO DI FREQUENZA MINORE
			previous = scrolling;						//TENGO IN MEMORIA IL NODO PRECEDENTE AL MINIMO
			minimum = scrolling->next;					//AGGIORNO IL MINIMO
		}
		scrolling = scrolling->next;					//VADO ALL'ELEMENTO SUCCESSIVO DELLA LISTA
	}
	if (previous == NULL){								//SE L'ELEMENTO ERA IL PRIMO DELLA LISTA
		*nodes_head = minimum->next;					//SPOSTO LA TESTA ALL'ELEMENTO SUCCESSIVO
		minimum->next = NULL;							//SCOLLEGO IL PUNTATORE AL SUCCESSIVO DEL MINIMO
	}
	else {												//SE IL MINIMO NON ERA IL PRIMO DELLA LISTA
		previous->next = minimum->next;					//IL PUNTATORE NEXT DEL NODO PRECEDENTE PUNTA AL SUCCESSIVO DI MINIMO
		minimum->next = NULL;							//SCOLLEGO IL PUNTATORE NEXT DEL MINIMO
	}

	return minimum;										//RESTITUISCO L'INDIRIZZO DEL NODO CON FREQUENZA MINORE
}

void fill_table(unsigned int *code_table, HNode *tree_node, unsigned int code) {
	if (tree_node->letter != 127)									//SE SIAMO ALLA FOGLIA (C'E' UN VALORE != DA 127)
		code_table[(int)tree_node->letter] = code;					//"CODE" HA ASSUNTO IL VALORE "BINARIO" DEL PERCORSO DA RADICE->FOGLIA
	else {															//SE NON SONO ANCORA GIUNTO ALLA FOGLIA
		if (code >= pow(10, 9)) {									//QUANDO CODE SUPERA IL MILIARDO (NON E' PIU' POSSIBILE CONTENERLO IN UN INT)
			fill_table(code_table, tree_node->left, code + 3);		//SE VADO A SX IL RAMO HA VALORE 0(1) - IN QUESTO CASO E' STATO MESSO +3 PER EVITARE CHE L'INT RAGGIUNGESSE I 10 MILIARDI
			fill_table(code_table, tree_node->right, code + 5);		//SE VADO A DX IL RAMO HA VALORE 1(2) - IN QUESTO CASO E' STATO MESSO +5 PER EVITARE CHE L'INT RAGGIUNGESSE I 10 MILIARDI
		}
		else {
			fill_table(code_table, tree_node->left, code * 10 + 1);	//SE VADO A SX IL RAMO HA VALORE 0(1)
			fill_table(code_table, tree_node->right, code * 10 + 2);//SE VADO A DX IL RAMO HA VALORE 1(2)
		}
	}

	return;
}

int compress_node(NODO * n, FILE * output, int *code_table) {
	if (n == NULL || n->word == NULL)
		return -1;
	compress_node(n->left, output, code_table);
	compress_string(n->word, output, code_table);
	compress_string(n->def, output, code_table);
	compress_node(n->right, output, code_table);

	return 0;
}

void compress_string(char *n_string, FILE *output, unsigned int *code_table) {
	char bit, c, byte = 0;										//1 BYTE
	unsigned int code, lenght, i = 0, bits_left = 8;

	do{
		if (n_string == NULL) {
			lenght = (int)log10((code_table[27]) + 1);			//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
			code = code_table[27];								//CODIFICA BIN
		}
		else {
			c = n_string[i];

			if (c >= 97 && c <= 122) {							//LETTERE MINUSCOLE			
				lenght = (int)log10((code_table[c - 97]) + 1);	//LENGHT = LUNGHEZZA CODIFICA (CODIFICA ASCII DELLE LETTERE - 97 = INDICE LETTERA NELLA TABELLA)
				code = code_table[c - 97];						//CODIFICA "BINARIA" DELLA LETTERA
			}
			if (c == 32) {										//CARATTERE SPAZIO
				lenght = (int)log10((code_table[26]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
				code = code_table[26];							//CODIFICA BIN
			}
			if (c == 0) {										//CARATTERE NULL
				lenght = (int)log10((code_table[27]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
				code = code_table[27];							//CODIFICA BIN
			}
			if (c == 10) {										//CARATTERE "NEW LINE"
				lenght = (int)log10((code_table[28]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
				code = code_table[28];							//CODIFICA BIN
			}
			if (c == 96) {										//CARATTERE "APOSTROFO"
				lenght = (int)log10((code_table[29]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
				code = code_table[29];							//CODIFICA BIN
			}
		}
		while (lenght + 1 > 0)								//MI SCORRO TUTTI I "BIT" DELLA CODIFICA
		{
			bit = (code / pow(10, lenght)) - 1;				//PRENDO "BIT PER BIT" LA CODIFICA DELLA LETTERA (-1 PERCHE' GLI 0 = 1 E 1 = 2 NELLA TABELLA)				
			switch (bit)
			{
			case 3:
				bit = 0;
				lenght++;
				code -= 3;
				break;
			case 4:
				bit = 1;
				lenght++;
				code -= 4;
				break;
			case 5:
				bit = 0;
				lenght++;
				code -= 4;
				break;
			case 6:
				bit = 1;
				lenght++;
				code -= 5;
				break;
			default:
				code -= (bit + 1)* pow(10, lenght);			//ELIMINO IL BIT VALUTATO
			}
			byte = byte | bit;								//PRENDO LA CODIFIA BINARIA DEL RESTO E LA METTO IN OR CON LE PRECEDENTI
			bits_left--;									//BIT NON "UTILIZZATI"
			lenght--;										//LUNGHEZZA DELLA CODIFICA RIMANENTE DA INSERIRE NEL FILE
			if (bits_left == 0) {							//SE HO RIEMPITO IL BYTE CON LA CODIFICA DELLE VARIE LETTERE
				fputc(byte, output);						//SCRIVO LA CODIFICA
				byte = 0;									//AZZERO X PER POTER "SCRIVERE" IL NUOVO BYTE
				bits_left = 8;								//RESETTO I BYTE "RIMANENTI" DI "BYTE"
			}
			byte = byte << 1;								//AVENDO INSERITO UNA CODIFICA "SHIFTO" I BIT PER NON SOVRASCRIVERLI
		}
		i++;
	} while (n_string != NULL && c != '\0');

	if (bits_left != 8){									//SE CODIFICANDO L'INTERA LINEA NON HO "RIEMPITO" IL BYTE
		byte = byte << (bits_left - 1);						//SHIFTO I BIT A SINISTA IN MODO DA NON AVERE "ZERI" CHE SI FRAPPONGANO FRA L'ULTIMA E LA PRECEDENTE LETTERA
		fputc(byte, output);								//SCRIVO IL BYTE NEL FILE
	}

	return;
}

int decompress_file(FILE *input, NODO **dict_root, HNode *tree){
	NODO *nodo_pointer;
	HNode *current = tree;
	char c, bit, *string;
	char mask = 1 << 7;
	int i, k = 0, bit_left;

	nodo_pointer = (NODO *)malloc(sizeof(NODO));
	string = (char *)malloc(sizeof(char)*20);
	if (nodo_pointer == NULL || string == NULL)
		return -1;
	nodo_pointer->word = string;

	while ((c = fgetc(input)) != EOF){									//ACQUISISCE CICLICAMENTE I CARATTERI FINCHE' NON ARRIVA ALLA FINE DEL FILE
		for (i = 0; i<8; i++){											//PER OGNI BIT
			bit = c & mask;												//PRENDE IL PRIMO BIT (AND CON 10000000)
			c = c << 1;													//SHIFTO IL BYTE
			if (bit == 0)												//SE IL BIT "ESTRATTO" E' "ZERO"
				current = current->left;								//MI SPOSTO SUL RAMO SINISTRO DELL'ALBERO
			else
				current = current->right;

			if (current->letter != 127){								//SE SONO GIUNTO AD UNA FOGLIA
				if (current->letter >= 0 && current->letter <= 25) {	//SE E' UNA LETTERA
					string[k] = current->letter + 97;
					k++;
				}
				if (current->letter == 26) {							//SE E' LO SPAZIO
					string[k] = 32;
					k++;
				}
				if (current->letter == 27)	{							//SE E' NULL
					if (k != 0)
						string[k] = '\0';
					else {
						free(string);
						if (string == nodo_pointer->word)
							nodo_pointer->word = NULL;
						else
							nodo_pointer->def = NULL;
						string = NULL;
					}
					if (string == nodo_pointer->word) {
						string = (char *)malloc(sizeof(char) * 50);
						nodo_pointer->def = string;
					}
					else {
						insertRBT(dict_root, nodo_pointer);
						nodo_pointer = (NODO *)malloc(sizeof(NODO));
						if (nodo_pointer == NULL)
							return -1;
						string = (char *)malloc(sizeof(char) * 20);
						nodo_pointer->word = string;
					}
					if (string == NULL)
						return -1;
					k = 0;
					i = 7;
				}
				if (current->letter == 28) {							//SE E' NEW LINE
					string[k] = 10;
					k++;
				}
				if (current->letter == 29) {							//SE E' L'APOSTROFO
					string[k] = 96;
					k++;
				}														//"STAMPO" NEW LINE NEL FILE
				current = tree;											//TORNO ALLA RADICE
			}
		}
	}

	return 0;
}

NODO * createFromFile(char * nameFile)
{
	unsigned short i = 0;
	char tmp;
	NODO* root = NULL;
	NODO* node = NULL;
	FILE* f = NULL;
	fopen_s(&f, nameFile, "r");
	if (f == NULL)
		return NULL;

	createSentinel();
		
	while (true)
	{
		//Create node
		node = (NODO*)malloc(sizeof(NODO));
		if (node == NULL)
			return NULL;

		//Create word space
		node->word = (char*)malloc(sizeof(char) * MAX_WORD);
		if (node->word == NULL)
			return NULL;

		//Add record
		node->def = NULL;
		node->isBlack = false;
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;

		tmp = getc(f);
		//Controllo che il carattere letto sia una lettera (anche accentata)
		for (i = 0; ((tmp >= 65 && tmp <= 90) || (tmp >= 97 && tmp <= 122) || (tmp >= 128 && tmp <= 165)); i++) {
			tmp = tolower(tmp);
			node->word[i] = tmp;
			tmp = getc(f);
		}
		node->word[i] = '\0'; //Add string terminator
		//Check if the word is 2 char lenght
		if (strlen(node->word) < 2) {
			free(node); //Release node
		}
		else {
			insertRBT(&root, node); //Insert node in RBT
			//printDictionary(root);
		}
		//Check if the file pointer is at the end
		if (tmp == EOF)
			break; //exit from the loop (infinity)
	}

	fclose(f);
	return root;
}

void printDictionary(NODO * dictionary)
{
	if (dictionary != NULL && dictionary != sentinel) {
		printDictionary(dictionary->left);
		printf("\"%s\": ", dictionary->word);
		printf("[%s]\n", dictionary->def);
		printDictionary(dictionary->right);
	}
}

//Function for print on file the dictionary
void printDictionaryFile(NODO * dictionary, FILE *f)
{
	if (dictionary != NULL && dictionary != sentinel) {
		printDictionaryFile(dictionary->left, f);
		fprintf(f, "\"%s\": ", dictionary->word);
		fprintf(f, "[%s]\n", dictionary->def);
		printDictionaryFile(dictionary->right, f);
	}
}

int countWord(NODO *n)
{
	if (n == NULL || n->word == NULL)						//CASO BASE (SENTINELLA)
		return 0;
	return countWord(n->left) + countWord(n->right) + 1;	//NUMERO PAROLE DEL RAMO SINISTRO + DESTRO + IL NODO
}

int insertWord(NODO ** dictionary, char * word)
{
	if (strlen(word) < 2)
		return 1;

	NODO* node = (NODO*)malloc(sizeof(NODO));
	if (node == NULL)
		return 1;

	node->def = NULL;
	node->isBlack = false;
	node->left = NULL;
	node->parent = NULL;
	node->right = NULL;
	node->word = (char *)malloc(sizeof(char) * MAX_WORD);
	if (node->word == NULL)
		return 1;
	strcpy_s(node->word, MAX_WORD, word);

	insertRBT(dictionary, node);
	return 0;
}

int cancWord(NODO ** dictionary, char * word)
{

}

char *getWordAt(NODO *n, int index)
{
	static int counter = 0;
	char *result = NULL;

	if (n->word == NULL)						//CASO BASE (SENTINELLA)
		return NULL;

	result = getWordAt(n->left, index);			//CONTROLLO IL FIGLIO SINISTRO
	if (result != NULL)							//SE HO TROVATO LA PAROLA LA RESTITUISCO
		return result;
	counter++;									//...ALTRIMENTI INCREMENTO IL CONTATORE
	if (counter == index)						//SE SONO ALLA I-ESIMA PAROLA (PARTENDO A CONTARE DA 1)
		return n->word;							//LA RESTITUISCO
	return getWordAt(n->right, index);			//ALTRIMENTI RESTITUISCO  QUELLO CHE MI PASSA IL FIGLIO DESTRO
}

int insertDef(NODO * dictionary, char * word, char * def)
{
	return 0;
}

char * searchDef(NODO * dictionary, char * word)
{
	return NULL;
}

int saveDictionary(NODO * dictionary, char * fileOutput)
{
	//check if the dictionary is empty
	if (dictionary == NULL)
		return -1;

	//Create file for write on it
	FILE* f = fopen(fileOutput, "w");	
	//check if the open file was fine
	if (f == NULL)
		return -1;

	//Print on file the dictionary
	printDictionaryFile(dictionary, f);

	fclose(f); //Close file
	return 0;
}

NODO * importDictionary(char * fileInput)
{
	return NULL;
}

int searchAdvance(NODO * dictionary, char * word, char ** first, char ** second, char ** third)
{
	return 0;
}

int compressHuffman(NODO * dictionary, char * file_name)
{
	unsigned int code_table[ELEMENTS];
	FILE *output_file = fopen(file_name, "w");
	HNode *root = build_huffman_tree();
	if (root == NULL)
		return -1;
	fill_table(code_table, root, 0);

	compress_node(dictionary, output_file, code_table);
	fclose(output_file);

	return 0;
}

int decompressHuffman(char * file_name, NODO ** dictionary)
{ 
	int x = 0;
	FILE *input_file = fopen(file_name, "r");
	HNode *root = build_huffman_tree();
	if (sentinel == NULL)
		x = createSentinel();
	if (root == NULL || input_file == NULL || x == -1)
		return -1;
	x = decompress_file(input_file, dictionary, root);
	fclose(input_file);

	return x;
}
