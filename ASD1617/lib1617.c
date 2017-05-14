#include "lib1617.h"

NODO* sentinel = NULL; //Sentinella per le foglie e padre della radice

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
		if (strlen(node->word) <= 2) {
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
		printf("%s\n", dictionary->word);
		printDictionary(dictionary->right);
	}
}

int countWord(NODO * dictionary)
{
	return 0;
}

int insertWord(NODO ** dictionary, char * word)
{
	return 0;
}

int cancWord(NODO ** dictionary, char * word)
{
	return 0;
}

char * getWordAt(NODO * dictionary, int index)
{
	return NULL;
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

int compressHuffman(NODO * dictionary, char * fileOutput)
{
	return 0;
}

int decompressHuffman(char * fileInput, NODO ** dictionary)
{
	return 0;
}
