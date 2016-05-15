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

code=3000

DOMAINS
  ENT	= STRING	/* Names of entities			*/
  ASSOC	= STRING	/* Names of associations		*/
  RELOP	= STRING	/* gt, lt, eq for comparisons		*/
  UNIT	= STRING	/* kilometers, citizens etc.		*/

INCLUDE "tdoms.pro"

/**********************************************************
 The Language Tables - These are the database predicates
 which define the language we will use to query Geobase.
***********************************************************/

DATABASE - language
  schema(ENT,ASSOC,ENT)		/* Entity network: entity-assoc-entity */
  entitysize(ENT,STRING)	/* This attribute tells which words can be
				   user to query the size of the entity	 */
  relop(STRINGLIST,STRING)		/* Example: relop([greater,than],gt] */
  assoc(ASSOC,STRINGLIST)		/* Alternative assoc names */
  synonym(STRING,ENT)		/* Synonyms for entities */
  ignore(STRING)		/* Words to be ignored */
  minn(STRING)			/* Words stating minimum */
  maxx(STRING)			/* Words stating maximum */
  size(STRING,STRING)		/* big, long, high .... */
  unit(STRING,STRING)		/* Units for population, area ... */


/**************************************************************
  The real database - These are the database predicates which
  actually  maintain the information we will access.
****************************************************************/

DATABASE - data
/*state(NAME,ABBREVIATION,CAPITAL,AREA,ADMIT,POPULATION,CITY,CITY,CITY,CITY */
  state(STRING,STRING,STRING,REAL,REAL,INTEGER,STRING,STRING,STRING,STRING)

/*city(STATE,ABBREVIATION,NAME,POPULATION) */
  city(STRING,STRING,STRING,REAL)

/*river(NAME,LENGTH,STATESTRINGLIST */
  river(STRING,INTEGER,STRINGLIST)

/*border(STATE,ABBREVIATION,STATELIST) */
  border(STRING,STRING,STRINGLIST)

/*highlow(STATE,ABBREVIATION,POINT,HEIGHT,POINT,HEIGHT) */
  highlow(STRING,STRING,STRING,INTEGER,STRING,INTEGER)

/*mountain(STATE,ABBREVIATION,NAME,HEIGHT) */
  mountain(STRING,STRING,STRING,REAL)

/*lake(NAME,AREA,STATELIST) */
  lake(STRING,REAL,STRINGLIST)

/*road(NUMBER,STATELIST) */
  road(STRING,STRINGLIST)

INCLUDE "tpreds.pro"
INCLUDE "menu2.pro"


/**************************************************************
	Access to the database
****************************************************************/

PREDICATES		/* membership of a list */
  member(STRING,STRINGLIST)

CLAUSES
  member(X,[X|_]).
  member(X,[_|L]):-member(X,L).

PREDICATES
  db(ENT,ASSOC,ENT,STRING,STRING)
  ent(ENT,STRING)

INCLUDE "geobase.inc" /* Include parser + scanner + eval*/

CLAUSES
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
  db(height,of,point,HEIGHT,POINT):-	highlow(_,_,_,_,POINT,H),str_int(HEIGHT,H),!.
  db(height,of,point,HEIGHT,POINT):-	highlow(_,_,POINT,H,_,_),str_int(HEIGHT,H),!.

  /* Relationships about mountains */
  db(mountain,in,state,MOUNT,STATE):-	mountain(STATE,_,MOUNT,_).
  db(state,with,mountain,STATE,MOUNT):-	mountain(STATE,_,MOUNT,_).
  db(height,of,mountain,HEIGHT,MOUNT):-	mountain(_,_,MOUNT,H1),str_int(HEIGHT,H1).

  /* Relationships about lakes */
  db(lake,in,state,LAKE,STATE):-	lake(LAKE,_,LIST),member(STATE,LIST).
  db(state,with,lake,STATE,LAKE):-	lake(LAKE,_,LIST),member(STATE,LIST).
  db(area,of,lake,AREA,LAKE):-		lake(LAKE,A1,_),str_real(AREA,A1).

  /* Relationships about roads */
  db(road,in,state,ROAD,STATE):-	road(ROAD,LIST),member(STATE,LIST).
  db(state,with,road,STATE,ROAD):-	road(ROAD,LIST),member(STATE,LIST).

  db(E,in,continent,VAL,usa):-		ent(E,VAL).
  db(name,of,_,X,X):-			bound(X).


