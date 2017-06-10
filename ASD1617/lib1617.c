#include "lib1617.h"

NODO *sentinel = NULL; //Sentinella per le foglie e padre della radice

						//   A	 B	 C	 D	  E	  F	  G	  H	  I	  J  K  L   M   N   O   
int letter_frequencies[] = { 81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76, 
						//  P   Q  R   S   T   U   V   W   X  Y	  Z	 SPC NULL NL  '  EOF
							20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130, 80, 80, 5, 80};


//PROTOTIPI FUNZIONI AUSILIARIE
int createSentinel();
unsigned short alphabeticalOrder(char *, char *);
void leftRotate(NODO **, NODO *);
void rightRotate(NODO **, NODO *);
void insertFixUp(NODO **, NODO **);
void insertRBT(NODO **, NODO *);
NODO* searchWord(NODO *, char *);

HNode * build_huffman_tree();
HNode * allocates_node(int);
HNode * extract_smaller_one(HNode **);
void fill_table(unsigned int *, HNode *, unsigned int);
int compress_node(NODO *, FILE *, unsigned int *);
void compress_string(char *, FILE *, unsigned int *);
char *find_index_word(NODO *, int, int *);
int decompress_file(FILE *, NODO **, HNode *);
int search_in_node(NODO *, MSWNode *, char *);
int levenshtein(const char *, int, const char *, int);
unsigned char convert_accent(unsigned char);

//FUNZIONI - IN WORKING

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
 *RETURN 3: n1-> NULL or n2-> NULL
*/
unsigned short alphabeticalOrder(char* n1, char* n2) {
	
	if (n1 == NULL || n2 == NULL)
		return 3;
	//Check character 
	for (int i = 0; i < MAX_WORD; i++) {
		if (n1[i] < n2[i])
			return 0;
		else if(n1[i] > n2[i])
			return 1;
	}

	return 2;
}

NODO* searchWord(NODO* root, char* word) {

	if ( root == NULL || alphabeticalOrder(root->word, word) == 2)
		return root;

	switch (alphabeticalOrder(root->word, word)) {
	case 0:
		return searchWord(root->right, word);
		break;
	case 1:
		return searchWord(root->left, word);
		break;
	}

	//Here there is an error
	return NULL;

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

//FUNZIONI "BASE"

NODO *createFromFile(char * nameFile)
{
	unsigned short i = 0;
	unsigned char tmp;
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
		for (i = 0; ((tmp >= 65 && tmp <= 90) || (tmp >= 97 && tmp <= 122) || (tmp >= 192 && tmp <= 252)); i++) {
			if (tmp >= 192 && tmp <= 252)
				tmp = convert_accent(tmp);
			else
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
		}
		//Check if the file pointer is at the end
		if (tmp == 0xff)
			break; //exit from the loop (infinity)
	}

	fclose(f);
	return root;
}

void printDictionary(NODO * dictionary) {
	if (dictionary != NULL && dictionary != sentinel) {
		printDictionary(dictionary->left);
		printf("\"%s\": ", dictionary->word);
		printf("[%s]\n", dictionary->def);
		printDictionary(dictionary->right);
	}
}

//Function for print on file the dictionary
void printDictionaryFile(NODO * dictionary, FILE *f) {
	if (dictionary != NULL && dictionary != sentinel) {
		printDictionaryFile(dictionary->left, f);
		fprintf(f, "\"%s\": ", dictionary->word);
		fprintf(f, "[%s]\n", dictionary->def);
		printDictionaryFile(dictionary->right, f);
	}
}

int countWord(NODO *n) {
	if (n == NULL || n->word == NULL)						//CASO BASE (SENTINELLA)
		return 0;
	return countWord(n->left) + countWord(n->right) + 1;	//NUMERO PAROLE DEL RAMO SINISTRO + DESTRO + IL NODO
}

int insertWord(NODO ** dictionary, char * word) {
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

void rb_trasplant(NODO** root, NODO* u, NODO* v) {

	if (u->parent == sentinel)
		*root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else {
		u->parent->right = v;
		v->parent = u->parent;
	}
}

void rb_deleteFixUp(NODO** root, NODO * x) {

	NODO* w = NULL;

	while (*root != x && x->isBlack == true) {
		
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->isBlack == false) {
				w->isBlack = true;
				x->parent->isBlack = false;
				leftRotate(root, x->parent);
				w = x->parent->right;
			}
			if ((w->left->isBlack == true) && (w->right->isBlack == true)) {
				w->isBlack = false;
				x = x->parent;
			}
			else {
				if (w->right->isBlack == true) {
					w->left->isBlack = true;
					w->isBlack = false;
					rightRotate(root, w);
					w = x->parent->right;
				}
				w->isBlack = x->parent->isBlack;
				x->parent->isBlack = true;
				w->right->isBlack = true;
				leftRotate(root, x->parent);
				x = *root;
			}
		}
		else {
			w = x->parent->left;
			if (w->isBlack == false) {
				w->isBlack = true;
				x->parent->isBlack = false;
				rightRotate(root, x->parent);
				w = x->parent->left;
			}
			if ((w->right->isBlack == true) && (w->left->isBlack == true)) {
				w->isBlack = false;
				x = x->parent;
			}
			else {
				if (w->left->isBlack == true) {
					w->right->isBlack = true;
					w->isBlack = false;
					leftRotate(root, w);
					w = x->parent->left;
				}
				w->isBlack = x->parent->isBlack;
				x->parent->isBlack = true;
				w->left->isBlack = true;
				rightRotate(root, x->parent);
				x = *root;
			}
		}

	}
	x->isBlack = true;
}

NODO* treeMinimum(NODO* x) {

	while (x->left != sentinel)
		x = x->left;

	return x;
}

NODO* treeSuccessor(NODO* root, NODO* x) {
	if (x->right != sentinel)
		return treeMinimum(x->right);

	NODO* y = x->parent;
	while (y != NULL && x == y->right) {
		x = y;
		y = y->parent;
	}
	return y;
}

/*NODO* treeSuccessor(NODO* root, NODO* x) {

	NODO* y = NULL;

	if ((y = x->right) == sentinel) {

		while (y->left != sentinel)
			y = y->left;

		return y;
	}
	else {
		y = x->parent;
		while (x == y->right) {
			x = y;
			y = y->parent;
		}
		if (y == root) return sentinel;

		return y;
	}
}*/

void rb_delete(NODO** root, NODO* z) {
	
	NODO* y = NULL;
	NODO* x = NULL;
	NODO* _root = *root;

	y = ((z->left == sentinel) || (z->right == sentinel)) ? z : treeSuccessor(*root, z);
	x = (y->left == sentinel) ? y->right : y->left;
	if (_root == (x->parent = y->parent))
		_root->left = x;
	else {
		if (y == y->parent->left)
			y->parent->left = x;
		else
			y->parent->right = x;
	}

	if (y != z) {
		if (y->isBlack == true) rb_deleteFixUp(&_root, x);

		y->left = z->left;
		y->right = z->right;
		y->parent = z->parent;
		y->isBlack = z->isBlack;
		z->left->parent = z->right->parent = y;
		if (z == z->parent->left)
			z->parent->left = y;
		else
			z->parent->right = y;
		free(z);
	}
	else {
		if (y->isBlack == true) rb_deleteFixUp(&_root, x);
		free(y);
	}
}

int cancWord(NODO ** dictionary, char * word)
{
	NODO* sWord = searchWord(*dictionary, word);
	if (sWord == NULL)
		return 1;

	rb_delete(dictionary, sWord);
	return 0;
}

char *getWordAt(NODO *n, int index) {
	int counter = 0;
	if (n == NULL)
		return NULL;
	return find_index_word(n, index, &counter);
}

int insertDef(NODO * dictionary, char * word, char * def)
{
	NODO* sWord = searchWord(dictionary, word);
	if (sWord == NULL)
		return 1; //Word not found

	sWord->def = (NODO*)malloc(sizeof(char) * MAX_DEF);
	if (sWord == NULL)
		return 1;
	strncpy(sWord->def, def, MAX_DEF); //Copy the definition with safe string copy
	return 0; //All ok
}

char *searchDef(NODO * dictionary, char * word)
{
	//search the word
	NODO* sWord = searchWord(dictionary, word);
	if (sWord == NULL) //check if the word exist
		return NULL;

	return sWord->def; //return the definition
}

int saveDictionary(NODO * dictionary, char * fileOutput) {
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

/*char* readApici(FILE* f, char* nullChar, char* string) {

	string[0] = '\0';
	nullChar[0] = '\0';

	//while (nullChar[0] = fgetc(f) != '"')
		//continue;

	while ((nullChar[0] = fgetc(f)) != '"') {

		nullChar[1] = '\0';
		strncat(string, nullChar, MAX_WORD);
	}

	return string;
}*/

/*char* readQuadre(FILE* f, char* nullChar, char* str) {

	str[0] = '\0';
	nullChar[0] = '\0';

	while (nullChar[0] = fgetc(f) != '[')
		continue;

	while ((nullChar[0] = fgetc(f)) != ']') {

		nullChar[1] = '\0';
		strncat(str, nullChar, MAX_WORD);
	}


	if (!strncmp(str, "(null)", MAX_DEF))
		return NULL;

	return str;
}*/

/*NODO *importDictionary(char * fileInput)
{
	char* str = NULL;
	char* nullChar = NULL;
	int setP = 0;
	NODO* root = NULL;
	NODO* node = NULL;
	FILE* f = fopen(fileInput, "rb");

	nullChar = (char *)malloc(sizeof(char) * 2);
	str = (char *)malloc(sizeof(char) * MAX_WORD);

	if (f == NULL || nullChar == NULL || str == NULL)
		return NULL;

	nullChar[1] = '\0';
	nullChar[0] = fgetc(f);

	while (!feof(f)) {			
		
		node = (NODO*)malloc(sizeof(NODO));
		node->word = (char *)malloc(sizeof(char) * MAX_WORD);
		node->def = (char *)malloc(sizeof(char) * MAX_DEF);
		if (node == NULL || node->word == NULL || node->def == NULL)
			return NULL;

		strncpy(node->word, readApici(f, nullChar, str), MAX_WORD);

		if (readQuadre(f, nullChar, str) == NULL)
			node->def = NULL;
		else
			strncpy(node->def, readQuadre(f, nullChar, str), MAX_DEF);		

		node->isBlack = false;
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;

		insertRBT(&root, node);	

		nullChar[0] = fgetc(f);
		if (nullChar[0] == '\n') {
			nullChar[0] = fgetc(f);
			if (nullChar[0] == EOF)
				break;
		}	

	}

	fclose(f);
	return root;
}*/

void removeChar(char* str, char garbage, char garbage2) {
	char *src, *dst; //to strings pointer
	for (src = dst = str; *src != '\0'; src++) {
		*dst = *src; //copy char form src to dst
		if (*dst != garbage && *dst != garbage2) dst++; //not find garbage character
	}
	*dst = '\0'; //ad end string
}

short readWordDef(FILE* f, char* word, char* def, bool* endFile) {

	char rchar; //simple sentinel for find EOF
	//Check if the argumets are omogeneous
	if (f == NULL || word == NULL || def == NULL)
		return -1; //bad

	fscanf(f, "%s", word); //read the line
	removeChar(word, '\"', ':');

	fscanf(f, "%[^\"]s", def); //read the line
	removeChar(def, '[', ']'); //remove [] character
	removeChar(def, '\n', '\ ');
	removeChar(def, '\r', '\r');
	//detect the EOF
	if (rchar = getc(f) == EOF)
		*endFile = true;

	return 0; //ok

}

NODO *importDictionary(char * fileInput) {

	//open file
	FILE* f = fopen(fileInput, "rb");
	if (f == NULL)
		return NULL;

	bool endFile = false; //for detect EOF
	NODO* root = NULL;
	NODO* node = NULL;

	//Allocate string for read from file
	char* word = (char*)malloc(sizeof(char) * MAX_WORD + 5);
	if (word == NULL)
		return NULL;
	char* def = (char*)malloc(sizeof(char) * MAX_DEF + 5);
	if (def == NULL)
		return NULL;
	//loop end of file
	while (!endFile) {
		//read word and definition
		if (readWordDef(f, word, def, &endFile) == -1)
			return NULL;
		//new node
		node = (NODO*)malloc(sizeof(NODO));
		if (node == NULL)
			return NULL;
		node->def = (char*)malloc(sizeof(char) * MAX_DEF);
		node->word = (char*)malloc(sizeof(char) * MAX_WORD);
		if (node->word == NULL || node->def == NULL)
			return NULL;
		//check if the def is null
		if (!strncmp(def, "(null)\r", MAX_DEF))
			node->def = NULL;
		else 
			strncpy(node->def, def, MAX_DEF);
		strncpy(node->word, word, MAX_WORD);
		node->isBlack = false;
		node->left = NULL;
		node->parent = NULL;
		node->right = NULL;
		//add to the tree
		insertRBT(&root, node);
	}

	return root;
}

int searchAdvance(NODO * dictionary, char * word, char ** first, char ** second, char ** third) {
	if (word == NULL || dictionary == NULL)
		return -1;

	MSWNode head[3];
	head[0].w_pointer = first;
	head[1].w_pointer = second;
	head[2].w_pointer = third;
	head[0].distance = head[1].distance = head[2].distance = 0x7fff;

	int res = search_in_node(dictionary, head, word);
	if (head[0].distance == 0x7fff ||
		head[1].distance == 0x7fff ||
		head[2].distance == 0x7fff)
		return -1;
	return res;
}

int compressHuffman(NODO * dictionary, char * file_name) {
	char eof = 27;
	unsigned int code_table[ELEMENTS];
	FILE *output_file = fopen(file_name, "wb");
	HNode *root = build_huffman_tree();
	if (root == NULL)
		return -1;
	fill_table(code_table, root, 0);
	compress_node(dictionary, output_file, code_table);
	compress_string(&eof, output_file, code_table);
	fclose(output_file);

	return 0;
}

int decompressHuffman(char * file_name, NODO ** dictionary) { 
	int x = 0;
	FILE *input_file = fopen(file_name, "rb");
	HNode *root = build_huffman_tree();
	if (sentinel == NULL)
		x = createSentinel();
	if (root == NULL || input_file == NULL || x == -1)
		return -1;
	x = decompress_file(input_file, dictionary, root);
	fclose(input_file);

	return x;
}

//FUNZIONI AUSILIARIE - COMPLETE

HNode *build_huffman_tree() {
	HNode *nodes_head = allocates_node(0);				//ALLOCO IL PRIMO NODO DELLA LISTA
	HNode *temp = nodes_head;

	if (temp == NULL)									//CONTROLLO L'ALLOCAZIONE
		return NULL;

	temp->frequencies = letter_frequencies[0];			//FREQUENZA DELLA LETTERA (CONTENUTA NELL'ARRAY)

	for (int i = 1; i < ELEMENTS; i++) {				//PER OGNI LETTERA DOPO LA PRIMA, ALLOCO UN NODO (CREO UNA LISTA)
		temp->next = allocates_node(i);					//ALLOCO UN NUOVO NODO E LO FACCIO PUNTARE DAL NEXT DEL PRECEDENTE
		temp = temp->next;								//SPOSTO LA VAR TEMP AL NODO APPENA CREATO (O A NULL IN CASO D'ERRORE)
		if (temp == NULL)								//CONTROLLO L'ALLOCAZIONE
			return NULL;
		temp->frequencies = letter_frequencies[i];		//ASSOCIO LA FREQUENZA DELLA LETTERA CONTENUTA NELL'ARRAY ALLA VARIABILE NEL NODO
	}

	while (nodes_head->next != NULL) {					//QUANDO HO ALTRI NODI OLTRE LA "RADICE" (SE NON CI SON STATI ERRORI PRECEDENTI CI SARA' SEMPRE ALMENO UN NODO; IN CASO CONTRARIO AVREBBE RESTITUITO NULL)		
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

HNode *allocates_node(int i) {
	HNode *node = (HNode *)malloc(sizeof(HNode));		//ALLOCO IL NODO
	if (node == NULL)									//CONTROLLO L'ALLOCAZIONE
		return NULL;

	node->letter = i;									//INDICE DELL'ARRAY CORRISPONDENTE ALLA LETTERA
	node->left = node->right = node->next = NULL;

	return node;										//PUNTATORE AL NODO
}

HNode *extract_smaller_one(HNode **nodes_head) {
	HNode *minimum = *nodes_head;
	HNode *scrolling = *nodes_head, *previous = NULL;

	if (*nodes_head == NULL)							//SE LA LISTA NON CONTIENE ELEMENTI
		return NULL;

	if ((*nodes_head)->next == NULL) {					//SE CONTIENE SOLO UN ELEMENTO
		*nodes_head = NULL;								//SVUOTO LA LISTA
		return minimum;									//RITORNO LA RADICE DELL'ALBERO
	}

	while (scrolling->next != NULL) {					//SCORRO LA LISTA
		if (scrolling->next->frequencies < minimum->frequencies) {	//SE TROVO UN ELEMENTO DI FREQUENZA MINORE
			previous = scrolling;						//TENGO IN MEMORIA IL NODO PRECEDENTE AL MINIMO
			minimum = scrolling->next;					//AGGIORNO IL MINIMO
		}
		scrolling = scrolling->next;					//VADO ALL'ELEMENTO SUCCESSIVO DELLA LISTA
	}
	if (previous == NULL) {								//SE L'ELEMENTO ERA IL PRIMO DELLA LISTA
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
		if ((code % 10) == 1 || (code % 10) == 2) {					
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
			if (c == 27) {										//CARATTERE "ESCAPE"
				lenght = (int)log10((code_table[30]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
				code = code_table[30];							//CODIFICA BIN
			}
		}
		while (lenght + 1 > 0)									//MI SCORRO TUTTI I "BIT" DELLA CODIFICA
		{
			bit = (code / pow(10, lenght)) - 1;					//PRENDO "BIT PER BIT" LA CODIFICA DELLA LETTERA (-1 PERCHE' GLI 0 = 1 E 1 = 2 NELLA TABELLA)				
			switch (bit)
			{
			case 3:
				bit = 0;
				code -= (3 * pow(10, lenght));
				lenght++;
				break;
			case 4:
				bit = 1;
				code -= (4 * pow(10, lenght));
				lenght++;
				break;
			case 5:
				bit = 0;
				code -= (4 * pow(10, lenght));
				lenght++;
				break;
			case 6:
				bit = 1;
				code -= (5 * pow(10, lenght));
				lenght++;

				break;
			default:
				code -= (bit + 1) * pow(10, lenght);			//ELIMINO IL BIT VALUTATO
			}
			byte = byte | bit;									//PRENDO LA CODIFIA BINARIA DEL RESTO E LA METTO IN OR CON LE PRECEDENTI
			bits_left--;										//BIT NON "UTILIZZATI"
			lenght--;											//LUNGHEZZA DELLA CODIFICA RIMANENTE DA INSERIRE NEL FILE
			if (bits_left == 0) {								//SE HO RIEMPITO IL BYTE CON LA CODIFICA DELLE VARIE LETTERE
				fputc(byte, output);							//SCRIVO LA CODIFICA
				byte = 0;										//AZZERO X PER POTER "SCRIVERE" IL NUOVO BYTE
				bits_left = 8;									//RESETTO I BYTE "RIMANENTI" DI "BYTE"
			}
			byte = byte << 1;									//AVENDO INSERITO UNA CODIFICA "SHIFTO" I BIT PER NON SOVRASCRIVERLI
		}
		i++;
	} while (n_string != NULL && c != '\0');

	if (bits_left != 8){										//SE CODIFICANDO L'INTERA LINEA NON HO "RIEMPITO" IL BYTE
		byte = byte << (bits_left - 1);							//SHIFTO I BIT A SINISTA IN MODO DA NON AVERE "ZERI" CHE SI FRAPPONGANO FRA L'ULTIMA E LA PRECEDENTE LETTERA
		fputc(byte, output);									//SCRIVO IL BYTE NEL FILE
	}

	return;
}

char *find_index_word(NODO *n, int index, int *counter_pt) {
	if (n->word == NULL)										//CASO BASE (SENTINELLA)
		return NULL;

	char *result = find_index_word(n->left, index, counter_pt);	//CONTROLLO IL FIGLIO SINISTRO
	if (result != NULL)											//SE HO TROVATO LA PAROLA LA RESTITUISCO
		return result;

	if (*counter_pt == index)									//SE SONO ALLA I-ESIMA PAROLA (PARTENDO A CONTARE DA 1)
		return n->word;											//LA RESTITUISCO
	(*counter_pt)++;
	return find_index_word(n->right, index, counter_pt);		//ALTRIMENTI RESTITUISCO  QUELLO CHE MI PASSA IL FIGLIO DESTRO
}

int decompress_file(FILE *input, NODO **dict_root, HNode *tree) {
	HNode *current = tree;
	register char c;
	register int k = 0;
	char bit;
	int i, end_of_file = 0;

	NODO *nodo_pointer = (NODO *)malloc(sizeof(NODO));
	char *string = (char *)malloc(sizeof(char) * 20);
	if (nodo_pointer == NULL || string == NULL)
		return -1;
	nodo_pointer->word = string;

	if ((c = fgetc(input)) == EOF)
		return -1;

	while (end_of_file != 1) {											//ACQUISISCE CICLICAMENTE I CARATTERI FINCHE' NON ARRIVA ALLA FINE DEL FILE
		for (i = 0; i<8; i++) {											//PER OGNI BIT
			bit = c & 0x80;												//PRENDE IL PRIMO BIT (AND CON 10000000)
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
				if (current->letter == 27) {							//SE E' NULL
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
				if (current->letter == ELEMENTS - 1) {					//SE E' L'EOF CREATO DA ME
					end_of_file = 1;
				}
				current = tree;											//TORNO ALLA RADICE
			}
		}
		c = fgetc(input);
	}
	return 0;
}

int search_in_node(NODO *n, MSWNode *head, char *word) {
	if (n->word == NULL)
		return 0;

	int ris = search_in_node(n->left, head, word);

	if (!strcmp(n->word, word))
		ris = 1;
	else {
		int ln = strlen(n->word);
		int lw = strlen(word);
		int dist = levenshtein(n->word, ln, word, lw);
		if (dist < head[2].distance) {
			head[2].distance = dist;
			*(head[2].w_pointer) = n->word;
			if (head[2].distance < head[1].distance) {
				MSWNode tmp = head[1];
				head[1] = head[2];
				head[2] = tmp;
				if (head[1].distance < head[0].distance) {
					tmp = head[0];
					head[0] = head[1];
					head[1] = tmp;
				}
			}
		}
	}
	return ris | search_in_node(n->right, head, word);
}

int levenshtein(const char *s, int ls, const char *t, int lt) {
	if (!ls)
		return lt;
	if (!lt)
		return ls;

	if (s[ls] == t[ls])
		return levenshtein(s, ls - 1, t, lt - 1);

	int a = levenshtein(s, ls - 1, t, lt - 1);
	int b = levenshtein(s, ls, t, lt - 1);
	int c = levenshtein(s, ls - 1, t, lt);

	if (a > b) a = b;
	if (a > c) a = c;

	return a + 1;
}

unsigned char convert_accent(unsigned char c) {
	if ((c >= 192 && c <= 198) || (c >= 224 && c <= 230))
		return 'a';
	if ((c >= 200 && c <= 203) || (c >= 232 && c <= 235))
		return 'e';
	if ((c >= 204 && c <= 207) || (c >= 236 && c <= 239))
		return 'i';
	if ((c >= 210 && c <= 214) || c == 216 || c == 240 || (c >= 242 && c <= 248))
		return 'o';
	if ((c >= 217 && c <= 220) || (c >= 249 && c <= 252))
		return 'u';
	if (c == 223)
		return 'b';
	if (c == 199 || c == 231)
		return 'c';
	if (c == 208)
		return 'd';
	if (c == 209 || c == 241)
		return 'n';
	if (c == 222)
		return 'p';
	if (c == 215)
		return 'x';
	if (c == 221)
		return 'y';
}