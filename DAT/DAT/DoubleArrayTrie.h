#ifndef _DOUBLE_ARRAY_TRIE_H_
#define _DOUBLE_ARRAY_TRIE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getline(FILE* f, char* buf, int size);
class DoubleArrayTrie
{
private:
	int*  m_bcArray;    //base and check
	char* m_tail;       //tail array
	int   m_bcPos;
	int   m_tailPos;
	int   m_bcMax;
	int   m_tailMax; 
	char* m_tmp;
	int   m_tmpMax;

private:
	void initialize();
	void release();
	void realloc_bc();
	void separate(int s, char *b, int tail_pos);
	int  change_bc(int current, int s, char *list, char ch);

	int base(int n) 
	{
		if(n > m_bcPos || n < 0) {
			return 0;
		} else {
			return m_bcArray[2 * n];
		}
	}

	int check(int n) 
	{
		if(n > m_bcPos || n < 0) {
			return 0;
		} else {
			return m_bcArray[2 * n + 1];
		}
	}

	void w_base(int n, int node) 
	{
		while(n >= m_bcMax) {
			realloc_bc();
		}
		if(n > m_bcPos) {
			m_bcPos = n;
		}
		m_bcArray[2 * n] = node; 
	}

	void w_check(int n, int node) 
	{
		while(n >= m_bcMax) {
			realloc_bc();
		}
		if(n > m_bcPos) {
			m_bcPos = n;
		}
		m_bcArray[2 * n + 1] = node; 
	}

	int x_check(char *list) 
	{
		int i, base_pos = 1, check_pos;
		unsigned char ch;
		i = 0;
		do {
			ch = list[i++];
			check_pos = base_pos + ch;  //change
			if(check(check_pos) != 0) {
				base_pos++;
				i = 0;
				continue;
			}
		} while(list[i] != '\0');
		return base_pos;
	}

	char *set_list(int s); 
	void read_tail(int p);
	void read_tail(int p, char* tmp, int size); //read tail data to tmp
	void write_tail(char *temp, int p);
	void tail_insert(int s, char *a, char *b);
	void bc_insert(int s, char *b);
	
	int  check_match_word(char* p_word);
	

public:
	DoubleArrayTrie();
	~DoubleArrayTrie();
	int  insert_word(char *p_word) ;
	int  search_word(char *p_word);
	int  delete_word(char *p_word);
	void export_dict(char* path);
	void load_dict(char* path);
	void match_word(char* p_word);
};
#endif //_DOUBLE_ARRAY_TRIE_H_