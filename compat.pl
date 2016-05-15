bound(X):-nonvar(X).

frontchar(A, C, Rest):-
        sub_atom(A,0,1,X,C),
        sub_atom(A,1,X,0,Rest).

str_real(A,R):-
        atom_string(A,S),
        number_string(R,S).
