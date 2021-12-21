#define FHO             0           /* enable/disable the operations of RTF file head */
#define LFN             1           /* enable/disable an easy Long File Name Support if used in Windows system*/
#define DEBUG           0           /* enable/disable debug mode */
/****************************************************************************/
#define INFO_VER        "0.1"
#define INFO_BUILD      "12"
#define INFO_MS         "Alpha2"
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
#include <string.h>

/**************************************
        Constants and Types
**************************************/
#define MAX_H           60          /* max characters in a line on-screen */
#define MAX_V           21          /* max lines on-screen */
#define MAX_L           350         /* max lines in-memory */
#define SCR_X           2           /* start position (x) of the edit area */
#define SCR_Y           2           /* start position (y) of the edit area */
#define NAM_MAX         50          /* Max length of the inputed filename */
#define FHS             "{\\rtf1\\ansi\\pard "
/* strings on-screen */
#define ERR_INIT1       "[!]  Too many arguments."
#define ERR_LOAD        "[X]  Cannot open the file."
#define ERR_SAVE1       "[X]  Cannot creat temp file."
#define ERR_SAVE2       "[!]  Cannot save as the specified   file. File saved as <save.tmp>."
#define SUC_SAVE        "[V]  File saved."
#define WAR_NOTSAVED    "[!]  Document not saved yet.          \nPress <Enter> to continue or <Esc> to cancel."
#define RIB_LINE        " F1:Help   F2:Open   F3:Save   F4:New   F10:Quit        RTF Writer for DOS " INFO_VER
#define HLP_CON         "Home/End: beginning/end of the line.  PgUp/PgDn: scroll up/down one screen. Shift & move: select characters.      Ctrl+B: Bold; Ctrl+I: Italic.         \n\31: Line Break; \4: End of Document     \n   RTF Writer for DOS 0.1." INFO_BUILD " " INFO_MS
#define NAM_HLP         "Press <Enter> to accept. Leave it blank to cancel."
/* formatting macros */
#define bold(A)         ((A) = (A) | 1)
#define unbold(A)       ((A) = (A) & (~1))
#define italic(A)       ((A) = (A) | 2)
#define unitalic(A)     ((A) = (A) & (~2))
#define isbold(A)       ((A) & 1)
#define isitalic(A)     ((A) & 2)
#define blink(A)
#define kn(SCAN,ASCII)  ((SCAN)<<8 | (ASCII))
/* other operation macros */
#define editwin()       (window(SCR_X, SCR_Y, SCR_X+MAX_H-1, SCR_Y+MAX_V))
#define ribbonwin()     (window(1,24,79,25))
/* colors on-screen */
#define col_default     LIGHTGRAY
#define col_bold        WHITE
#define col_italic      CYAN
#define col_b_i         LIGHTCYAN
#define col_editbkgd    BLUE
#define col_border      WHITE
#define col_msgbkgd     LIGHTGRAY
#define col_ribbontext  WHITE
#define col_ribbonbkgd  BLACK
#define col_msgtext     BLACK
#define col_msghigh     WHITE

typedef struct _chr
{
    char c;               /* character */
    int f;                /* format ...000IB*/
} chr;

/**************************************
        Function Declarations
**************************************/
int     backspace();
int     c_bold();
int     c_italic();
chr *   chrcpy(chr *, chr *);
int     del(int);
int     dispdoc();
int     dispwindow();
int     drawborder(int, int, int, int);
int     edit();
int     end();
int     help();
int     home();
int     ins(char);
int     left();
int     linecpy(chr *,chr *);
int     linedown();
int     lineup();
char *  lfnhandler(char *);
int     load(char *);
int     name();
int     newdoc();
int     notice(char *);
int     phandler(int,int);
int     quit();
int     readfile(char *);
int     removespace(char *);
int     right();
int     save(char *);
int     scroll(int);
int     writefile(char *);

/**************************************
        Global Variables
**************************************/
chr     doc[MAX_L][MAX_H];      /* the document in the memory */
char    info[1000];             /* infomation of the documemt */
char    docpath[100];           /* path of the current document */
chr  *  chead;                  /* head pointer of cursor on screen */
chr  *  ctail;                  /* tail pointer of cursor on screen */
int     headline;               /* head line number of the current page */
int     endline;                /* the end line number of the current document (in-memory) */
int     ifsaved;
int     isins;                  /* mode: Insert=1, Overwrite=0 */
int     isrtf;                  /* whether the document is RTF or plain test */
int     nowformat;              /* store the current format to use */



/*
 ***************************************************************************
 ***************************************************************************
                                PROGRAM ENTRANCE
 ***************************************************************************
 ***************************************************************************
 */

int main(int argc, char *argv[])
{
    editwin();
    gotoxy(1,1);
    dispwindow();
    ifsaved = 1;  /* So that the not-saved notice won't be displayed */
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
    long i,j;
    int ifchangef = 1;
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
        printf("Line%3d | Col%3d",ny+headline,nx);

        /* update nowformat */
        if (ifchangef)
        {
            if (isbold(chead->f))
                bold(nowformat);
            else
                unbold(nowformat);
            if (isitalic(chead->f))
                italic(nowformat);
            else
                unitalic(nowformat);
        }
        ifchangef = 1;
        /* Format Indicator */
        if (isbold(nowformat))
        {
            gotoxy(5,1);
            textbackground(col_ribbonbkgd);
            textcolor(col_bold);
            putch('B');
        }
        if (isitalic(nowformat))
        {
            gotoxy(7,1);
            textcolor(col_italic);
            putch('I');
        }

        editwin();
        gotoxy(nx,ny);
        /*
         * End of Ribbon Info
         */

        /* update endline number */
        for (j = 0; j < MAX_L; j++)
            for (i = 0; i < MAX_H; i++)
                if(doc[j][i].c == EOF)
                {
                    endline = j;
                    break;
                }
        /*
         * Detecting input and operate
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
                    if(!ifsaved)
                    {
                        if(notice(WAR_NOTSAVED) != -1)
                            quit();
                    }
                    else quit();
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
                    scroll(MAX_V);
                    break;
                case 0x51:   /* PgDn */
                    scroll(-MAX_V);
                    break;
                case 0x53:   /* Del */
                    del(0);
                    ifsaved = 0;
                    break;
            }
        else
            switch(key.i)
            {
                case kn(0x0E,0x08):   /* Backspace */
                    del(-1);
                    ifsaved = 0;
                    break;
                case kn(0x30,0x02):
                    c_bold();
                    ifchangef = 0;
                    ifsaved = 0;
                    break;
                case kn(0x17,0x09):
                    c_italic();
                    ifchangef = 0;
                    ifsaved = 0;
                    break;
                default:
                    /* put into document */
                    ins(key.ch[0]);
                    ifchangef = 0;
                    ifsaved = 0;
                    break;
            }
    }while(1);
}



/* functions */
int c_bold()
{
    chr *p;

    if(isbold(nowformat))
    {
        for (p = chead; p < ctail; p++)
            unbold(p->f);
        unbold(nowformat);
    }
    else
    {
        for (p = chead; p < ctail; p++)
            bold(p->f);
        bold(nowformat);
    }
    dispdoc();
}

int c_italic()
{
    chr *p;

    if(isitalic(nowformat))
    {
        for (p = chead; p < ctail; p++)
            unitalic(p->f);
        unitalic(nowformat);
    }
    else
    {
        for (p = chead; p < ctail; p++)
            italic(p->f);
        italic(nowformat);
    }
    dispdoc();
}

/* editor operations */
int scroll(int lines) /* up+ down- */
{
    int r;

    if (lines > 0)
    {
        if(headline - lines >= 0)
        {
            headline -= lines;
            phandler(wherex(),wherey());
            r = 1;
        }
        else
        {
            headline = 0;
            r = 0;
        }
    }
    else
    {
        if(headline - lines + MAX_V <= MAX_L && headline - lines + MAX_V - 1 <= endline)
        {
            headline -= lines;
            phandler(wherex(),wherey());
            r = 1;
        }
        else
        {
            headline = endline - MAX_V + 1;
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

    while(doc[headline + y - 1][x - 1].c <= 0)
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
            if(scroll(1))
                end();
        }
        else
        {
        gotoxy(x,y - 1);
            end();
        }
    else
    {
    gotoxy(x - 1,y);
    phandler(x - 1,y);
    }
}

int right()
{
    int x = wherex();
    int y = wherey();
    int i;
    int c = doc[headline + y - 1][x - 1].c;

    if(x == MAX_H || c == '\n')
        if(y == MAX_V)
        {
            if(scroll(-1))
                home();
        }
        else
        {
        gotoxy(x,y + 1);
            home();
        }
    else
        if(c != EOF)
        {
        gotoxy(x + 1,y);
        phandler(x + 1,y);
        }
}

int lineup()
{
    int x = wherex();
    int y = wherey();
    int i;

    if (y == 1)
        scroll(1);
    else
    {
    gotoxy(x,y - 1);
    phandler(x,y - 1);
    if (doc[headline +y - 2][x - 1].c == 0)
            end();
    }
}

int linedown()
{
    int x = wherex();
    int y = wherey();
    int i;

    if (y == MAX_V)
        scroll(-1);
    else
    {
        if (headline + y <= endline)
        {
        gotoxy(x,y + 1);
        phandler(x,y + 1);
        if (doc[headline + y][x - 1].c == 0)
                end();
        }
    }
}

int del(int n)      /* (del)0, (bksp)left- */
{
    chr *pt;
    chr *ph;
    chr tl[MAX_H];
    int l,i;
    int flag = 1;       /* = 0 if has reached and end of a paragraph */

    /* initialization */
    if (ctail == doc)   return;
    if (chead == ctail)
    {
        pt = ctail + n + 1;
        ph = chead + n;
    }
    else
    {
        pt = ctail + 1;
        ph = chead;
    }
    /* operation */
    while(pt->c != EOF && flag)
    {
            chrcpy(ph, pt);
            if (pt->c == '\n')
            {
                if ((ph + 1 - (chr*)doc) % MAX_H == 0)
                    for (l = (pt + 1 - (chr*)doc) / MAX_H; !isendline(l); l++) /* move up one line */
                        linecpy(doc[l], doc[l+1]);
                for (i = 1; (ph - (chr*)doc + i) % MAX_H != 0; i++)    /* fill the remaining of the line with zero */
                {
            (ph + i)->c = 0;
                }
                flag = 0;
            }
        pt++;
        ph++;
    }
    if (flag)
        chrcpy(ph, pt);
    /* cursor position */
    ctail = chead;
    if (n == 0)
    gotoxy((chead - (chr*)doc) % MAX_H + 1, (chead - (chr*)doc) / MAX_H + 1 - headline);
    else
    {
        /* handle if cursor is out of the edit area #2 */
    if ((chead - (chr*)doc) % MAX_H == 0 && (chead - (chr*)doc) / MAX_H + 1 - headline <= 0)
        {
            headline--;
            dispdoc();
        gotoxy(1, (chead - (chr*)doc) /MAX_H +1 - headline);
            end();
            return;
        }
        else
        gotoxy((chead - (chr*)doc) % MAX_H, (chead - (chr*)doc) / MAX_H + 1 - headline);
        chead--;
        ctail = chead;
    }
    dispdoc();

}

int ins(char c)
{
    chr *pt;
    chr *ph;
    chr t1, t2, tx;
    chr tl1[MAX_H], tl2[MAX_H];
    int l;
    int flag = 1;       /* = 0 if has reached and end of a paragraph */
    int i;
    int flagn = 0;

    if (chrcpy(&tx, chead)->c == EOF)
    {
        if (c == '\r')
        {
            c = '\n';
            flagn = -1;
            pt = chead;
            goto line_move_down;
        }
        else
        {
            (chead + 1)->c = chead->c;
            goto end_of;
        }
    }

    if (c == '\r')
    {
        c = '\n';
        flagn = 1;
        pt = ctail;
        goto line_move_down;
    }

    if (ctail != chead)
        del(0);


    chrcpy(&t1, chead);
    if ((chead+1)->c == 0)      /* cursor at a line break */
    {
        pt = ctail;
        goto meet_lb;
    }
    for (pt = ctail + 1; pt->c != EOF && flag; pt++)
    {
        chrcpy(&t2,pt);
        chrcpy(pt,&t1);
        chrcpy(&t1,&t2);
        if (t1.c == '\n')
        {
meet_lb:
            if ((pt + 1 - (chr*)doc) % MAX_H == 0)
            {
line_move_down:
                for ((l = (pt - (chr*)doc) / MAX_H, linecpy(tl1, doc[l])); !isendline(l); l++) /* move down one line */
                {
                    linecpy(tl2, doc[l + 1]);
                    linecpy(doc[l + 1], tl1);
                    linecpy(tl1,tl2);
                }
                linecpy(doc[l + 1], tl1);
                /* fill the remaining of the line with zero */
                for (i = (pt + 1) - doc[(pt + 1 - (chr*)doc) / MAX_H]; i < MAX_H; i++)
                    doc[(pt + 1 - (chr*)doc)/MAX_H][i].c = 0;
                if(flagn) goto end_of;
            }
            chrcpy(pt + 1, &t1);
            flag = 0;
        }
    }
    if (flag)
    {
        chrcpy(pt,&t1);
        (pt+1)->c = EOF;
        (pt+1)->f = 0;
    }

end_of:
    chead->c = c;
    chead->f = nowformat;
    if      (flagn == 1)
    {
        chead = doc[(pt - (chr*)doc) / MAX_H + 1];
        ctail += MAX_H - 1;
        del(0);
    }
    else if (flagn == 0)
    {
        chead++;
        ctail = chead;
    }
    else if (flagn == -1)
    {
        /* fill the remaining of the line with zero */
        for (i = (pt + 1) - doc[(pt+1-(chr*)doc)/MAX_H]; i < MAX_H; i++)
            doc[(pt + 1 - (chr*)doc)/MAX_H][i].c = 0;
        chead = doc[(chead - (chr*)doc) / MAX_H + 1];
        ctail = chead;
        chrcpy(chead, &tx);
    }

    gotoxy((chead - (chr*)doc) % MAX_H + 1, (chead - (chr*)doc) / MAX_H +1 - headline);
    dispdoc();
}

int isendline(int nline)
{
    int i;
    for (i = 0; i < MAX_H; i++)
        if(doc[nline][i].c == EOF)
            return 1;
    return 0;
}

int linecpy(chr *to, chr *from)
{
    int i;
    for (i = 0; i < MAX_H; i++)
    chrcpy(to + i, from + i);
}

chr *chrcpy(chr *to, chr *from)
{
    to->c = from->c;
    to->f = from->f;
    return to;
}

int phandler(int x, int y)
{
    chead = (chr*)doc + x - 1 + (headline + y - 1)*MAX_H;
    ctail = (chr*)doc + x - 1 + (headline + y - 1)*MAX_H;
}

/* commands */
int help()
{
    notice(HLP_CON);
}

int load(char *path)
{
    if (path[0] != 0)
        if (readfile(path))
        {
            ifsaved = 1;
            headline = 0;
            chead = ctail = doc;
            dispdoc();
        }
        else
        {
            notice(ERR_LOAD);
            ifsaved = 1;    /* So that the not-saved notice won't be displayed */
            newdoc();
        }
}

int save(char *path)
{
    if (path[0] == '\0')
        name();
    if (writefile(path))
    {
        ifsaved = 1;
        notice(SUC_SAVE);
    }
}

int newdoc()
{
    if (!ifsaved)
        if(notice(WAR_NOTSAVED) == -1)   return;

    docpath[0] = 0;
    doc[0][0].c = EOF, doc[0][0].f = 0;
    dispdoc();
    ifsaved = 0;
    chead = doc;
    ctail = doc;
    endline = 0;
    headline = 0;
    gotoxy(1,1);
}

int quit()
{
    textbackground(BLACK);
    textcolor(LIGHTGRAY);
    window(1,1,80,25);
    clrscr();
    exit(0);
}


/**************************************
        Module: Graphical UI
**************************************/
int dispwindow()
{
    int  nx, ny;

    nx = wherex();
    ny = wherey();
    window(1,1,80,MAX_V + 2);
    textbackground(col_editbkgd);
    textcolor(col_border);
    clrscr();
    drawborder(1,1,62,MAX_V + 2);
    dispribbon();
    editwin();
    gotoxy(nx, ny);
}

int drawborder(int beginx, int beginy, int endx, int endy)
{
    int i;

    /*for(i = 1;i < endx;i++)
    {
        gotoxy(i,1);
        putchar(196);
        gotoxy(i,endy);
        putchar(196);
    }*/
    for(i=beginy;i<=endy;i++)
    {
       gotoxy(beginx,i);
       putchar(179);
       gotoxy(endx,i);
       putchar(179);
    }
    gotoxy(beginx,beginy);
    putchar(218);
    gotoxy(beginx,endy);
    putchar(192);
    gotoxy(endx,beginy);
    putchar(191);
    gotoxy(endx,endy);
    putchar(217);
}


int dispribbon()
{
    ribbonwin();
    textbackground(col_ribbonbkgd);
    textcolor(col_ribbontext);
    clrscr();
    gotoxy(1,2);
    cputs(RIB_LINE);
}

int notice(char *content)  /* return 1: Enter; -1: Esc */
{
    int i;
    int nx, ny;
    union _key
    {
        char ch[2];
        unsigned i;
    }key;

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
    gotoxy(15,12);
    cputs("Press");
    textcolor(col_msghigh);
    cputs(" <Enter>");

    window(22,9,59,17);
    gotoxy(1,1);
    textcolor(col_msgtext);
    cprintf("%s",content);
    window(20,7,61,19);
    gotoxy(21,12);
    textcolor(col_msghigh);
    putch('<');

    do
    {
        key.i = bioskey(0);
        switch (key.i)
        {
            case kn(0x1C, 0x0D): /* Enter */
                editwin();
                gotoxy(nx,ny);
                dispdoc();
                return 1;
            case kn(0x01, 0x1B): /* Esc */
                editwin();
                gotoxy(nx,ny);
                dispdoc();
                return -1;
        }
    }while(1);
}

int name()
{
    char c;
    int  i;
    int  nx, ny;
    char tmppath[100];

    nx = wherex();
    ny = wherey();
    window(14,11,68,14);
    textbackground(col_msgbkgd);
    textcolor(col_msgtext);
    clrscr();

    window(14,11,68,14);
    gotoxy(1,1);
    cprintf("\4===================Input File Path===================\4");
    for(i=2;i<=2;i++)
    {
        gotoxy(1,i);
        putch('|');
        gotoxy(55,i);
        putch('|');
    }
    gotoxy(1,3);
    cprintf("\4-----------------------------------------------------\4");
    gotoxy(2,4);
    cprintf(NAM_HLP);
    /* input area */
    window(16,12,66,12);
    i = 0;
    gotoxy(1,1);

    gets(tmppath);
    /*while((c = getche()) != '\r' && c != 27 )
    {
        docpath[i] = c;
        i++;
        if(i > NAM_MAX -1)
        {
            while( (c=getch()) != '\r');
            break;
        }
    }
    docpath[i] = '\0';
    if(docpath[0]=='\0')  strcpy(docpath,"");*/
    if (tmppath[0] != 0)
        strcpy(docpath, tmppath);
    /*if(c == 27)
        strcpy(docpath,lastpath);*/
    editwin();
    gotoxy(nx,ny);
    dispwindow();
    dispdoc();
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
    /* handle if cursor is out of the edit area #1 */
    if (ny > MAX_V)
    {
        ny--;
        headline++;
    }
    editwin();
    textbackground(col_editbkgd);
    clrscr();
    textcolor(col_default);
    gotoxy(1,1);
    for (line = 0; line < MAX_V ; line++)
        for (col = 0; col < MAX_H; col++)
        {
            cc = doc[line + headline][col].c;
            cf = doc[line + headline][col].f;
            /* handle formats */
            if (cf == 0) textcolor(col_default);
            if (isbold(cf)) textcolor(col_bold);
            if (isitalic(cf)) textcolor(col_italic);
            if (isbold(cf) && isitalic(cf)) textcolor(col_b_i);
            /* put on the char */
            if (cc != EOF)
                putch((cc == '\n') ? 25 : cc);
            else
            {
                putch(4);
                goto end_of_dispdoc;
            }
        }
end_of_dispdoc:
    editwin();
    gotoxy(nx, ny);
    return 1;
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
    char *ext;
    int  f = 0;              /* the same as _chr.f */
    int  nline = 0;
    int  flag_tag = 0;
    int  flag_tag_f = 0;     /* is it the first character in a tag */
    int  flag_block = 0;     /* store the number of block layers of {} */

    ext = lfnhandler(path);

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
                        if (c == '{' || c == '}') ungetc(c, fp);
                        break;
                    default:
                        *ptag = c;
                        ptag++;
                        flag_tag_f = 0;
                        break;
                }
            }
            else
            {
                if (strcmp(strupr(ext), "RTF") == 0)
                    switch (c)
                    {
                        case '\n':
                            break;
                        case '\\':
                            flag_tag = 1;
                            flag_tag_f = 1;
                            break;
                        case '{':
                            flag_block++;
                            break;
                        case '}':
                            flag_block--;
                            break;
                        default:
put_in_mem:
                            #if FHO
                            #else
                            if (flag_block == 1)
                            {
                                p->c = c;
                                p->f = f;
                                p++;
                                nline = (p - doc[0] + 1) / MAX_H;
                            }
                            break;
                            #endif
                    }
                else
                {
                    if (c != '\n')
                    {
                        p->c = c;
                        p->f = f;
                        p++;
                        nline = (p - doc[0] + 1) / MAX_H;
                    }
                    else
                    {
                        p->c = '\n';
                        for (++p; (p - doc[nline]) < MAX_H; ++p)    /* fill the whole line with zero after '\n' */
                        p->c = 0;
                        p = doc[++nline];
                    }
                }
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
    chr  *p = doc;
    int  lf = 0;       /* last format */
    char tag[100];    /* tag to be inserted */
    char *ext;

    ext = lfnhandler(path);
    tag[0]=0;
    remove(tmppath);
    if ((fp = fopen(tmppath, "w")) == NULL)
    {
        notice(ERR_SAVE1);
        return 0;
    }
    else
    {
        if (strcmp(strupr(ext), "RTF") == 0)
        {
            fputs(FHS,fp);
            while(p->c != EOF)
            {
                if (isbold(p->f) && !isbold(lf))
                    strcat(tag, "\\b");
                if (!isbold(p->f) && isbold(lf))
                    strcat(tag,"\\b0");
                if (isitalic(p->f) && !isitalic(lf))
                    strcat(tag, "\\i");
                if (!isitalic(p->f) && isitalic(lf))
                    strcat(tag, "\\i0");
                if (p->c == '\n')
                    strcat(tag, "\\par");
                if (tag[0] != 0)
                {
                    strcat(tag, " ");
                    fputs(tag, fp);
                    tag[0] = 0;
                }

                lf = p->f;
                /* key characters filter */
                switch (p->c)
                {
                    case '\\':
                    case '{':
                    case '}':
                        fputc('\\',fp);
                }
                fputc(p->c, fp);
                if (p->c == '\n')
                    p = doc[(p - (chr*)doc) / MAX_H + 1];
                else
                    p++;
            }
            fputc('}',fp);
        }
        else
            while(p->c != EOF)
            {
                fputc(p->c, fp);
                p++;
            }
        fclose(fp);
        remove(path);
        if(rename(tmppath, path) == -1)
        {
            notice(ERR_SAVE2);
            return 0;
        }
    return 1;
    }
}

char *lfnhandler(char *path)        /* return extension */
{
    #if LFN
    char *dotp;
    char *ext;

    removespace(path);
    dotp = strpbrk(path,".");
    dotp++;
    strcpy(ext,dotp);
    if (dotp - path > 9)
    {
        *(ext+3) = 0;
        *(path+6) = 0;
        strcat(path,"~1.");
        strcat(path,ext);
    }
    #else
    char *ext;

    ext = strpbrk(path,".");
    ext++;
    #endif
    return ext;
}

int removespace(char *str)
{
    char *p1, *p2;
    char ch;
    p1 = str;
    p2 = str;
    if (p1 == NULL) return;
    while (*p1)
    {
        if (*p1 != ' ')
        {
            ch = *p1;
            *p2 = ch;
            p1++;
            p2++;
        }
        else
        {
            p1++;
        }
    }
    *p2 = '\0';
}
