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
            Group 23
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
#define HELPLINE " F1:Help   F2:Open   F3:Save   F4:New   F10:Quit      RTF Writer for DOS " INFO_VER
/* formatting macros */
#define bold(A)     ((A) = (A) | 1)
#define unbold(A)   ((A) = (A) & (~1))
#define italic(A)   ((A) = (A) | 2)
#define unitalic(A) ((A) = (A) & (~2))
#define isbold(A)   ((A) & 1)
#define isitalic(A) ((A) & 2)
#define blink(A)
/* other operation macros */
#define editwin()       (window(SCR_X, SCR_Y, SCR_X+MAX_H-1, SCR_Y+MAX_V))
#define ribbonwin()     (window(1,24,79,25))
/* colors on-screen */
#define col_default     LIGHTGRAY
#define col_bold        WHITE
#define col_italic      CYAN
#define col_b_i         LIGHTCYAN
#define col_editbkgd    BLUE
#define col_msgbkgd     LIGHTGRAY
#define col_ribbontext  WHITE
#define col_ribbonbkgd  BLACK
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
int chrcpy(chr *, chr *);
int del(int);
int dispdoc();
int dispwindow();
int end();
int help();
int home();
int ins(char);
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
int isnewdoc;               /* whether the editing document is first created */
int endline;                /* the end line number of the current document (in-memory) */
int isins;                  /* mode: Insert=1, Overwrite=0 */





int main(int argc, char *argv[])
{
    editwin();
    gotoxy(1,1);
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

    int nx,ny;
    union _key
    {
        char ch[2];
        unsigned i;
    }key;

    do
    {
        /*
         * Ribbon Infomation, part of the Graphical UI Module
         */
        nx = wherex();
        ny = wherey();
        dispribbon();
        /* Line, Col */
        gotoxy(60,1);
        printf("Line %d | Col %d",ny,nx);
        /* Format Indicator */
        if (chead == ctail)
        {
            if (isbold(chead->f))
            {
                gotoxy(5,1);
                textbackground(col_ribbonbkgd);
                textcolor(col_bold);
                putch('B');
            }
            if (isitalic(chead->f))
            {
                gotoxy(7,1);
                textcolor(col_italic);
                putch('I');
            }
        }
        editwin();
        gotoxy(nx,ny);
        /*
         * End if Ribbon Info
         */

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
                    break;
                case 0x44:
                    exit(0);
                    break;
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
                case '\b':   /* Backspace */
                    del(-1);
                    break;
                default:     /* put into document */
                    ins(key.ch[0]);
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
    int r;

    if (lines > 0)
    {
        if(pheadline - lines >= 0)
        {
            pheadline -= lines;
            phandler(wherex(),wherey());
            r = 1;
        }
        else
        {
            pheadline = 0;
            r = 0;
        }
    }
    else
    {
        if(pheadline - lines + MAX_V <= MAX_L && pheadline - lines + MAX_V <= endline)
        {
            pheadline -= lines;
            phandler(wherex(),wherey());
            r = 1;
        }
        else
        {
            pheadline = endline - MAX_V;
            r = 0;
        }
    }
    dispdoc();
    return r;
}

int end()
{
    int x = MAX_H;
    int y = wherey();

    while(doc[pheadline+y-1][x-1].c <= 0)
        x--;
    gotoxy(x,y);
    phandler(x,y);
}

int home()
{
    int x = 1;
    int y = wherey();

    gotoxy(x,y);
    phandler(x,y);
}

int left()
{
    int x = wherex();
    int y = wherey();
    int i;

    if(x == 1)
        if(y == 1)
        {
            if(scoll(1))
                end();
        }
        else
        {
            gotoxy(x,y-1);
            end();
        }
    else
        gotoxy(x-1,y);
        phandler(x-1,y);
}

int right()
{
    int x = wherex();
    int y = wherey();
    int i;
    int c = doc[pheadline+y-1][x-1].c;

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
        if(c != EOF)
        {
            gotoxy(x+1,y);
            phandler(x+1,y);
        }
}   

int lineup()
{
    int x = wherex();
    int y = wherey();
    int i;

    if (y == 1)
        scoll(1);
    else
    {
        gotoxy(x,y-1);
        phandler(x,y-1);
        if (doc[pheadline+y-2][x-1].c == 0)
            end();
    }
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
        if (pheadline + y <= endline)
        {
            gotoxy(x,y+1);
            phandler(x,y+1);
            if (doc[pheadline+y][x-1].c == 0)
                end();
        }
    }
}

int del(int n)      /* right+, left- */
{
    
}

int ins(char c)
{
    chr *pt;
    chr *ph;
    chr t1, t2;
    int flag;       /* if in blank area */
    if (ctail == chead)
    {
        chrcpy(&t1, chead);
        for (pt = ctail+1; pt->c != EOF; pt++)
        {
            chrcpy(&t2,pt);
            chrcpy(pt,&t1);
            chrcpy(&t1,&t2);
            if (t1.c == '\n')
            {
                pt = (char*)doc[(pt-(char*)doc) / MAX_H + 1];
                ph = (char*)doc[(ph-(char*)doc) / MAX_H + 1];
            }

        }
    }
    ph->c = EOF;
    chead->c = c;
    chead++;
    ctail = chead;
    gotoxy((chead-doc)%MAX_H+1, (chead-doc)/MAX_H+1);
    dispdoc();
}

int chrcpy(chr *to, chr *from)
{
    to->c = from->c;
    to->f = from->f;
}

int phandler(int x, int y)
{
    chead = (chr*)doc + x-1 + (pheadline+y-1)*MAX_H;
    ctail = (chr*)doc + x-1 + (pheadline+y-1)*MAX_H;
}

/* commands */
int help()
{
    
}

int load(char *path)
{
    if (path[0] != 0)
        if (readfile(path))
        {
            pheadline = 0;
            chead = ctail = doc;
            dispdoc();
        }
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
    isnewdoc = 1;
}




/**************************************
        Module: Graphical UI
**************************************/
int dispwindow()
{
    int  nx, ny;

    nx = wherex();
    ny = wherey();
    window(1,1,80,MAX_V+2);
    textbackground(col_editbkgd);
    clrscr();
    dispribbon();
    editwin();
    gotoxy(nx, ny);
}

int dispribbon()
{
    ribbonwin();
    textbackground(col_ribbonbkgd);
    textcolor(col_ribbontext);
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
    editwin();
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
    /*#if DEBUG
        window(1,1,80,25);
        gotoxy(1,1);
        printf("|{%d,%d}|",nx,ny);
    #endif*/
    editwin();
    textbackground(col_editbkgd);
    clrscr();
    textcolor(col_default);
    gotoxy(1,1);
    /*#if DEBUG
        line = 0;
        for (col = 0;col < MAX_H; col++)
            cprintf("<%c|%d>",doc[line + pheadline][col].c,doc[line + pheadline][col].f);
    #endif*/
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
            if (cc != EOF)
                putch((cc == '\n') ? '\0':cc);
            else
                goto end_of_dispdoc;
        }
end_of_dispdoc:
    #if DEBUG
    window(1,1,80,25);
    gotoxy(1,1);
    printf("|{%d,%d}|",nx,ny);
    #endif
    editwin();
    gotoxy(nx, ny);
    return 1;
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
                            {
                                p->c = '\n';
                                for (++p; (p - doc[nline]) < MAX_H; ++p)    /* fill the whole line with zero after '\n' */
                                p->c = 0;
                                p = doc[++nline];
                            }
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
                        #if FHO
                        #else
                        if (flag_block == 0)
                        {
                            p->c = c;
                            p->f = f;
                            p++;
                            nline = (p - doc[0] + 1) / MAX_H;
                        }
                        break;
                        #endif
                }
        }
    p->c = EOF;
    endline = nline;
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
    char tag[100];    /* tag to be inserted */

    tag[0]=0;
    remove(tmppath);
    if ((fp = fopen(tmppath, "w")) == NULL)
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
                strcat(tag, "\b");
            if (!isbold(p->f) && isbold(lf))
                strcat(tag,"\b0");
            if (isitalic(p->f) && !isitalic(lf))
                strcat(tag, "\i");
            if (!isitalic(p->f) && isitalic(lf))
                strcat(tag, "\i0");
            if (tag[0] != 0)
                strcat(tag, " ");
            fputs(tag, fp);
            tag[0] = 0;
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
