#define FHO     0          /* enable/disable the operations of RTF file head */
#define DEBUG   1          /* enable/disable debug mode */
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
#include <conio.h>

/**************************************
        Constants and Types
**************************************/
#define MAX_H       75    /* max characters in a line on-screen */
#define MAX_V       20    /* max lines on-screen */
#define MAX_L       100  /* max lines in-memory */
/* notice strings */
#define ERR_INIT1   "[!]  Too many arguments."
#define ERR_LOAD    "[X]  Cannot open the file."
#define ERR_SAVE1   "[X]  Cannot creat temp file."
#define ERR_SAVE2   "[!]  Cannot save as the specified file. File saved as <save.tmp>."
#define SUC_SAVE    "[V]  File saved."
/* formatting macros */
#define bold(A)     ((A) = (A) | 1)
#define unbold(A)   ((A) = (A) & (~1))
#define italic(A)   ((A) = (A) | 2)
#define unitalic(A) ((A) = (A) & (~2))
#define isbold(A)   ((A) & 1)
#define isitalic(A) ((A) & 2)
/* colors on-screen */
#define col_default     LIGHTGRAY
#define col_bold        WHITE
#define col_italic      CYAN
#define col_b_i         LIGHTCYAN
#define col_editbkgd    BLUE
#define col_msgbkgd     DARKGRAY
#define col_ribbon      BLACK
typedef struct _chr
{
    char c;               /* content */
    int f;                /* format ...000IB*/
} chr;

/**************************************
        Function Declarations
**************************************/
int c_bold();
int c_italic();
int dispdoc();
int dispwindow();
int load(char *);
int name();
int notice(char *);
int readfile(char *);
int save(char *);
int writefile(char *);

/**************************************
        Global Variables
**************************************/
chr doc[MAX_H][MAX_L];      /* the document in the memory */
char info[1000];            /* infomation of the documemt */
char docpath[100];          /* path of the current document */
chr *chead;                 /* head pointer of cursor on screen */
chr *ctail;                 /* tail pointer of cursor on screen */
int pheadline;             /* head line number of the current page */





int main(int argc, char *argv[])
{
    dispwindow();
    if (argc == 1)
        docpath[0] = 0;
    else if (argc > 2)
    {
        notice(ERR_INIT1);
        docpath[0] = 0;
    }
    else
        strcpy(docpath, argv[1]);
    #if DEBUG
    puts(docpath);
    #endif
    load(docpath);
    getch();
}
/**************************************
        Module: Editor
**************************************/
/* Editor main loop */
int edit()
{
    char key;
    do
    {

    }while(1);
}


/* edit functions */
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
    if (path[0] != 0)
        if (readfile(path))
            dispdoc();
}

int save(char *path)
{
    if (path[0] == '\0')
        name();
    if (writefile(path))
        notice(SUC_SAVE);
}

int name()
{

}

/**************************************
        Module: Graphical UI
**************************************/
int dispwindow()
{
    
    
}

int notice(char *content)
{
    window(21,9,60,10);
    textbackground(LIGHTGRAY);
    textcolor(BLACK);
    clrscr();
    gotoxy(1,1);
    
    cputs(content);
}

int dispdoc()
{
    int line, col;
    char cc;
    int  cf;

    window(2, 2, 2+MAX_H, 2+MAX_V);      /* !!!change document area here!!! */
    textbackground(col_editbkgd);
    clrscr();
    textcolor(col_default);
    gotoxy(1,1);
    #if DEBUG
    for (line = 0; line < MAX_V; line++)
        for (col = 0; col < MAX_H; col++)
            {putch(doc[line][col].c); printf("<%d>",doc[line][col].f);}
    #else
    for (line = 0; line < MAX_V; line++)
        for (col = 0; col < MAX_H + 1; col++)
        {
            cc = doc[line + pheadline][col].c;
            cf = doc[line + pheadline][col].f;
            if (isbold(cf)) textcolor(col_bold);
            if (isitalic(cf)) textcolor(col_italic);
            if (isbold(cf) && isitalic(cf)) textcolor(col_b_i);
            if (cc != EOF) putch((cc == '\n') ? '\0':cc);
            else return 1;
        }
    #endif
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

    #if DEBUG
        puts(path);
    #endif
    if ((fp = fopen(path, "r")) == NULL)
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
                        for (++p; p - doc[nline] < MAX_H; ++p)    /* fill the whole line with zero after '/n' */
                            p->c = 0;
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
        fclose(tmppath);
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

