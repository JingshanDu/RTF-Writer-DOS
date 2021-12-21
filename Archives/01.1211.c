#define FHO = 0          /* enable/disable the operations of RTF file head */
/****************************************************************************/


/*
		RTF Writer for DOS
			version 0.1
(c)2011 Du Jingshan, Fang Mingtong, Li Yue
Chu Kochen Honors College of Zhejiang University
Course Project of The C Programming Language
*/
/**************************************
		Include Files
**************************************/
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>

/**************************************
		Constants and Types
**************************************/
#define MAX_H 70    /* max characters in a line on-screen */
#define MAX_V 20    /* max lines on-screen */
/* notice strings */
#define ERR_INIT1 "[!]  Too many arguments. A new document created."
#define ERR_LOAD  "[X]  Cannot open the file."
#define ERR_SAVE1 "[X]  Cannot creat temp file."
#define ERR_SAVE2 "[!]  Cannot save as the specified file. File saved as <save.tmp>."
/* formatting macros */
#define bold(A)     ((A) = (A) | 1)
#define unbold(A)   ((A) = (A) & (~1))
#define italic(A)   ((A) = (A) | 2)
#define unitalic(A) ((A) = (A) & (~2))
#define isbold(A)   ((A) & 1)
#define isitalic(A) ((A) & 2)

typedef struct _chr
{
	char c;               /* content */
	int f;                /* format ...000IB*/
} chr;

/**************************************
		Function Declarations
**************************************/
int notice(char *);
int readfile(char *);
int writefile(char *);
int load();
int save();
int dispdoc();
int dispwindow();

/**************************************
		Variables
**************************************/
chr doc[MAX_H][MAX_V];      /* the document in the memory */
char info[1000];            /* infomation of the documemt */
char docpath[100];          /* path of the currect document */
char *shead;                /* head pointer on screen */
char *stail;                /* tail pointer on screen */





int main(int argc, char *argv[])
{
	diswindow();
	if (argc == 1)
	{
	    doc[0][0] = EOF;
	    docpath = "";
	}
	else if (argc > 2)
	{
		notice(ERR_INIT1);
		doc[0][0] = EOF;
	    docpath = "";
	}
	else
	{
		strcpy(docpath, argv[2]);
		load(docpath);
	}
}
/**************************************
		Module: File I/O
**************************************/
int readfile(char *path)
{
	FILE *fp;
	char *p = doc;
	char c;
	char *tag;
	char *ptag = tag;
	int  f;       /* the same as _chr.f */
	int  nline = 0;
	int  flag_tag = 0;
	int  flag_tag_f = 0;     /* is it the first character in a tag */
	int  flag_block = 0;    /* store the number of block layers of {} */

	
	if ((fp = fopen(path, "wt+")) == NULL)
	{
		notice(ERR_LOAD);
		return 0;
	}
	else
	{
		while ((c = fgetc(fp)) != EOF)
		{
			if (flag_tag == 1)
			{
				switch (c)
				{
					case '\': case '{': case '}': case ' ':
						if (flag_tag_f)
						{
                        	flag_tag = 0;
							flag_tag_f = 0;
							goto put_in_mem;
						}
						else
						{
							*(ptag+1) = '\0';
							ptag = tag;
							switch (tag)           /* !!!Process tags here!!! */
							{
								#if FHO
								#endif
								case "b":
									bold(f);
									break;
								case "b0":
									unbold(f);
									break;
								case "i":
									italic(f);
									break;
								case "i0":
									unitalic(f);
									break;
							}
						}
						break;
					default:
						*ptag = c;
						ptag++;
						break;
				}
			}
			else
				switch (c)
				{
					case '\n':
						p->c = '\n';
						nline += 1;
						p = doc[nline];
						break;
					case '\':
						flag_tag = 1;
						flag_tag_f = 1;
						break;
					case '{':
						flag_block += 1;
						break;
					case '}':
						flag_block -= 1;
						break;
					default:
                        p->c = c;
                        p->f = f;
                        p++;
                        nline = (p - doc - 1) / MAX_H;		/* determine line num */
                        break;
				}
		}
	*p.c = EOF;
	fclose(fp);
	return 1;
	}
}

int writefile(char *path)
{
	FILE *fp;
	char tmppath[] = "save.tmp";
	char *p = doc;
	int lf = 0;       /* last format */
	remove(tmppath);
	if ((fp = fopen(tmppath, "wt+")) == NULL)
	{
		notice(ERR_SAVE);
		return 0;
	}
	else
	{
		while(p->c != EOF)
		{
			if (isbold(p->f) && !isbold(lf))
				fputs("\b ",fp);
            if (!isbold(p->f) && isbold(lf))
                fputs("\b0 ",fp);
            if (isitalic(p->f) && !isitalic(lf))
				fputs("\i ",fp);
            if (!isitalic(p->f) && isitalic(lf))
                fputs("\i0 ",fp);
			fputc(p->f, fp);
			if (p->c == '\n')
			    p = doc[(p - 1) / MAX_H + 1];
			else
				p++;
		}
	return 1;
	}
}
