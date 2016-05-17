frontchar(A, C, Rest):-
        sub_atom(A,0,1,X,C),
        sub_atom(A,1,X,0,Rest).

str_real(A,R):-
        nonvar(A),!,
        atom_string(A,S),
        number_string(R,S).

str_real(A,R):-
        nonvar(R),!,
        number_string(R,S),
        atom_string(A,S).
