#include "DoubleArrayTrie.h"


#define MIN_CODE 1
#define MAX_CODE 255
#define BC_INC 10240
#define TAIL_INC 102400
#define TEMP_INC 1024
#define TOKEN_CHAR 2

char *mem_str(char *area_name, int *max, int init) 
{
	*max = init;
	char *area = (char*)malloc(sizeof(char) * (*max));
	if(area == NULL) {
		printf("%s malloc error! \n");
	}

	memset(area, *max, '\0');
	return area;
}

char *realloc_str(char *area_name, char *area, int *max, int inc) 
{
	int pre_size;
	int i;
	pre_size = *max;
	*max += inc;
	area = (char*) realloc(area, sizeof(char) * (*max));
	if(area == NULL) {
		printf("%s realloc error! \n", area_name);
		exit(-1);
	}
	for(i = pre_size; i < *max; i++) {
		area[i] = '\0';
	}

	printf("%s  realloc ok! \n", area_name);
	return area;
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


DoubleArrayTrie::DoubleArrayTrie()
{
	m_bcArray = NULL;
	m_tail = NULL;
	m_tmp = NULL;
	m_bcPos = m_tailPos = 0;
	m_bcMax = m_tailMax = m_tmpMax = 0;
	initialize();
}

DoubleArrayTrie::~DoubleArrayTrie()
{
	release();
}

void DoubleArrayTrie::release()
{
	if (m_tail) free(m_tail);
	if (m_tmp) free(m_tmp);
	if (m_bcArray) free(m_bcArray);

	m_bcArray = NULL;
	m_tail = NULL;
	m_tmp = NULL;
	m_bcPos = m_tailPos = 0;
	m_bcMax = m_tailMax = m_tmpMax = 0;
}

void DoubleArrayTrie::initialize()
{
	m_bcMax = BC_INC * 300;
	m_bcPos = m_tailPos = 1;
	m_bcArray = (int*)malloc(sizeof(int) * 2 * m_bcMax);
	
	if(m_bcArray == NULL) {
		printf("BC malloc error! \n");
		return;
	}

	memset(m_bcArray, 0, 2*m_bcMax);
	
	w_base(1, 1);
	m_bcPos = 1;

	m_tail = mem_str("TAIL", &m_tailMax, TAIL_INC);
	m_tail[0] = TOKEN_CHAR;
	m_tmp = mem_str("TEMP", &m_tmpMax, TEMP_INC);
}


void DoubleArrayTrie::realloc_bc()
{
	int i, pre_bc;
	pre_bc = m_bcMax;
	m_bcMax += BC_INC;
	m_bcArray =(int*)realloc(m_bcArray, sizeof(int) * 2 * m_bcMax);
	if(m_bcArray == NULL) {
		printf("realloc bc error!\n");
		return;
	}
	for(i = 2 * pre_bc; i < 2 * m_bcMax; i++) {
		m_bcArray[i] = 0;
	}
}

void DoubleArrayTrie::separate(int s, char *b, int tail_pos)
{
	int t = base(s) + (unsigned char)(*b); // change
	b++;
	w_check(t, s);
	w_base(t, (-1) * tail_pos);
	write_tail(b, tail_pos);
}

int DoubleArrayTrie::change_bc(int current, int s, char *list, char ch)
{
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
			while(k - base(old_node) <= MAX_CODE || k < m_bcPos) {
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

void DoubleArrayTrie::read_tail(int p) 
{
	int i = 0;
	while(m_tail[p] != TOKEN_CHAR){ 
		m_tmp[i++] = m_tail[p++];
	}
	m_tmp[i++] = TOKEN_CHAR;
	m_tmp[i] = '\0';
}

void DoubleArrayTrie::read_tail(int p, char* tmp, int size)
{
	int i = 0;
	while(m_tail[p] != TOKEN_CHAR && i < size){ 
		tmp[i++] = m_tail[p++];
	}

	if (i == size)
		i = 0;

	tmp[i++] = TOKEN_CHAR;
	tmp[i] = '\0';
}

void DoubleArrayTrie::write_tail(char *temp, int p) 
{
	int i = 0; 
	int tail_index;

	tail_index = p;
	while((p + strlen(temp)) >= m_tailMax - 1) {
		m_tail = realloc_str("TAIL", m_tail, &m_tailMax, TAIL_INC);
	}

	if (temp[0] == 0){
		m_tail[tail_index] = TOKEN_CHAR;
	}else{
		while(*(temp + i) != '\0') {
			m_tail[tail_index++] = *(temp + i);
			i++;
		}
	}

	if(p + i + 1 > m_tailPos) {
		m_tailPos = p + i;
	}
}

char* DoubleArrayTrie::set_list(int s)
{
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

void DoubleArrayTrie::tail_insert(int s, char *a, char *b)
{
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
	separate(s, b + length, m_tailPos);
}

void DoubleArrayTrie::bc_insert(int s, char *b) 
{
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

	separate(s, b, m_tailPos);
}

int DoubleArrayTrie::insert_word(char *p_word) 
{
	unsigned char ch;
	int h = -1;
	int s = 1;
	int t;

	char buf[2] = {0};
	buf[0] = TOKEN_CHAR;

	strcat(p_word, buf);
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

	if(p_word[h] != TOKEN_CHAR)
		read_tail((-1) * base(t));
	if(p_word[h] == TOKEN_CHAR || strcmp(m_tmp, p_word + h + 1) == 0) {
		return 1;
	} 
	if(base(t) != 0) {
		tail_insert(t, m_tmp, p_word + h + 1);
	}
	return 1;
}

// if found word, return its base index, if not, return -1
int DoubleArrayTrie::search_word(char *p_word)
{ 
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
	if(p_word[h] != TOKEN_CHAR)
		read_tail((-1) * base(t));
	if(p_word[h] == TOKEN_CHAR || strcmp(m_tmp, p_word + h + 1) == 0) {
		return t;
	} else {
		return -1;
	}
}

// if delete given word, return 1, else return 0
int DoubleArrayTrie::delete_word(char *p_word) 
{ 
	int t = search_word(p_word);
	if(t = -1) {
		return 0;
	} else {
		w_base(t, 0);
		w_check(t, 0);
		return 1;
	}
}


int DoubleArrayTrie::check_match_word(char* p_word)
{
	unsigned char ch;
	int h = 0;
	int s = 1;
	int t, t1;
	int m = 0;
	char tmp[2048] = {0};

	while(*(p_word+h)){
		ch = p_word[h];

		t = base(s) + (unsigned char)(ch);
		if(check(t) != s) 
			return m;

		if(base(t) < 0) {
			break;
		}

		t1 = base(t) + (unsigned char)(TOKEN_CHAR);
		if (base(t1) < 0 && check(t1) == t)
			m = h+1;

		s = t;
		++h;
	}

	if (*(p_word+h) == 0){
		return m;
	}

	if (base(t) < 0)
		read_tail((-1) * base(t), tmp, 2048);

	++h;
	int j = 0;
	while(p_word[h]!=0 && tmp[j]!=TOKEN_CHAR){
		if (p_word[h] == tmp[j]){
			++h;
			++j;
		}else{
			return m;
		}
	}

	if (tmp[j] != TOKEN_CHAR) return m;

	return h;
}

void DoubleArrayTrie::match_word(char* p_word)
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

void DoubleArrayTrie::export_dict(char* path)
{
	FILE* f = fopen(path, "wb");
	if (f == NULL){
		printf("open file %s failed, export falied\n", path);
		return;
	}

	fprintf(f, "%u\n", m_bcPos);    
	fprintf(f, "%u\n", m_tailPos);
	fprintf(f, "%u\n", m_bcMax);
	fprintf(f, "%u\n", m_tailMax);
	fprintf(f, "%u\n", m_tmpMax);
	//write BC data
	fwrite(m_bcArray, m_bcMax*sizeof(int), 1, f);
	//write TAIL data
	fwrite(m_tail, m_tailMax, 1, f);
	fclose(f);
}


void DoubleArrayTrie::load_dict(char* path)
{
	FILE* f = fopen(path, "rb");
	if (f == NULL){
		printf("open file %s failed, load dict falied\n", path);
		return;
	}

	//local BC_POS
	char buf[1024];
	if (getline(f, buf, 1024) < 0)
		goto FAILED;
	m_bcPos = atoi(buf);

	if (getline(f, buf, 1024) < 0)
		goto FAILED;
	m_tailPos = atoi(buf);


	if (getline(f, buf, 1024) < 0)
		goto FAILED;
	m_bcMax = atoi(buf);


	if (getline(f, buf, 1024) < 0)
		goto FAILED;
	m_tailMax = atoi(buf);

	if (getline(f, buf, 1024) < 0)
		goto FAILED;
	m_tmpMax = atoi(buf);

	m_bcArray = (int*)malloc(m_bcMax*sizeof(int));
	int  r  = fread(m_bcArray,sizeof(int),m_bcMax, f);
	if (r != m_bcMax)
		goto FAILED;

	m_tail = (char*)malloc(m_tailMax);
	r = fread(m_tail, 1,m_tailMax, f);
	if (r != m_tailMax)
		goto FAILED;

	fclose(f);
	m_tmp = mem_str("TEMP", &m_tmpMax, TEMP_INC);
	return;

FAILED:
	printf("open file %s failed, load dict falied\n", path);
	release();
	initialize();
	return;
}
