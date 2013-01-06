; Template-ul newworld are in plus campul directie, necesar pentru situatiile in
; care trebuie sa sar mai multi oameni
(deftemplate newworld
        (multislot limit) ; max size (orizontala, verticala)
        (multislot ball) ; minge
        (multislot men) ; pozitiile una dupa alta, x y -
        (slot id) ; identificator pt lume
        (multislot moves) ; lista mutarilor, initial vida
        (multislot direction)
        )
;Regula de stergere atunci cand nu avem solutii.
(defrule clean
(declare (salience 2))
?w <- (newworld (limit $?l))
=>
(retract ?w)
)
;Regula pentru stergerea faptelor ramase, cu exceptia Win
;cand am castigat
(defrule winclean
(declare (salience 6))
?wn<- (win (id ?id))
?wls<- (newworld (id ?id))
=>
(retract ?wls)
)

;Regula create preia faptul world, asa cum este dat , si il "converteste"
;la newworld. Defapt, sterge faptul vechi cu world, creeaza un fapt 
;identic cu newworld , cu directie vida.
;Prioritatea cea mai mare - se executa primul.
(defrule create
(declare (salience 7))
?nw<-(world (limit ?ll ?lc) (ball ?lb ?lcl) (men $?a)  (id ?id) (moves $?b))
=>
(assert (newworld (limit ?ll ?lc) (ball ?lb ?lcl) (men $?a)  (id ?id) (
moves $?b)(direction )))
(retract ?nw)
)
;Regula de castig, daca am ajuns pe ultima linie ( adica dimMax -1 ) am castigat.
(defrule win
    (declare (salience 5))
    ?wrl<-(newworld (limit ?ll ?lc) (ball ?lb ?lcl) (men $?a)  (id ?id) 
(moves $?b)(direction ?drcx ?drcy))
    (test (= (- ?ll 1) ?lb))
    =>
    (assert (win (id ?id) (moves $?b ?lb ?lcl -)))
    (retract ?wrl)
)
;Regula nbs creeaza fapte noi cu lumi in care mingea este pe oricare 
;din vecinii mingii initiale, care sunt oameni.
;updateaza directia si lista de miscari
;Stiu ca liniile depasesc 80 de caractere insa mi s-a parut ca asa
;e cel mai usor de inteles codul. ( Nu am impartit campul men).
(defrule nbs
     (declare (salience 3))
        ?wl<-(newworld (limit ?ll ?lc) (ball ?lb ?bc) (men $?mend ?xn ?yn - $?mens) 
 (id ?id)(moves $?b)(direction $?dr))
        =>
(if(and (= (+ ?lb 1) ?xn) (= ?bc ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball (+ ?lb 1) ?bc) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction +1 0)))
)

(if (and (= (- ?lb 1) ?xn) (= ?bc ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball (- ?lb 1) ?bc) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction -1 0)))
)

(if (and (= ?lb ?xn) (= (- ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  ?lb (- ?bc 1)) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction 0 -1)))
)

(if (and (= ?lb ?xn) (= (+ ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  ?lb (+ ?bc 1)) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction 0 +1)))
)

(if (and (= (- ?lb 1) ?xn) (= (+ ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  (- ?lb 1) (+ ?bc 1)) (men $?mend ?xn ?yn - $?mens)
 (id ?id) (moves $?b ?lb ?bc -) (direction -1 +1)))
)

(if (and (= (- ?lb 1) ?xn) (= (- ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  (- ?lb 1) (- ?bc 1)) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction -1 -1)))
)

(if (and (= (+ ?lb 1) ?xn) (= (- ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  (+ ?lb 1) (- ?bc 1)) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction +1 -1)))
)

(if (and (= (+ ?lb 1) ?xn) (= (+ ?bc 1) ?yn))
then
(assert (newworld (limit ?ll ?lc) (ball  (+ ?lb 1) (+ ?bc 1)) (men $?mend ?xn ?yn - $?mens) 
(id ?id) (moves $?b ?lb ?bc -) (direction +1 +1)))
)
)
;Regula " de sarit".
;Se executa in momentul in care mingea se afla pe un om
;Datorita recursivitatii ( salience-ul ei e mai mare decat al nbs),
;se va executa pana cand "nu mai poate sari" in directia respectiva
;creeaza lumi noi cu noile pozitii, le sterge pe cele vechi.
        (defrule moving
        (declare (salience 4))
?w <- (newworld (limit ?ll ?lc) (ball ?lb ?bc ) (men $?a ?lm ?cm - $?b) (id ?id)
 (moves $?c)(direction ?drx ?dry))
(test (and (=  ?lb ?lm ) (=  ?bc ?cm)))
=>
(assert (newworld (limit ?ll ?lc) (ball (+ ?lb ?drx) (+ ?bc ?dry)) (men $?a $?b) 
(id ?id) (moves $?c) (direction ?drx ?dry)))
(retract ?w)
)
