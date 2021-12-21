#define FHO     0          /* enable/disable the operations of RTF file head */
#define DEBUG   0          /* enable/disable debug mode */
/****************************************************************************/
#define INFO_VER    "0.1"
/****************************************************************************/

/*
        RTF Writer for DOS
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
#define MAX_H       60    /* max characters in a line on-screen */
#define MAX_V       21    /* max lines on-screen */
#define MAX_L       100   /* max lines in-memory */
#define SCR_X       2     /* start position (x) of the edit area */
#define SCR_Y       2     /* start position (y) of the edit area */
#define FHS         "{\rtf1\ansi\pard"

/* notice strings */
#define ERR_INIT1   "[!]  Too many arguments."
#define ERR_LOAD    "[X]  Cannot open the file."
#define ERR_SAVE1   "[X]  Cannot creat temp file."
#define ERR_SAVE2   "[!]  Cannot save as the specified   file. File saved as <save.tmp>."
#define SUC_SAVE    "[V]  File saved."
/* text in the ribbon */
#define HELPLINE " F1:Help     F2:Open     F3:Save     F4:New            RTF Writer for DOS " INFO_VER
/* formatting macros */
#define bold(A)     ((A) = (A) | 1)
#define unbold(A)   ((A) = (A) & (~1))
#define italic(A)   ((A) = (A) | 2)
#define unitalic(A) ((A) = (A) & (~2))
#define isbold(A)   ((A) & 1)
#define isitalic(A) ((A) & 2)
#define blink(A)    
/* colors on-screen */
#define col_default     LIGHTGRAY
#define col_bold        WHITE
#define col_italic      CYAN
#define col_b_i         LIGHTCYAN
#define col_editbkgd    BLUE
#define col_msgbkgd     LIGHTGRAY
#define col_ribbon      BLACK
#define col_msgtext     BLACK
#define col_msghigh     WHITE
typedef struct _chr
{
    char c;               /* content */
    int f;                /* format ...000IB*/
} chr;

/**************************************
        Function Declarations
**************************************/
int backspace();
int c_bold();
int c_italic();
int del(int);
int dispdoc();
int dispwindow();
int end();
int help();
int home();
int left();
int linedown();
int lineup();
int load(char *);
int name();
int newdoc();
int notice(char *);
int phandler(int,int);
int readfile(char *);
int right();
int save(char *);
int scoll(int);
int writefile(char *);

/**************************************
        Global Variables
**************************************/
chr doc[MAX_L][MAX_H];      /* the document in the memory */
char info[1000];            /* infomation of the documemt */
char docpath[100];          /* path of the current document */
chr *chead;                 /* head pointer of cursor on screen */
chr *ctail;                 /* tail pointer of cursor on screen */
int pheadline;              /* head line number of the current page */





int main(int argc, char *argv[])
{
    dispwindow();
    if (argc == 1)
        newdoc();
    else if (argc > 2)
    {
        notice(ERR_INIT1);
        newdoc();
    }
    else
        strcpy(docpath, argv[1]);
    load(docpath);
    edit();

}
/**************************************
        Module: Editor
**************************************/
/* Editor main loop */
int edit()
{
    union k
    {
        char ch[2];
        unsigned i;
    }key;

    do{
        key.i=bioskey(0);
        if(!key.ch[0])
            switch(key.ch[1])
            {
                /* Function keys */
                case 0x3B:
                    help();
                    break;
                case 0x3C:
                    name();
                    load(docpath);
                    break;
                case 0x3D:
                    save(docpath);
                    break;
                case 0x3E:
                    newdoc();
                /* Operations */
                case 0x47:
                    home();
                    break;
                case 0x4F:
                    end();
                    break;
                case 0x4B:
                    left();
                    break;
                case 0x4D:
                    right();
                    break;
                case 0x48:
                    lineup();
                    break;
                case 0x50:
                    linedown();
                    break;
                case 0x49: /* PgUp */
                    scoll(MAX_V);
                    break;
                case 0x51:   /* PgDn */
                    scoll(-MAX_V);
                    break;
                case 0x53:   /* Del */
                    del(1);
                    break;
            }
        else
            switch(key.ch[0])
            {
                case '\n':
        
                    break;
                case '\b':   /* Backspace */
                    del(-1);
                    break;
                default:     /* insert into file */
                    
                    break;
            }
    }while(1);
}
    


/* functions */
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

int name()
{

}

/* editor operations */
int scoll(int lines) /* up+ down- */
{
    if (lines > 0)
        if(pheadline > 1)
            pheadline -= lines;
    else
        if(pheadline < MAX_L)
            pheadline -= lines;
    dispdoc();
}

int end()
{
    int x = MAX_H;
    int y = wherey();

    while(doc[pheadline+y][x].c == 0)
        x--;
    gotoxy(x,y);
}

int home()
{
    int x = 1;
    int y = wherey();

    gotoxy(x,y);
}

int left()
{
    int x = wherex();
    int y = wherey();
    int i;

    if(x == 1)
        if(y == 1)
        {
            scoll(1);
            end();
        }
        else
        {
            gotoxy(x,y-1);
            end();
        }
    else
        gotoxy(x-1,y);
}

int right()
{
    int x = wherex();
    int y = wherey();
    int i;
    int c = doc[pheadline+y][x].c;

    if(x == MAX_H || c == '\n')
        if(y == MAX_V)
        {
            scoll(-1);
            home();
        }
        else
        {
            gotoxy(x,y+1);
            home();
        }
    else
        gotoxy(x+1,y);
}   

int lineup()
{
    int x = wherex();
    int y = wherey();
    int i;

    if (y == 1)
        scoll(1);
    else
        gotoxy(x,y-1);
}

int linedown()
{
    int x = wherex();
    int y = wherey();
    int i;

    if (y == MAX_V)
        scoll(-1);
    else
    {
        gotoxy(x,y+1);
        if (doc[pheadline+y+1][x].c == 0)
            end();
    }
}

int del(int n)      /* right+, left- */
{
    
}

/* commands */
int help()
{
    
}

int load(char *path)
{
    if (path[0] != 0)
        if (readfile(path))
            dispdoc();
        else
        {
            notice(ERR_LOAD);
            path[0] = 0;
            dispdoc();
        }
}

int save(char *path)
{
    if (path[0] == '\0')
        name();
    if (writefile(path))
        notice(SUC_SAVE);
}

int newdoc()
{
    docpath[0] = 0;
    doc[0][0].c = -1;
    dispdoc();
}




/**************************************
        Module: Graphical UI
**************************************/
int dispwindow()
{
    window(1,1,80,MAX_V+1);
    textbackground(col_editbkgd);
    clrscr();
    /* ribbon */
    window(2,24,79,25);
    textbackground(BLACK);
    textcolor(WHITE);
    clrscr();
    gotoxy(1,2);
    cputs(HELPLINE);
    
}

int notice(char *content)
{
    int i;
    int  nx, ny;

    nx = wherex();
    ny = wherey();
    window(20,7,61,19);              /*   13*42   */
    textbackground(col_msgbkgd);
    textcolor(col_msgtext);
    clrscr();

    gotoxy(1,1);
    cprintf("\4================ Notice ================\4");
    for(i=2;i<=12;i++)
    {
        gotoxy(1,i);
        putch('|');
        gotoxy(42,i);
        putch('|');
    }
    gotoxy(1,13);
    cprintf(" ----------------------------------------");
    gotoxy(15,10);
    cputs("Press");
    textcolor(col_msghigh);
    cputs(" <Enter>");

    window(23,9,58,17);
    gotoxy(1,1);
    textcolor(col_msgtext);
    cprintf("%s",content);
    gotoxy(18,8);
    textcolor(col_msghigh);
    putch('<');

    while(getch()!= 13);
    window(SCR_X, SCR_Y, SCR_X+MAX_H-1, SCR_Y+MAX_V-1);
    gotoxy(nx,ny);
    return 1;
}

int dispdoc()
{
    int  line, col;
    char cc;
    int  cf;
    int  nx, ny;

    nx = wherex();
    ny = wherey();
    #if DEBUG
        printf("|{%d,%d}|",nx,ny);
    #endif
    window(SCR_X, SCR_Y, SCR_X+MAX_H-1, SCR_Y+MAX_V-1);
    textbackground(col_editbkgd);
    clrscr();
    textcolor(col_default);
    gotoxy(1,1);
    for (line = 0; line < MAX_V ; line++)
        for (col = 0; col < MAX_H; col++)
        {
            cc = doc[line + pheadline][col].c;
            cf = doc[line + pheadline][col].f;
            /* handle formats */
            if (cf == 0) textcolor(col_default);
            if (isbold(cf)) textcolor(col_bold);
            if (isitalic(cf)) textcolor(col_italic);
            if (isbold(cf) && isitalic(cf)) textcolor(col_b_i);
            /* put on the char */
            if (cc != EOF) putch((cc == '\n') ? '\0':cc);
            else
            {
                window(SCR_X, SCR_Y, SCR_X+MAX_H-1, SCR_Y+MAX_V-1);
                gotoxy(nx, ny);
                return 1;
            }
        }
    /*#endif*/
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

    if ((fp = fopen(path, "r")) == NULL)
    {
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
                    case '\n':
                        if (flag_tag_f)
                        {
                            flag_tag = 0;
                            flag_tag_f = 0;
                            goto put_in_mem;
                        }
                        else
                        {
                            *ptag = '\0';
                            ptag = tag;
                            #if FHO
                            #endif
                            if      (strcmp(tag, "b") == 0)
                                bold(f);
                            else if (strcmp(tag, "b0") == 0)
                                unbold(f);
                            else if (strcmp(tag, "i") == 0)
                                italic(f);
                            else if (strcmp(tag, "i0") == 0)
                                unitalic(f);
                            else if (strcmp(tag, "par") == 0)
                                    c = '\n';
                                    goto put_in_mem;
                            if (c != '\\') flag_tag = 0;
                        }
                        break;
                    default:
                        *ptag = c;
                        ptag++;
                        flag_tag_f = 0;
                        break;
                }
            }
            else
put_in_mem:
                switch (c)
                {
                    case '\n':
                        p->c = '\n';
                        for (++p; (p - doc[nline]) < MAX_H; ++p)    /* fill the whole line with zero after '\n' */
                            p->c = 0;
                        p = doc[++nline];
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
                        nline = (p - doc[0] + 1) / MAX_H;
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
        fputs(FHS,fp);
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
        fputc('}',fp);
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
