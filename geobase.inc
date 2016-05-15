
/*************************************************************************
  SUPPORT PREDICATES - These are the clauses which support the
  general system, including the parser and the menu system. Most of
  the clauses involve list processing and are general enough to be
  used in any system.
*************************************************************************/

PREDICATES
  write_list(INTEGER,STRINGLIST)    /* Write the list separated by spaces */
  write_list2(STRINGLIST)		             /* Display an answer */
  append(STRINGLIST,STRINGLIST,STRINGLIST)	      /* Append two lists */
  unik(STRINGLIST,STRINGLIST)		/* Eliminate duplicates in a list */
  index(STRINGLIST,INTEGER,STRING)       /* Select an element from a list */

CLAUSES
  index([X|_],1,X):- !.
  index([_|L],N,X):- N>1,N1=N-1,index(L,N1,X).

  unik([],[]).
  unik([H|T],L):-member(H,T),!,unik(T,L).
  unik([H|T],[H|L]):-unik(T,L).

  append([],L,L).
  append([Ah|At],B,[Ah|C]):-append(At,B,C).



  write_list(_,[]).
  write_list(_,[X]):-!,write(X).
  write_list(4,[H|T]):-!,write(H),nl,write_list(0,T).
  write_list(3,[H|T]):-str_len(H,LEN),LEN>13,!,write(H),nl,write_list(0,T).
  write_list(N,[H|T]):-str_len(H,LEN),LEN>13,!,N1=N+2,writef("%-27 ",H),write_list(N1,T).
  write_list(N,[H|T]):-N1=N+1,writef("%-13 ",H),write_list(N1,T).

  write_list2([]).
  write_list2([H|T]):-write(H,' '),write_list2(T).


/*************************************************************************
  Evaluating queries - This is the mechanism which reads a query, scans
  the string and removes punctuation, parses the query and evaluates
  it.  The number of solutions are also reported here.
*************************************************************************/

DOMAINS
/* Nine types of questions are recognized by the evaluator */
  QUERY	=	q_e(ENT) ;
		q_eaec(ENT,ASSOC,ENT,STRING) ;
		q_eaq(ENT,ASSOC,ENT,QUERY) ;
		q_sel(ENT,RELOP,ENT,REAL);
		q_min(ENT,QUERY);
		q_max(ENT,QUERY);
		q_not(ENT,QUERY) ;
		q_or(QUERY,QUERY) ;
		q_and(QUERY,QUERY)

PREDICATES
/* Input-output */
  loop(STRING)			/* Main loop */
  readquery(STRING)
  write_unit(STRING)		/* Write the unit for an entity */
  write_solutions(INTEGER)	/* Write the number of solutions */

/* Scanner */
  scan(STRING,STRINGLIST)		/* Convert a string to a list of words */
  filter(STRINGLIST,STRINGLIST)		/* Eliminate commas and periods	*/

/* Parser */
  pars(STRINGLIST,STRING,QUERY)

/* Evaluation */
  eval(QUERY,STRING)


CLAUSES
  loop(STR):-	STR >< "",
  		scan(STR,LIST),               /* Returns a list of words(symbols)           */
		filter(LIST,LIST1),           /* Removes punctuation and words to be ignored*/
		pars(LIST1,E,Q),              /* Parses queries                            */
		findall(A,eval(Q,A),L),
		unik(L,L1),
		write_list(0,L1),
		write_unit(E),
		listlen(L1,N),
		write_solutions(N),
		fail.

  loop(STR):-	STR >< "",readquery(L),loop(L).

  readquery(QUERY):-nl,nl,write("Query: "),readln(QUERY).

  scan(STR,[TOK|LIST]):-
		fronttoken(STR,SYMB,STR1),!,
		upper_lower(SYMB,TOK),
		scan(STR1,LIST).
  scan(_,[]).

  filter(["."|T],L):-	!,filter(T,L).
  filter([","|T],L):-	!,filter(T,L).
  filter(["?"|T],L):-	!,filter(T,L).
  filter([H|T],L):-	ignore(H),!,filter(T,L).
  filter([H|T],[H|L]):-	filter(T,L).
  filter([],[]).

  write_unit(E):-unit(E,UNIT),!,write(' ',UNIT).
  write_unit(_).

  write_solutions(0):-!,write("\nNo solutions").
  write_solutions(1):-!.
  write_solutions(N):-!,writef("\n% Solutions",N).

/*************************************************************************
  ENTITY NAMES
*************************************************************************/

PREDICATES
  entn(STRING,STRING)		/* Convert an entity to singular form */
  entity(STRING)		/* Get all entities */
  ent_synonym(STRING,STRING)	/* Synonyms for entities */
  ent_name(STRING,STRING)	/* Convert between an entity
				   name and an internal entity name */

CLAUSES
  ent_synonym(E,ENT):-synonym(E,ENT).
  ent_synonym(E,E).

  ent_name(ENT,NAVN):-entn(E,NAVN),ent_synonym(E,ENT),entity(ENT).

  entn(E,N):-concat(E,"s",N).
  entn(E,N):-free(E),bound(N),concat(X,"ies",N),concat(X,"y",E).
  entn(E,E).

  entity(name):-!.
  entity(continent):-!.
  entity(X):-schema(X,_,_).


/*************************************************************************
  ERROR DETECTION -
  Once the string has been converted to a list of words, the word
  list can be checked against the language database to see if it
  is a known word. Words which are not known are collected into a
  list which the system reports on.
*************************************************************************/

PREDICATES
  error(STRINGLIST)
  known_word(STRING)

CLAUSES
  error(LIST):-	write(">> "),member(Y,LIST),not(known_word(Y)),!,
		write("Unknown word: ",Y),nl.

  error(_):-	write("Sorry, the sentence can't be recognized").

  known_word(X):-str_real(X,_),!.  /*   Check for special case words    */
  known_word("and"):-!.
  known_word("or"):-!.
  known_word("not"):-!.
  known_word("all"):-!.
  known_word("thousand"):-!.
  known_word("million"):-!.
  known_word(X):-minn(X),!.     /*  If not a special case word, check the */
  known_word(X):-maxx(X),!.     /*  dynamic database for known words      */
  known_word(X):-size(_,X),!.   /*  additional words.                     */
  known_word(X):-ignore(X),!.
  known_word(X):-unit(_,X),!.
  known_word(X):-assoc(_,AL),member(X,AL),!.
  known_word(X):-ent_name(_,X),!.
  known_word(X):-entity(X),!.
  known_word(X):-relop(L,_),member(X,L),!.
  known_word(X):-entity(E),not(unit(E,_)),ent(E,X).

/*************************************************************************
		PARSER
*************************************************************************/

/*
   PARSER SUPPORT -  Compound entities:
   This is used by the parser to handle a compound entity (e.g.
   New York).
*/

PREDICATES		
  check(STRINGLIST)                          /* Check that the list is empty */
  get_ent(STRINGLIST,STRINGLIST,STRING)            /* Get the compound entity      */
  get_cmpent(STRINGLIST,STRINGLIST,STRING,STRING)  /* Get the first component      */
  ent_end(STRINGLIST)                        /* Get the rest of the entity   */

CLAUSES
  check([]).	

  get_ent([E|S],S,E):-ent_end(S),!.
  get_ent(S1,S2,ENT):-get_cmpent(S1,S2," ",E1),frontchar(E1,_,E),ENT=E.

  get_cmpent([E|S],S,IND,ENT):-ent_end(S),concat(IND,E,ENT).
  get_cmpent([E|S1],S2,IND,ENT):-
		concat(IND,E,II),concat(II," ",III),
		get_cmpent(S1,S2,III,ENT).

  ent_end([]).
  ent_end(["and"|_]).
  ent_end(["or"|_]).

/*
  Here begins the parser. The first two parameters for the parsing
  predicates are the inputlist and what remains of the list
  after a part of a query is stripped off. In the last parameter, a
  structure for the query is built up.

  This method is called "parsing by difference lists." Once you
  understand how it works, you can easily add new sentence
  constructions to the language.
*/

PREDICATES
  s_rel(STRINGLIST,STRINGLIST,STRING)
  s_unit(STRINGLIST,STRINGLIST,STRING)
  s_val(STRINGLIST,STRINGLIST,REAL)

CLAUSES
  s_rel(S1,S2,REL):-relop(RLIST,REL),append(RLIST,S2,S1).

  s_unit([UNIT|S],S,UNIT).
  s_val([X,thousand|S],S,VAL):-	!,str_real(X,XX),VAL=1000*XX.
  s_val([X,million|S],S,VAL):-	!,str_real(X,XX),VAL=1000000*XX.
  s_val([X|S],S,VAL):-		str_real(X,VAL).


PREDICATES
  s_attr(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_minmax(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_rest(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_or(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_or1(STRINGLIST,STRINGLIST,STRING,QUERY,QUERY)
  s_and(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_and1(STRINGLIST,STRINGLIST,STRING,QUERY,QUERY)
  s_elem(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_assoc(STRINGLIST,STRINGLIST,STRING,QUERY)
  s_assoc1(STRINGLIST,STRINGLIST,STRING,STRING,QUERY)
  s_nest(STRINGLIST,STRINGLIST,STRING,QUERY)
  get_assoc(STRINGLIST,STRINGLIST,STRING)

CLAUSES
  pars(LIST,E,Q):-s_attr(LIST,OL,E,Q),check(OL),!.
  pars(LIST,_,_):-error(LIST),fail.

  /* How big is the city new york -- BIG ENTITY CONSTANT */
  s_attr([BIG,ENAME|S1],S2,E1,q_eaec(E1,A,E2,X)):-
		ent_name(E2,ENAME),size(E2,BIG),
		entitysize(E2,E1),schema(E1,A,E2),
		get_ent(S1,S2,X),!.

  /* How big is new york -- BIG CONSTANT */
  s_attr([BIG|S1],S2,E1,q_eaec(E1,A,E2,X)):-
		get_ent(S1,S2,X),
		size(E2,BIG),entitysize(E2,E1),
		schema(E1,A,E2),ent(E2,X),!.

  /* How big is the biggest city -- BIG QUERY */
  s_attr([BIG|S1],S2,E1,q_eaq(E1,A,E2,Q)):-
		size(_,BIG),s_minmax(S1,S2,E2,Q),
		size(E2,BIG),entitysize(E2,E1),
		schema(E1,A,E2),!.

  s_attr(S1,S2,E,Q):-s_minmax(S1,S2,E,Q).

/* The smallest city -- MIN QUERY */
  s_minmax([MIN|S1],S2,E,q_min(E,Q)):-minn(MIN),!,s_rest(S1,S2,E,Q).

/* The biggest city -- MAX QUERY */
  s_minmax([MAX|S1],S2,E,q_max(E,Q)):-maxx(MAX),!,s_rest(S1,S2,E,Q).

  s_minmax(S1,S2,E,Q):-s_rest(S1,S2,E,Q).


/* give me cities -- ENTITY */
  s_rest([ENAME],[],E,q_e(E)):-!,ent_name(E,ENAME).

  s_rest([ENAME|S1],S2,E,Q):-ent_name(E,ENAME),s_or(S1,S2,E,Q).


/* And has a higher priority than or */
  s_or(S1,S2,E,Q):-s_and(S1,S3,E,Q1),s_or1(S3,S2,E,Q1,Q).
  s_or1(["or",ENT|S1],S2,E,Q1,q_or(Q1,Q2)):-ent_name(E,ENT),!,s_or(S1,S2,E,Q2).
  s_or1(["or"|S1],S2,E,Q1,q_or(Q1,Q2)):-!,s_or(S1,S2,E,Q2).
  s_or1(S,S,_,Q,Q).

  s_and(S1,S2,E,Q):-s_elem(S1,S3,E,Q1),s_and1(S3,S2,E,Q1,Q).
  s_and1(["and",ENT|S1],S2,E,Q1,q_and(Q1,Q2)):-ent_name(E,ENT),!,s_elem(S1,S2,E,Q2).
  s_and1(["and"|S1],S2,E,Q1,q_and(Q1,Q2)):-!,s_elem(S1,S2,E,Q2).
  s_and1(S,S,_,Q,Q).


/* not QUERY */
  s_elem(["not"|S1],S2,E,q_not(E,Q)):-!,s_assoc(S1,S2,E,Q).
  s_elem(S1,S2,E,Q):-s_assoc(S1,S2,E,Q).


/* ... longer than 1 thousand miles -- REL VAL UNIT */
  s_assoc(S1,S4,E,q_sel(E,REL,ATTR,VAL)):-
		s_rel(S1,S2,REL),s_val(S2,S3,VAL),
		s_unit(S3,S4,UNIT),!,unit(ATTR,UNIT).

/* ... longer than 1 thousand -- REL VAL */
  s_assoc(S1,S3,E,q_sel(E,REL,ATTR,VAL)):-
		s_rel(S1,S2,REL),s_val(S2,S3,VAL),!,
		entitysize(E,ATTR).

  s_assoc(S1,S3,E,Q):-
		get_assoc(S1,S2,A),s_assoc1(S2,S3,E,A,Q).


/* Before s_assoc1 is called ENT ASSOC is met */

/* ... the shortest river in texas -- MIN QUERY */
  s_assoc1([MIN|S1],S2,E1,A,q_eaq(E1,A,E2,q_min(E2,Q))):-minn(MIN),!,
		s_nest(S1,S2,E2,Q),schema(E1,A,E2).

/* ... the longest river in texas -- MAX QUERY */
  s_assoc1([MAX|S1],S2,E1,A,q_eaq(E1,A,E2,q_max(E2,Q))):-maxx(MAX),!,
		s_nest(S1,S2,E2,Q),schema(E1,A,E2).

/* ... with a population that is smaller than 1 million citizens --
  							 ENT REL VAL UNIT */
  s_assoc1([ATTR|S1],S4,E,A,q_sel(E,REL,ATTR,VAL)):-
	s_rel(S1,S2,REL),s_val(S2,S3,VAL),s_unit(S3,S4,UNIT1),!,
	ent_name(E2,ATTR),schema(E,A,E2),unit(E2,UNIT),
	UNIT=UNIT1,!.

/* ... with a population that are smaller than 1 million -- ENT REL VAL */
  s_assoc1([ATTR|S1],S3,E,A,q_sel(E,REL,ATTR,VAL)):-
	s_rel(S1,S2,REL),s_val(S2,S3,VAL),!,
	ent_name(E2,ATTR),schema(E,A,E2),unit(E2,_).

/* ... that is smaller than 1 million citizens -- REL VAL UNIT */
  s_assoc1(S1,S4,E,A,q_sel(E,REL,E2,VAL)):-
	s_rel(S1,S2,REL),s_val(S2,S3,VAL),s_unit(S3,S4,UNIT1),!,
	schema(E,A,E2),unit(E2,UNIT),
	UNIT=UNIT1,!.

/* ... that is smaller than 1 million -- REL VAL */
  s_assoc1(S1,S3,E,A,q_sel(E,REL,E2,VAL)):-
	s_rel(S1,S2,REL),s_val(S2,S3,VAL),!,
	schema(E,A,E2),unit(E2,_).

/* ... with a population on 1 million citizens -- ENT VAL UNIT */
  s_assoc1([ATTR|S1],S3,E,A,q_sel(E,eq,ATTR,VAL)):-
	s_val(S1,S2,VAL),s_unit(S2,S3,UNIT1),!,
	ent_name(E2,ATTR),schema(E,A,E2),unit(E2,UNIT2),UNIT1=UNIT2,!.

/* ... with a population on 1 million -- ENT VAL */
  s_assoc1([ATTR|S1],S2,E,A,q_sel(E,eq,ATTR,VAL)):-
	s_val(S1,S2,VAL),
	ent_name(E2,ATTR),schema(E,A,E2),unit(E2,_),!.

/* .. the state new york -- ENT CONST */
  s_assoc1([ENAME|S1],S2,E1,A,q_eaec(E1,A,E2,X)):-
		get_ent(S1,S2,X),ent_name(E2,ENAME),
		not(unit(E2,_)),
		schema(E1,A,E2),
		ent(E2,X),!.

  s_assoc1(S1,S2,E1,A,q_eaq(E1,A,E2,Q)):-
		s_nest(S1,S2,E2,Q),schema(E1,A,E2),!.

/* .. new york -- CONST */
  s_assoc1(S1,S2,E1,A,q_eaec(E1,A,E2,X)):-
		get_ent(S1,S2,X),schema(E1,A,E2),ent(E2,X),!.

/* Parse a nested query */
  s_nest([ENAME|S1],S2,E,Q):-ent_name(E,ENAME),s_elem(S1,S2,E,Q).
  s_nest([ENAME|S],S,E,q_e(E)):-ent_name(E,ENAME).

/* ... runs through texas -- ASSOC REST */
  get_assoc(IL,OL,A):-append(ASL,OL,IL),assoc(A,ASL).

/*************************************************************************
  EVALUATION OF QUESTIONS
*************************************************************************/

PREDICATES  /* Support predicates for the parser */
  sel_min(STRING,STRING,REAL,STRING,STRING,STRINGLIST)
  sel_max(STRING,STRING,REAL,STRING,STRING,STRINGLIST)

CLAUSES
  eval(q_min(ENT,TREE),ANS):-
		findall(X,eval(TREE,X),L),
		entitysize(ENT,ATTR),
		sel_min(ENT,ATTR,99e99,"",ANS,L).

  eval(q_max(ENT,TREE),ANS):-
		findall(X,eval(TREE,X),L),
		entitysize(ENT,ATTR),
		sel_max(ENT,ATTR,-1,"",ANS,L).

  eval(q_sel(E,gt,ATTR,VAL),ANS):-
		schema(ATTR,ASSOC,E),
		db(ATTR,ASSOC,E,SVAL2,ANS),
		str_real(SVAL2,VAL2),
		VAL2>VAL.

  eval(q_sel(E,lt,ATTR,VAL),ANS):-
		schema(ATTR,ASSOC,E),
		db(ATTR,ASSOC,E,SVAL2,ANS),
		str_real(SVAL2,VAL2),
		VAL2<VAL.

  eval(q_sel(E,eq,ATTR,VAL),ANS):-
		schema(ATTR,ASSOC,E),
		db(ATTR,ASSOC,E,SVAL,ANS),
		str_real(SVAL,VAL).

  eval(q_not(E,TREE),ANS):-
		findall(X,eval(TREE,X),L),
		ent(E,ANS),
		not(member(ANS,L)).

  eval(q_eaq(E1,A,E2,TREE),ANS):-
		eval(TREE,VAL),db(E1,A,E2,ANS,VAL).

  eval(q_eaec(E1,A,E2,C),ANS):-db(E1,A,E2,ANS,C).

  eval(q_e(E),ANS):-	ent(E,ANS).

  eval(q_or(TREE,_),ANS):- eval(TREE,ANS).

  eval(q_or(_,TREE),ANS):- eval(TREE,ANS).

  eval(q_and(T1,T2),ANS):- eval(T1,ANS1),eval(T2,ANS),ANS=ANS1.


  sel_min(_,_,_,RES,RES,[]).
  sel_min(ENT,ATTR,MIN,_,RES,[H|T]):-schema(ATTR,ASSOC,ENT),
	db(ATTR,ASSOC,ENT,VAL,H),
	str_real(VAL,HH),MIN>HH,!,
	sel_min(ENT,ATTR,HH,H,RES,T).
  sel_min(ENT,ATTR,MIN,NAME,RES,[_|T]):-sel_min(ENT,ATTR,MIN,NAME,RES,T).


  sel_max(_,_,_,RES,RES,[]).
  sel_max(ENT,ATTR,MAX,_,RES,[H|T]):-
	schema(ATTR,ASSOC,ENT),
	db(ATTR,ASSOC,ENT,VAL,H),
	str_real(VAL,HH),MAX<HH,!,
	sel_max(ENT,ATTR,HH,H,RES,T).
  sel_max(ENT,ATTR,MAX,NAME,RES,[_|T]):-sel_max(ENT,ATTR,MAX,NAME,RES,T).

/**************************************************************************
  MAIN MENU - Here begins the user interface which demonstrates
  how to process an action from a list of choices.
**************************************************************************/

PREDICATES
/* Main loop */
  natlang
  loaddba
  savedba
  mainmenu
  proces(INTEGER)

/* View and update the language */
  viewlang viewlang1(INTEGER)
  updatelang updatelang1(INTEGER)

GOAL loaddba, natlang.

CLAUSES
  natlang:-
	makewindow(21,112,0,"",24,0,1,80),
	write("ESC: Quit this menu -- Use arrow keys to select and hit RETURN to activate."),
	makewindow(22,112,0,"",24,0,1,80),
	write("Esc: Quit     F8: Last line    Ctrl S: Stop output    End: End of line"),
	makewindow(2,7,7,"GEOBASE: Natural language interface to U.S. geography",0,0,24,80),
	mainmenu.

  mainmenu:-	repeat,
		menu(8,49,14,6,
		  [ "Tutorial",
		    "DOS Shell",
		    "Editor",
		    "컴컴컴컴컴컴컴컴컴�",
		    "Query the database",
		    "컴컴컴컴컴컴컴컴컴�",
		    "View the language",
		    "Update the language"]," Main Menu ",1,CHOICE),
		proces(CHOICE),
		CHOICE=0,!,
		removewindow,removewindow.

  proces(0):-write("\nAre you sure you want to quit? (y/n): "),readchar(T),T='y'.
  proces(1):-file_str("geobase.hlp",TXT),display(TXT),clearwindow,!.
  proces(1):-write(">> geobase.hlp not in default directory\n").
  proces(2):-makewindow(3,7,0,"",0,0,25,80),write("Type EXIT to return\n\n"),
             system(""),!,removewindow.
  proces(2):-write(">> command.com not accessible. press any key"),readchar(_),removewindow.
  proces(3):-makewindow(3,7,112,"",9,5,15,75),edit("",_),removewindow.
  proces(4).
  proces(5):-readquery(L),loop(L).
  proces(6).
  proces(7):-viewlang.
  proces(8):-updatelang.

  loaddba:-schema(_,_,_),!.  /* Database already loaded */
  loaddba:-
	existfile("geobase.lan"),existfile("geobase.dba"),
	write("Loading database file - please wait\n"),
	consult("geobase.lan",language),
	consult("geobase.dba",data),!.
  loaddba:-
	write(">> geobase.dba not in default directory\n").

  savedba:-
	write("Saving language definition - please wait\n"),
	deletefile("geobase.bak"),
	renamefile("geobase.lan","geobase.bak"),
	save("geobase.lan",language).

/**************************************************************************
   View and the language
**************************************************************************/

  viewlang:-	repeat,
		menu(5,40,14,10,
		  [ "1  Schema for the entity network",
		    "2  Names of entities",
		    "3  Synonyms for entities",
		    "4  Alternative names for associations",
		    "5  Words to ignore",
		    "6  Units for attributes",
		    "7  Alternatives for relation operators",
		    "8  Words stating minimums",
		    "9  Words stating maximum"
		  ]," Language ",1,CHOICE),
		nl,viewlang1(CHOICE),CHOICE=0,!.


  viewlang1(0).

  viewlang1(1):-
	writef("%-12 %-8 %-12\n","Entity","Assoc","Entity"),
	write("************ ******** ************\n"),
	schema(E1,A,E2),writef("%-12 %-8 %-12\n",E1,A,E2),fail.
	
  viewlang1(1):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(2):-
	write("Entities\n********\n"),
	findall(X,entity(X),L),unik(L,L1),write_list(0,L1),nl.
	
  viewlang1(2):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(3):-
	writef("%-15 %-15\n","Synonym","Entity"),
	write("*************** ***************\n"),
	synonym(E,S),writef("%-15 %-15\n",E,S),fail.
	
  viewlang1(3):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(4):-
	write("Associations\n************\n"),
	assoc(X,L),
	writef("%-8 ",X),write_list2(L),nl,fail.
	
  viewlang1(4):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(5):-
	write("Ignore\n******\n"),
	findall(X,ignore(X),L),write_list(0,L),nl.
	
  viewlang1(5):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(6):-
	writef("%-15 %-15\n","entity","unit"),
	write("*************** ***************\n"),
	unit(E,U),writef("%-15 %-15\n",E,U),fail.
	
  viewlang1(6):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(7):-
	write("Names of relational operators\n*****************************\n"),
	relop(LIST,REL),write(REL,": "),write_list2(LIST),nl,fail.
	
  viewlang1(7):-
    write("\n\nPress any key to continue"),
    readchar(_).	

  viewlang1(8):-
	write("Minimum\n*******\n"),
	findall(X,minn(X),L),write_list(0,L),nl.
	
  viewlang1(8):-
    write("\n\nPress any key to continue"),
    readchar(_).
  	
  viewlang1(9):-
	write("Maximum\n*******\n"),
	findall(X,maxx(X),L),write_list(0,L),nl.
	
  viewlang1(9):-
    write("\n\nPress any key to continue"),
    readchar(_).	

/*************************************************************************
   Update the language
*************************************************************************/

DATABASE - updateflag
  updated

PREDICATES
  newignore
  newsynonym
  newassoc
  getent(ENT)
  getassoc(ASSOC)
  reg_updated
  save_if_updated

CLAUSES
  updatelang:-	retractall(updated),
  		repeat,
		menu(5,40,3,9,
		  [ "New Synonyms for entities",
		    "New Alternatives for associations",
		    "New Words to be ignored"
		  ],"Update Language",1,CHOICE),
		nl,updatelang1(CHOICE),CHOICE=0,!,
		save_if_updated.

  updatelang1(0).
  updatelang1(1):-newsynonym.
  updatelang1(2):-newassoc.
  updatelang1(3):-newignore.

  newsynonym:-	getent(E),write("Synonym: "),
		readln(SYNONYM),SYNONYM><"",
		assert(synonym(SYNONYM,E)),
		reg_updated,
		newsynonym.

  newignore:-	write("Ignore:"),readln(IGNORE),IGNORE><"",
		reg_updated,
		assert(ignore(IGNORE)),newignore.

  newassoc:-
		getassoc(ASSOC),
		write("New form: "),
		readln(FORM),FORM >< "",
		scan(FORM,LIST),
		reg_updated,
		assert(assoc(ASSOC,LIST)),
		newassoc.

  getassoc(A):-
		findall(X,assoc(X,_),L),
		unik(L,L1),
		menu(11,30,7,7,L1,"Assoc",1,C),
		index(L1,C,A).

  getent(E):-
		findall(X,entity(X),L),
		unik(L,L1),
		menu(2,49,7,7,L1,"Entity",1,C),
		index(L1,C,E).

  reg_updated:-updated,!.
  reg_updated:-assert(updated).

  save_if_updated:-updated,!,savedba.
  save_if_updated.
