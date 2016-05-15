/****************************************************************

  Copyright (c) 1986, 88 by Borland International, Inc.

 			menu
  Implements a popup menu with at most 23 possible choices.
  For more than 23 possible choices use longmenu.

  The up and down arrow keys can be used to move the bar
  RETURN or F10  will select an indicated item.
  Pressing Esc aborts menu selection and returns zero.

  The arguments to menu are:
  
  menu(ROW,COL,WINDOWATTR,FRAMEATTR,STRINGLIST,HEADER,STARTCHOICE,SELECTION)

	ROW and COL determines the position of the window
	WATTR and FATTR determine the attributes for the window
		and its frame - if FATTR is zero there
		will be no frame around the window.
	STRINGLIST is the list of menu items
	HEADER is the text to appear at the top of the menu window
	STARTCHOICE determines where the bar should be placed.

  Ex:	 menu(5,5,7,7,[this,is,a,test],"select word",0,CHOICE)

****************************************************************/
/*                    remove comment to run  
include "tdoms.pro"
include "tpreds.pro"
*/
PREDICATES
  menu(ROW,COL,ATTR,ATTR,STRINGLIST,STRING,INTEGER,INTEGER)
  menuinit(ROW,COL,ATTR,ATTR,STRINGLIST,STRING,ROW,COL)
  menu1(SYMBOL,ROW,ATTR,STRINGLIST,ROW,COl,INTEGER)
  menu2(KEY,STRINGLIST,ROW,ROW,ROW,SYMBOL)

CLAUSES
  menu(ROW,COL,WATTR,FATTR,LIST,HEADER,STARTCHOICE,CHOICE) :-
	menuinit(ROW,COL,WATTR,FATTR,LIST,HEADER,NOOFROW,LEN),
	ST1=STARTCHOICE-1,max(0,ST1,ST2),MAX=NOOFROW-1,min(ST2,MAX,STARTROW),
	menu1(cont,STARTROW,WATTR,LIST,NOOFROW,LEN,CHOICE),
	removewindow.

  menuinit(ROW,COL,WATTR,FATTR,LIST,HEADER,NOOFROW,NOOFCOL):-
	maxlen(LIST,0,MAXNOOFCOL),
	str_len(HEADER,HEADLEN),
	HEADL1=HEADLEN+4,
	max(HEADL1,MAXNOOFCOL,NOOFCOL),
	listlen(LIST,LEN), LEN>0,
	NOOFROW=LEN,
	adjframe(FATTR,NOOFROW,NOOFCOL,HH1,HH2),
	adjustwindow(ROW,COL,HH1,HH2,AROW,ACOL),
	makewindow(81,WATTR,FATTR,HEADER,AROW,ACOL,HH1,HH2),
	writelist(0,NOOFCOL,LIST).

  menu1(cont,ROW,ATTR,LIST,MAXROW,NOOFCOL,CHOICE):-!,
	reverseattr(ATTR,REV),
	field_attr(ROW,0,NOOFCOL,REV),
	cursor(ROW,0),
	readkey(KEY),
	field_attr(ROW,0,NOOFCOL,ATTR),
	menu2(KEY,LIST,MAXROW,ROW,NEXTROW,CONT),
	menu1(CONT,NEXTROW,ATTR,LIST,MAXROW,NOOFCOL,CHOICE).
  menu1(esc,ROW,_,_,_,_,CHOICE):-!,CHOICE=ROW+1.
  menu1(_,ROW,ATTR,_,_,NOOFCOL,CHOICE):-
	CHOICE=ROW+1,
	reverseattr(ATTR,REV),
	field_attr(ROW,0,NOOFCOL,REV).

  menu2(esc,_,_,_,-1,esc):-!.
  menu2(fkey(10),_,_,ROW,ROW,stop):-!.
  menu2(char(C),LIST,_,_,CH,selection):-tryletter(C,LIST,CH),!.
/*menu2(fkey(1),_,_,ROW,ROW,cont):-!,help.  If a help system is used */
  menu2(cr,_,_,ROW,CH,selection):-!,CH=ROW.
  menu2(up,_,_,ROW,NEXTROW,cont):-ROW>0,!,NEXTROW=ROW-1.
  menu2(down,_,MAXROW,ROW,NEXTROW,cont):-NEXTROW=ROW+1,NEXTROW<MAXROW,!.
  menu2(end,_,MAXROW,_,NEXT,cont):-!,NEXT=MAXROW-1.
  menu2(pgdn,_,MAXROW,_,NEXT,cont):-!,NEXT=MAXROW-1.
  menu2(home,_,_,_,0,cont):-!.
  menu2(pgup,_,_,_,0,cont):-!.
  menu2(_,_,_,ROW,ROW,cont).



/****************************************************************/
/* 			menu_repeat				*/
/* As menu but the window is not removed on return.		*/
/****************************************************************/

PREDICATES
  nondeterm menu_repeat(ROW,COL,ATTR,ATTR,STRINGLIST,STRING,INTEGER,INTEGER)
  nondeterm menu_repeat1(ROW,ATTR,STRINGLIST,ROW,COl,INTEGER)
  nondeterm menu_repeat3(SYMBOL,ROW,ATTR,STRINGLIST,ROW,COl,INTEGER,INTEGER)

CLAUSES
  menu_repeat(ROW,COL,WATTR,FATTR,LIST,HEADER,STARTCHOICE,CHOICE) :-
	menuinit(ROW,COL,WATTR,FATTR,LIST,HEADER,NOOFROW,NOOFCOL),
	ST1=STARTCHOICE-1,max(0,ST1,ST2),MAX=NOOFROW-1,min(ST2,MAX,STARTROW),
	menu_repeat1(STARTROW,WATTR,LIST,NOOFROW,NOOFCOL,CHOICE).

  menu_repeat(_,_,_,_,_,_,_,_):-removewindow,fail.


	
  menu_repeat1(STARTROW,WATTR,LIST,NOOFROW,NOOFCOL,C):-
	menu1(cont,STARTROW,WATTR,LIST,NOOFROW,NOOFCOL,C1),
	menu_repeat3(cont,STARTROW,WATTR,LIST,NOOFROW,NOOFCOL,C1,C).
		
  menu_repeat3(_,_,_,_,_,_,C,C):-C<>0.
  menu_repeat3(cont,_,WATTR,LIST,NOOFROW,NOOFCOL,C1,C):-
	C1<>0,
	XX=C1-1,
	menu_repeat1(XX,WATTR,LIST,NOOFROW,NOOFCOL,C).
