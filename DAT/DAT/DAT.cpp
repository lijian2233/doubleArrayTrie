/*#include<iostream>
#include<string>
using namespace std;

#define MIN_CODE 1
#define MAX_CODE 255
#define BC_INC 10240
#define TAIL_INC 102400
#define TEMP_INC 1024

int *BC;
char *TAIL;
char *TEMP;
int BC_POS;
int TAIL_POS;
int BC_MAX;
int TAIL_MAX;
int TEMP_MAX;

void realloc_bc();
void separate(int s, char *b, int tail_pos);
int  change_bc(int current, int s, char *list, char ch);

int base(int n) {
	if(n > BC_POS) {
		return 0;
	} else {
		return BC[2 * n];
	}
}

int check(int n) {
	if(n > BC_POS) {
		return 0;
	} else {
		return BC[2 * n + 1];
	}
}

void w_base(int n, int node) {
	while(n >= BC_MAX) {
		realloc_bc();
	}
	if(n > BC_POS) {
		BC_POS = n;
	}
	BC[2 * n] = node; 
}

void w_check(int n, int node) {
	while(n >= BC_MAX) {
		realloc_bc();
	}
	if(n > BC_POS) {
		BC_POS = n;
	}
	BC[2 * n + 1] = node; 
}

char *mem_str(char *area_name, int *max, int init) {
	*max = init;
	char *area = (char*)malloc(sizeof(char) * (*max));
	if(area == NULL) {
		cout << area_name << " malloc error!" << endl;
	}

	memset(area, *max, '\0');
	return area;
}

void realloc_bc() {
	int i, pre_bc;
	pre_bc = BC_MAX;
	BC_MAX += BC_INC;
	BC =(int*)realloc(BC, sizeof(int) * 2 * BC_MAX);
	if(BC == NULL) {
		cout << "realloc bc error!" << endl;
		return;
	}
	for(i = 2 * pre_bc; i < 2 * BC_MAX; i++) {
		BC[i] = 0;
	}
}

char *realloc_str(char *area_name, char *area, int *max, int inc) {
	int pre_size;
	int i;
	pre_size = *max;
	*max += inc;
	area = (char*) realloc(area, sizeof(char) * (*max));
	if(area == NULL) {
		cout << area_name << " realloc error!" << endl;
		exit(-1);
	}
	for(i = pre_size; i < *max; i++) {
		area[i] = '\0';
	}

	cout << area_name << " realloc ok!" << endl;
	return area;
}

void read_tail(int p) {
	int i = 0;
	while(TAIL[p] != '#') TEMP[i++] = TAIL[p++];
	TEMP[i++] = '#';
	TEMP[i] = '\0';
}

void write_tail(char *temp, int p) {
	int i = 0; 
	int tail_index;

	tail_index = p;
	while((p + strlen(temp)) >= TAIL_MAX - 1) {
		TAIL = realloc_str("TAIL", TAIL, &TAIL_MAX, TAIL_INC);
	}

	if (temp[0] == 0){
		TAIL[tail_index] = '#';
	}else{
		while(*(temp + i) != '\0') {
			TAIL[tail_index++] = *(temp + i);
			i++;
		}
	}

	if(p + i + 1 > TAIL_POS) {
		TAIL_POS = p + i;
	}
}

int x_check(char *list) {
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

char *set_list(int s) {
	char *list = (char*)malloc(MAX_CODE + 1 + 1); // 256¸ö×Ö·û + 1 '\0'
	int i, j = 0, t;
	for(i = MIN_CODE; i < MAX_CODE; i++) {
		t = base(s) + i;
		if(check(t) == s) {
			list[j] = (unsigned char)i; //change
			j++;
		}
	}
	list[j] = '\0';
	return list;
}

void separate(int s, char *b, int tail_pos) {
	int t = base(s) + (unsigned char)(*b); // change
	b++;
	w_check(t, s);
	w_base(t, (-1) * tail_pos);
	write_tail(b, tail_pos);
}

void bc_insert(int s, char *b) {
	int t;
	char list_s[MAX_CODE + 2];
	char list_t[MAX_CODE + 2];
	t = base(s) + (unsigned char)(*b); // change
	if(check(t) != 0) {
		strcpy(list_s, set_list(s));
		strcpy(list_t, set_list(check(t)));
		if(strlen(list_s) + 1 < strlen(list_t)) {
			s = change_bc(s, s, list_s, *b);
		} else {
			s = change_bc(s, check(t), list_t, '\0');
		}
	}

	separate(s, b, TAIL_POS);
}

int  change_bc(int current, int s, char *list, char ch) {
	int i, k, old_node, new_node, old_base;
	char a_list [MAX_CODE + 2];
	old_base = base(s);
	if(ch != '\0') {
		strcpy(a_list, list);
		i = strlen(a_list);
		a_list[i] = ch;
		a_list[i + 1] = '\0';
	} else {
		strcpy(a_list, list);
	}
	w_base(s, x_check(a_list));
	i = 0;
	do {
		old_node = old_base + (unsigned char)(*list); //change
		new_node = base(s) + (unsigned char)(*list);
		w_base(new_node, base(old_node));
		w_check(new_node, s);
		if(base(old_node) > 0) {
			k = base(old_node) + 1;
			while(k - base(old_node) <= MAX_CODE || k < BC_POS) {
				if(check(k) == old_node) {
					w_check(k, new_node);
				}
				++k;
			}
		}
		if(current != s && old_node == current) {
			current = new_node;
		}
		w_base(old_node, 0);
		w_check(old_node, 0);
		list++;
	} while(*list != '\0');
	return current;
}

void tail_insert(int s, char *a, char *b) {
	char list[3];
	unsigned char ch;
	int i = 0;
	int length = 0;
	int t;
	int old_tail_pos;
	old_tail_pos = (-1) * base(s);
	while(a[length] == b[length])
		length++;
	while(i < length) {
		ch = a[i++];
		list[0] = ch;
		list[1] = '\0';
		w_base(s, x_check(list));
		t = base(s) + (unsigned char)(ch);
		w_check(t, s);
		s = t;
	}
	list[0] = a[length];
	list[1] = b[length];
	list[2] = '\0';
	w_base(s, x_check(list));
	separate(s, a + length, old_tail_pos);
	separate(s, b + length, TAIL_POS);
}

int search_word(char *p_word) { // if found word, return its base index, if not, return -1
	unsigned char ch;
	int h = -1;
	int s = 1;
	int t;
	do {
		++h;
		ch = p_word[h];
		t = base(s) + (unsigned char)(ch);
		if(check(t) != s) { 
			return -1;
		}
		if(base(t) < 0) {
			break;
		}
		s = t;
	} while(*(p_word + h));
	if(p_word[h] != '#')
		read_tail((-1) * base(t));
	if(p_word[h] == '#' || strcmp(TEMP, p_word + h + 1) == 0) {
		return t;
	} else {
		return -1;
	}
}

int delete_word(char *p_word) { // if delete given word, return 1, else return 0
	int t = search_word(p_word);
	if(t = -1) {
		return 0;
	} else {
		w_base(t, 0);
		w_check(t, 0);
		return 1;
	}
}

int insert_word(char *p_word) {
	unsigned char ch;
	int h = -1;
	int s = 1;
	int t;
	strcat(p_word, "#");
	do {
		++h;
		ch = *(p_word + h);
		t = base(s) + (unsigned char)(ch);
		if(check(t) != s) {
			bc_insert(s, p_word + h);
			return 1;
		}
		if(base(t) < 0) {
			break;
		}
		s = t;
	} while(*(p_word + h));

	if(p_word[h] != '#')
		read_tail((-1) * base(t));
	if(p_word[h] == '#' || strcmp(TEMP, p_word + h + 1) == 0) {
		return 1;
	} 
	if(base(t) != 0) {
		tail_insert(t, TEMP, p_word + h + 1);
	}
	return 1;
}

void initialize() {
	BC_MAX = BC_INC*300;
	BC_POS = 1;
	TAIL_POS = 1;
	BC = (int*)malloc(sizeof(int) * 2 * BC_MAX);
	if(BC == NULL) {
		cout << "BC malloc error!" << endl;
		return;
	}
	for(int i = 0; i < 2 * BC_MAX; i++) {
		BC[i] = 0;
	}
	w_base(1, 1);
	BC_POS = 1;

	TAIL = mem_str("TAIL", &TAIL_MAX, TAIL_INC);
	TAIL[0] = '#';
	TEMP = mem_str("TEMP", &TEMP_MAX, TEMP_INC);
} 


int check_match_word(char* p_word)
{
	unsigned char ch;
	int h = 0;
	int s = 1;
	int t, t1;
	int m = 0;

	while(*(p_word+h)){
		ch = p_word[h];

		t = base(s) + (unsigned char)(ch);
		if(check(t) != s) 
			return m;
		
		if(base(t) < 0) {
			break;
		}

		t1 = base(t) + (unsigned char)('#');
		if (base(t1) < 0 && check(t1) == t)
			m = h+1;

		s = t;
		++h;
	}

	if (*(p_word+h) == 0){
		return m;
	}

	if (base(t) < 0)
		read_tail((-1) * base(t));

	++h;
	int j = 0;
	while(p_word[h]!=0 && TEMP[j]!='#'){
		if (p_word[h] == TEMP[j]){
			++h;
			++j;
		}else{
			return m;
		}
	}

	if (TEMP[j] != '#') return m;

	return h;
}

void match_word(char* p_word)
{
	int start = 0;
	int len = strlen(p_word);

	do 
	{
		int r = check_match_word(p_word+start);
		if (r == 0){
			++start;
		}else{
			memset(p_word+start, '*', r);
			start += r;
		}
	} while (start < len);
}

int getline(FILE* f, char* buf, int size)
{
	char ch;
	int start = 0;
	buf[0] = 0;
	while((ch=fgetc(f)) != EOF){
		if (ch == '\n'){
			buf[start] = 0;
			return start;
		}

		if (ch != '\r')
			buf[start++] = ch;
	}

	return -1;
}


void export_dict(char* path)
{
	FILE* f = fopen(path, "wb");
	if (f == NULL){
		printf("open file %s failed, export falied\n", path);
		return;
	}

	
	fprintf(f, "%u\n", BC_POS);    
	fprintf(f, "%u\n", TAIL_POS);
	fprintf(f, "%u\n", BC_MAX);
	fprintf(f, "%u\n", TAIL_MAX);
	//write BC data
	fwrite(BC, BC_MAX*sizeof(int), 1, f);
	//write TAIL data
	fwrite(TAIL, TAIL_MAX, 1, f);
	fclose(f);

}

void load_dict(char* path)
{
	FILE* f = fopen(path, "rb");
	if (f == NULL){
		printf("open file %s failed, load dict falied\n", path);
		return;
	}

	//local BC_POS
	char buf[1024];
	getline(f, buf, 1024);
	BC_POS = atoi(buf);
	getline(f, buf, 1024);
	TAIL_POS = atoi(buf);
	getline(f, buf, 1024);
	BC_MAX = atoi(buf);
	getline(f, buf, 1024);
	TAIL_MAX = atoi(buf);

	BC = (int*)malloc(BC_MAX*sizeof(int));
	int  r  = fread(BC,sizeof(int),BC_MAX, f);
	TAIL = (char*)malloc(TAIL_MAX);
	r = fread(TAIL, 1,TAIL_MAX, f);
	fclose(f);
	TEMP = mem_str("TEMP", &TEMP_MAX, TEMP_INC);
}

#include <set>
#include <string>
void test()
{
	using namespace std;
	std::set<std::string> store;
	store.insert(string("ÖìÈÝ»ù"));
	store.insert(string("ÖìÈÝ»ù1"));

	for(std::set<std::string>::iterator it=store.begin(); it!=store.end(); ++it){
		printf((*it).c_str());
	}
}

#include <Windows.h>

void main() {
	char word[1024] = {'\0'};
	initialize();
	FILE *key_file = fopen("key_words.txt", "r");
	if(key_file == NULL) {
		cout << "open key file error!" << endl;
		return ;
	}

	int i = 0;
	
	while(getline(key_file, word, 1024) >= 0){
	//while(fgets(word, 1024, key_file) >=0){
		insert_word(word);
		++i;

		if (i%100 == 0){
			printf("now handle %i\n", i);
		}
	}

	export_dict("dict.data");
	return;
	load_dict("dict.data");
	
	char tmp[1024];
	while(1){
		printf("please input key :");
		gets(word);
		match_word(word);
		printf("match word %s \n ", word);
	}
}*/
