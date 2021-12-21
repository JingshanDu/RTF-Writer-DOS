#define FHO 0          /* enable/disable the operations of RTF file head */
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
#define ERR_INIT1 "[!]  Too many arguments."
#define ERR_LOAD  "[X]  Cannot open the file."
#define ERR_SAVE1 "[X]  Cannot creat temp file."
#define ERR_SAVE2 "[!]  Cannot save as the specified file. File saved as <save.tmp>."
#define SUC_SAVE  "[V]  File saved."
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
int load(char *);
int save();
int name();
int dispdoc();
int dispwindow();
int c_bold();
int c_italic();

/**************************************
        Global Variables
**************************************/
chr doc[MAX_H][MAX_V];      /* the document in the memory */
char info[1000];            /* infomation of the documemt */
char docpath[100];          /* path of the current document */
chr *chead;                 /* head pointer of cursor on screen */
chr *ctail;                 /* tail pointer of cursor on screen */
chr *phead;                 /* head pointer of the current page */





int main(int argc, char *argv[])
{
    dispwindow();
    if (argc == 1)
        strcpy(docpath,"");
    else if (argc > 2)
    {
        notice(ERR_INIT1);
        strcpy(docpath,"");
    }
    else
        strcpy(docpath, argv[2]);
    load(docpath);
}
/**************************************
        Module: Editor
**************************************/




int c_bold()
{
    chr *p;
    for (p = chead; p <= ctail; p++)
        bold(p->f);
    dispdoc();
}

int c_italic()
{
    chr *p;
    for (p = chead; p <= ctail; p++)
        italic(p->f);
    dispdoc();
}

int load(char *path)
{
    if (path[0] != '\0')
        if (readfile(*path))
            dispdoc();
}

int save(char *path)
{
    if (path[0] == '\0')
        name();
    if (writefile(*path))
        notice(SUC_SAVE);
}



/**************************************
        Module: Graphical UI
**************************************/
int dispwindow()
{
    int gdriver = DETECT, gmode;
    initgraph(&gdriver, &gmode, "");
    registerbgidriver(EGAVGA_driver);
    
}

int notice(char *content)
{
    
}

int dispdoc()
{
    int line, col;
    for (phead = ; line < MAX_V; line++)
        for (col = 0; col < MAX_H; col++)
            if (doc[line][col] != EOF)
                    
            else return 1;
}

/**************************************
        Module: File I/O
**************************************/
int readfile(char *path)
{
    FILE *fp;
    chr *p = doc;
    char c;
    char tag[10];
    char *ptag = tag;
    int  f = 0;              /* the same as _chr.f */
    int  nline = 0;
    int  flag_tag = 0;
    int  flag_tag_f = 0;     /* is it the first character in a tag */
    int  flag_block = 0;     /* store the number of block layers of {} */

    
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
                    case '\\':
                    case '{':
                    case '}':
                    case ' ':
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
                            #if FHO
                            #endif
                            if (strcmp(tag, "b") == 0)
                                bold(f);
                            if (strcmp(tag, "b0") == 0)
                                unbold(f);
                            if (strcmp(tag, "i") == 0)
                                italic(f);
                            if (strcmp(tag, "i0") == 0)
                                unitalic(f);
                    	}
                        break;
                    default:
                        *ptag = c;
                        ptag++;
                        break;
                }
            }
            else
put_in_mem:
                switch (c)
                {
                    case '\n':
                        p->c = '\n';
                        nline += 1;
                        p = doc[nline];
                        break;
                    case '\\':
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
                        nline = (p - doc - 1) / MAX_H;      /* determine line num */
                        break;
                }
        }
    p->c = EOF;
    fclose(fp);
    return 1;
    }
}

int writefile(char *path)
{
    FILE *fp;
    char tmppath[] = "save.tmp";
    chr *p = doc;
    int lf = 0;       /* last format */
    remove(tmppath);
    if ((fp = fopen(tmppath, "wt+")) == NULL)
    {
        notice(ERR_SAVE1);
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
                p = doc[(p - doc - 1) / MAX_H + 1];
            else
                p++;
        }
        if(remove(path) == -1)
        {
            notice(ERR_SAVE2);
            return 0;
        }
        if(rename(tmppath, path) == -1)
        {
            notice(ERR_SAVE2);
            return 0;
        }
    return 1;
    }
}
