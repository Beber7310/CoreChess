typedef enum book_conf_t
{
	BOOK_NONE,
	BOOK_NARROW, 
	BOOK_BROAD
}book_conf;

void readBookFile();
void book_addline( int line_no, char * movestring);
int book_add(sboard* pBoard, char* movestring);
int book_present(sboard* pBoard, smove* curr_move);
int book_getMaxFreq(sboard* pBoard);
void book_loadInternal();
smove getBookMove(sboard* pBoard, book_conf book_type);
void initBook();


