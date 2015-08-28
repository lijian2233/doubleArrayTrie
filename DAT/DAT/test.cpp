#include "DoubleArrayTrie.h"
#include <stdio.h>

void main() 
{

	char word[1024] = {'\0'};
	DoubleArrayTrie* pDAT = new DoubleArrayTrie();

	FILE *key_file = fopen("wordfilter.txt", "r");
	if(key_file == NULL) {
		printf("open key file error! \n");
		return ;
	}

	int i = 0;

	while(getline(key_file, word, 1024) >= 0){
		//while(fgets(word, 1024, key_file) >=0){
		pDAT->insert_word(word);
		++i;

		if (i%100 == 0){
			printf("now handle %i\n", i);
		}
	}

	pDAT->export_dict("wordfilter.dat");
	return;
	pDAT->load_dict("wordfilter.dat");

	char tmp[1024];
	while(1){
		printf("please input key :");
		gets(word);
		pDAT->match_word(word);
		printf("match word %s \n ", word);
	}
}