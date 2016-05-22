:- module(genealogist,
      [  ancestor_decendant/2,
         siblings/2,
         parent_child/2,
         father_child/2,
         mother_child/2
      ]).

ancestor_decendant(X, Y) :- parent_child(X, Y).
ancestor_decendant(X, Z) :- parent_child(X, Y), ancestor_decendant(Y, Z).

siblings(X, Y) :- parent_child(Z, X), parent_child(Z, Y), X @< Y.

parent_child(X, Y) :- mother_child(X, Y).
parent_child(X, Y) :- father_child(X, Y).

mother_child(trude, sally).

father_child(tom, sally).
father_child(tom, erica).
father_child(mike, tom).