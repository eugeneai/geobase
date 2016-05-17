/*
		 G E O B A S E
		 =============

  Copyright (c) 1986, 88 by Borland International, Inc.

  The GEOGRAPHY database with a natural language interface

  GEOBASE REQUIRES 512 K OF MEMORY TO COMPILE. IF THE FOLLOWING
  ERROR MESSAGE APPEARS IN THE MESSAGE WINDOW:

  1003  Heap overflow. Not enough memory or an endless loop,

  REMOVE ANY RAM RESIDENT PROGRAMS TO FREE
  ADDITIONAL MEMORY.  THEN RECOMPILE AND RUN
  GEOBASE AGAIN.
*/

:- consult('compat.pl').

:- use_module(library(porter_stem)). %tokenize_atom

:- consult('tdoms.pl').

/**********************************************************
 The Language Tables - These are the database predicates
 which define the language we will use to query Geobase.
***********************************************************/


:- dynamic([state/10, city/4, river/3, border/3,highlow/6,mountain/4,lake/3,road/2]).
:- dynamic([schema/3,relop/2,assoc/2,synonym/2,ignore/1,minn/1,maxx/1,size/2,unit/2]).


% DATABASE - data
% /*state(NAME,ABBREVIATION,CAPITAL,AREA,ADMIT,POPULATION,CITY,CITY,CITY,CITY */
%   state(STRING,STRING,STRING,REAL,REAL,INTEGER,STRING,STRING,STRING,STRING)

% /*city(STATE,ABBREVIATION,NAME,POPULATION) */
%   city(STRING,STRING,STRING,REAL)

% /*river(NAME,LENGTH,STATESTRINGLIST */
%   river(STRING,INTEGER,STRINGLIST)

% /*border(STATE,ABBREVIATION,STATELIST) */
%   border(STRING,STRING,STRINGLIST)

% /*highlow(STATE,ABBREVIATION,POINT,HEIGHT,POINT,HEIGHT) */
%   highlow(STRING,STRING,STRING,INTEGER,STRING,INTEGER)

% /*mountain(STATE,ABBREVIATION,NAME,HEIGHT) */
%   mountain(STRING,STRING,STRING,REAL)

% /*lake(NAME,AREA,STATELIST) */
%   lake(STRING,REAL,STRINGLIST)

% /*road(NUMBER,STATELIST) */
%   road(STRING,STRINGLIST)

:- consult('tpreds.pl').
% :- consult('menu2.pl').


/**************************************************************
	Access to the database
****************************************************************/


:-consult('geobase_inc.pl'). /* Include parser + scanner + eval*/

/*
  ent returns values for a given entity name. Ex. if called by
  ent(city,X)  X  is instantiated to cities.
*/
  ent(continent,usa).
  ent(city,NAME):-	city(_,_,NAME,_).
  ent(state,NAME):-	state(NAME,_,_,_,_,_,_,_,_,_).
  ent(capital,NAME):-	state(_,_,NAME,_,_,_,_,_,_,_).
  ent(river,NAME):-	river(NAME,_,_).
  ent(point,POINT):-	highlow(_,_,_,_,POINT,_).
  ent(point,POINT):-	highlow(_,_,POINT,_,_,_).
  ent(mountain,M):-	mountain(_,_,M,_).
  ent(lake,LAKE):-	lake(LAKE,_,_).
  ent(road,NUMBER):-	road(NUMBER,_).
  ent(population,POPUL):-city(_,_,_,POPUL1),str_real(POPUL,POPUL1).
  ent(population,S):-state(_,_,_,POPUL,_,_,_,_,_,_),str_real(S,POPUL).

/*
  The db predicate is used to establish relationships between
  entities. The first three parameters should always be instantiated
  to entity_name - assoc_name - entity_name. The last two parameters
  return the values corresponding to the two entity names.
*/

  /* Relationships about cities */
  db(city,in,state,CITY,STATE):-	city(STATE,_,CITY,_).
  db(state,with,city,STATE,CITY):-	city(STATE,_,CITY,_).
  db(population,of,city,POPUL,CITY):-	city(_,_,CITY,POPUL1),str_real(POPUL,POPUL1).
  db(population,of,capital,POPUL,CITY):-city(_,_,CITY,POPUL1),str_real(POPUL,POPUL1).

  /* Relationships about states */
  db(abbreviation,of,state,ABBREVIATION,STATE):-	state(STATE,ABBREVIATION,_,_,_,_,_,_,_,_).
  db(state,with,abbreviation,STATE,ABBREVIATION):-state(STATE,ABBREVIATION,_,_,_,_,_,_,_,_).
  db(area,of,state,AREA,STATE):-	state(STATE,_,_,_,AREA1,_,_,_,_,_),str_real(AREA,AREA1).
  db(capital,of,state,CAPITAL,STATE):-	state(STATE,_,CAPITAL,_,_,_,_,_,_,_).
  db(state,with,capital,STATE,CAPITAL):-state(STATE,_,CAPITAL,_,_,_,_,_,_,_).
  db(population,of,state,POPULATION,STATE):-state(STATE,_,_,POPUL,_,_,_,_,_,_),str_real(POPULATION,POPUL).
  db(state,border,state,STATE1,STATE2):-border(STATE2,_,LIST),member(STATE1,LIST).

  /* Relationships about rivers */
  db(length,of,river,LENGTH,RIVER):-	river(RIVER,LENGTH1,_),str_real(LENGTH,LENGTH1).
  db(state,with,river,STATE,RIVER):-	river(RIVER,_,LIST),member(STATE,LIST).
  db(river,in,state,RIVER,STATE):-	river(RIVER,_,LIST),member(STATE,LIST).

  /* Relationships about points */
  db(point,in,state,POINT,STATE):-	highlow(STATE,_,POINT,_,_,_).
  db(point,in,state,POINT,STATE):-	highlow(STATE,_,_,_,POINT,_).
  db(state,with,point,STATE,POINT):-	highlow(STATE,_,POINT,_,_,_).
  db(state,with,point,STATE,POINT):-	highlow(STATE,_,_,_,POINT,_).
  db(height,of,point,HEIGHT,POINT):-	highlow(_,_,_,_,POINT,H),str_real(HEIGHT,H),!.
  db(height,of,point,HEIGHT,POINT):-	highlow(_,_,POINT,H,_,_),str_real(HEIGHT,H),!.

  /* Relationships about mountains */
  db(mountain,in,state,MOUNT,STATE):-	mountain(STATE,_,MOUNT,_).
  db(state,with,mountain,STATE,MOUNT):-	mountain(STATE,_,MOUNT,_).
  db(height,of,mountain,HEIGHT,MOUNT):-	mountain(_,_,MOUNT,H1),str_real(HEIGHT,H1).

  /* Relationships about lakes */
  db(lake,in,state,LAKE,STATE):-	lake(LAKE,_,LIST),member(STATE,LIST).
  db(state,with,lake,STATE,LAKE):-	lake(LAKE,_,LIST),member(STATE,LIST).
  db(area,of,lake,AREA,LAKE):-		lake(LAKE,A1,_),str_real(AREA,A1).

  /* Relationships about roads */
  db(road,in,state,ROAD,STATE):-	road(ROAD,LIST),member(STATE,LIST).
  db(state,with,road,STATE,ROAD):-	road(ROAD,LIST),member(STATE,LIST).

  db(E,in,continent,VAL,usa):-		ent(E,VAL).
  db(name,of,_,X,X):-			nonvar(X).


/* ------------------------ tests --------------------------------- */

test(X):-X='state', loaddba, geobase(X).
