#lang racket/gui
(require 2htdp/universe)
(require 2htdp/image)
(require "sound.ss")
(define (play-goat-sound)
  (background-play-sound-file "goat.wav"))
(define (play-button-sound)
  (background-play-sound-file "button.wav"))
(define (clap-sound)
  (background-play-sound-file "clap.wav"))
;----------------------------------------------------------<defination-part>---------------------------------------------------------------------------------
(define fbl 
  (bitmap "abcd/easy/capture.png"))
(define fbu 
  (bitmap "abcd/easy/capture1.png"))
(define liked #f)

(define goat-image (bitmap/file "b.png"))
(define goat-remaining 24)
(define goat-eaten 0)
(define tiger-image (bitmap/file "a.jpg") )
(define goat (bitmap/file "goat.png"))
(define error1 (bitmap/file "abcd/easy/error.png"))

(define tiger (bitmap/file "leopard.png"))
(define tiger-remaining 2)
(define tiger-turn 0)
(define goat-turn 1)
(define turn tiger-turn)
(define terminated #f)
(define (end w)
  (if terminated #t #f))
(define play-button (bitmap/file "play.png") )
(define instr-button (bitmap/file "instr.png") )
(define instruction (bitmap/file "instruction.jpg"))
(define back-button (bitmap/file "back.png"))
(define play-store (bitmap "abcd/easy/playstore.png"))
(define start #t)
(define play #f)
(define how-to #f)
(define (error-sound)
  (background-play-sound-file "error.wav"))
  

(define board-image (bitmap/file "b.jpg") )
(define back (bitmap/file "tiger.jpg") )
(define (background) 
  (place-image 
   (text (string-append "Goats eaten: " (number->string goat-eaten)) 20 "indigo") 530 90
   (place-image (text (string-append "Goats to be placed: " (number->string goat-remaining)) 20 "indigo") 
                550 50 (underlay/xy (rectangle 650 650 "solid" "white") 10 10 board-image ))))


(define(make-2d-vector r c)
  (build-vector r (λ(x)(make-vector c #f))))

(define(vector-2d-ref vec r c)
  (vector-ref (vector-ref vec r) c))

(define(vector-2d-copy vec)
  (build-vector (vector-length vec)(λ(x)(vector-copy (vector-ref vec x)))))

(define(vector-2d-set! vec r c val)
  (vector-set! (vector-ref vec r) c val))

(define (concat l) (foldr append `() l))
;---------------------------------------------------------<macros>------------------------------------------------------------
(define-syntax lc
  (syntax-rules (: <- @)
    [(lc exp : var <- lexp) (map (lambda (var) exp) lexp)]
    [(lc exp : @ guard) (if guard (list exp) `())]
    [(lc exp : @ guard qualifier ...) 
     (concat (lc (lc exp : qualifier ...) : guard))]
    [(lc exp : var <- lexp qualifier ...) 
     (concat (lc (lc exp :  qualifier ... ) : var <- lexp))]))

(define-syntax for 
  (syntax-rules ( : )
    [(for init : bexp : change : statements)
     (begin
       init
       (define(loop)
         (cond[bexp (begin
                      statements
                      change
                      (loop))]))
       (loop))]))

;-------------------------------------------------------</macros>--------------------------------------------------------------------------------
(define (get-pos coord)
  (let* ((x (car coord))
         (y (cdr coord)))
    (cond ( (and (> x 123) (< x 510) (> y 110) (< y 480))
            (cons (+ (quotient (- x 118) 79) 2) (+ (quotient (- y 98) 79) 2)))
          ( (and (> x 54) (< x 94) (> y 170) (< y 260)) (cons 0 3))
          ( (and (> x 30) (< x 80) (> y 260) (< y 350)) (cons 0 4))
          ( (and (> x 54) (< x 94) (> y 350) (< y 440)) (cons 0 5))
          ( (and (> x 94) (< x 136) (> y 230) (< y 280)) (cons 1 3))
          ( (and (> x 80) (< x 138) (> y 280) (< y 330)) (cons 1 4))
          ( (and (> x 94) (< x 136) (> y 330) (< y 380)) (cons 1 5))
          ( (and (> x 190) (< x 280) (> y 33) (< y 73)) (cons 3 0))
          ( (and (> x 280) (< x 370) (> y 12) (< y 62)) (cons 4 0))
          ( (and (> x 370) (< x 460) (> y 33) (< y 73)) (cons 5 0))
          ( (and (> x 250) (< x 300) (> y 73) (< y 113)) (cons 3 1))
          ( (and (> x 300) (< x 350) (> y 60) (< y 110)) (cons 4 1))
          ( (and (> x 350) (< x 400) (> y 73) (< y 113)) (cons 5 1))
          ( (and (> x 510) (< x 610) (> y 190) (< y 420)) 
            (cons (- 8 (car (get-pos (cons (- (* 324 2) x) y)))) 
                  (cdr (get-pos (cons (- (* 324 2) x) y)))))
          ( (and (> x 190) (< x 450) (> y 490) (< y 600)) 
            (cons (car (get-pos (cons x (- (* 305 2) y)))) 
                  (- 8 (cdr (get-pos(cons  x (- (* 305 2) y)))))))
          (else (cons 0 0)))))

;----------------------------------------------------------------------------------------------------------------------------------------          

(define (get-coord pos)
  (let* ((x (car pos))
         (y (cdr pos)))
    (cond ((and (> x 1) (< x 7) (> y 1) (< y 7))
           (cons (round (+ 170 (* (/ 315 4) (- x 2))))
                 (round (+ 150 (* (/ 315 4) (- y 2))))))
          ( (and (= x 3) (= y 0)) (cons 234 53))
          ( (and (= x 4) (= y 0)) (cons 324 37))
          ( (and (= x 3) (= y 1)) (cons 274 94))
          ( (and (= x 4) (= y 1)) (cons 324 88))
          ( (and (= x 5) (= y 0)) (cons 420 53))
          ( (and (= x 5) (= y 1)) (cons 376 94))
          ( (and (= x 0) (= y 3)) (cons 74 214))
          ( (and (= x 0) (= y 4)) (cons 54 307))
          ( (and (= x 0) (= y 5)) (cons 74 396))
          ( (and (= x 1) (= y 3)) (cons 116 255))
          ( (and (= x 1) (= y 4)) (cons 110 305))
          ( (and (= x 1) (= y 5)) (cons 116 353))
          ( (and (> x 6) (< x 9) (> y 2) (< y 6)) 
            (cons (- (* 2 324) (car (get-coord (cons (- 8 x) y)))) 
                  (cdr (get-coord (cons (- 8 x) y)))))
          ( (and (> y 6) (< y 9) (> x 2) (< x 6)) 
            (cons  (car (get-coord (cons x (- 8 y)))) (- (* 2 305) 
                                                         (cdr (get-coord (cons x (- 8 y)))))))
          )))




;----------------------------------------------------</defination-part>--------------------------------------------------------------------------------


(define board%
  (class object%
    (init-field (row 9))
    (init-field (column 9))
    (init-field (tiger-list '()))
    (init-field (goat-list '()))
    (init-field (grid (make-2d-vector 9 9)))
    (super-new)
    
    (define/public (add2-tiger-list t)
      (set! tiger-list (cons t tiger-list))
      (vector-2d-set! grid (car (get-pos (get-field pos t))) 
                      (cdr (get-pos (get-field pos t))) #\T))
    
    (define/public (add2-goat-list t)
      (set! goat-list (cons t goat-list))
      (vector-2d-set! grid (car (get-pos (get-field pos t))) 
                      (cdr (get-pos (get-field pos t))) #\G))
    
    
    (map (lambda (x) (vector-2d-set!  grid (car x) (cdr x)  #\X)) 
         (remove*  (lc (cons x y): x <-'(2 6) y <- '(2 6)) 
                   (lc (cons x y): x <-'(0 1 2 6 7 8) y <- '(0 1 2  6 7 8))))
    
    (define/public (delete-element-from-list posn)
      (cond ((eq? (vector-2d-ref grid (car posn) (cdr posn)) #\T)
             (begin
               (set! tiger-list (remove (get-tiger posn) tiger-list))
               (vector-2d-set! grid (car posn) (cdr posn) #f)))
            ((eq? (vector-2d-ref grid (car posn) (cdr posn)) #\G)
             (begin
               (if (or (eq? play #\T) 
                       (and (eq? play #\G) 
                            (= turn tiger-turn)))
                   (begin 
                     (set! goat-eaten (+ goat-eaten 1))
                     (play-goat-sound))
                   (void))
               (set! goat-list (remove (get-goat posn) goat-list))
               (vector-2d-set! grid (car posn) (cdr posn) #f)))))
    
    
    (define/public (get-goat posn)
      (car (filter (lambda (x) (equal? posn (send x pos-g ))) goat-list)))
    
    (define/public (get-tiger posn)
      (car (filter (lambda (x) (equal? posn (send x pos-t))) tiger-list)))
    ))

(define my-board (new board%)) 

(define goat%
  (class object%
    (init-field pos) ;coordinates
    (define/public (pos-g) (get-pos pos)) ;position
    (define (pos-sum) (+ (car (pos-g)) (cdr (pos-g))))
    (super-new)
    
    (send my-board add2-goat-list this)
    ;(set! goat-remaining (- goat-remaining 1))
    (define/public (can-move x)
      (let* ((a (car x))
             (b (cdr x))
             (c (car (pos-g)))
             (d (cdr (pos-g))))
        (if(even? (pos-sum))
           (and
            (not (or
                  (no-path? a b c d)
                  (no-path? b a d c)
                  (no-path? (- 8 a) b (- 8 c) d)
                  (no-path? a (- 8 b) c (- 8 d))
                  (no-path? (- 8 b) a (- 8 d) c)
                  (no-path? b (- 8 a) d (- 8 c))
                  (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                  (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
            (member  (cons (abs(- a c)) (abs (- b d))) 
                     (list (cons 0 1) (cons 1 0) (cons 1 1)))
            (not (vector-2d-ref (get-field grid my-board) a b)))
           (and 
            (not (or
                  (no-path? a b c d)
                  (no-path? b a d c)
                  (no-path? (- 8 a) b (- 8 c) d)
                  (no-path? a (- 8 b) c (- 8 d))
                  (no-path? (- 8 b) a (- 8 d) c)
                  (no-path? b (- 8 a) d (- 8 c))
                  (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                  (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
            (or
             (and 
              (= 1 (abs(- a c)))
              (= 0 (abs(- b d))))
             (and 
              (= 0 (abs(- a c)))
              (= 1 (abs(- b d)))))
            (not(vector-2d-ref (get-field grid my-board) a b)))
           )))))

(make-object goat% (get-coord (cons 3 3)))
(make-object goat% (get-coord (cons 3 4)))
(make-object goat% (get-coord (cons 3 5)))
(make-object goat% (get-coord (cons 4 5)))
(make-object goat% (get-coord (cons 5 5)))
(make-object goat% (get-coord (cons 5 4)))
(make-object goat% (get-coord (cons 5 3)))
(make-object goat% (get-coord (cons 4 3)))
(set! goat-remaining (- goat-remaining 8))

(define tiger%
  (class object%
    (init-field pos)
    (define/public (pos-t) (get-pos pos))
    (super-new)    
    (send my-board add2-tiger-list this)
    ))



;------------------------------------------------------------------------------------------------------------------------------------------------------------------------
(define ( list-of-next-valid-move-tiger grid)
  (lc (cons tiger (cons c d)) : 
      tiger <- (lc (cons a b) : 
                   a <- (list 0 1 2 3 4 5 6 7 8)
                   b <- (list 0 1 2 3 4 5 6 7 8)
                   @ (eq?(vector-2d-ref grid a b) #\T))
      c <- (list 0 1 2 3 4 5 6 7 8)
      d <- (list 0 1 2 3 4 5 6 7 8)
      @ (let* ((a (car tiger))
               (b (cdr tiger)))
          (cond [(even?(+ a b))
                 (or(and  (not (or
                                (no-path? a b c d)
                                (no-path? b a d c)
                                (no-path? (- 8 a) b (- 8 c) d)
                                (no-path? a (- 8 b) c (- 8 d))
                                (no-path? (- 8 b) a (- 8 d) c)
                                (no-path? b (- 8 a) d (- 8 c))
                                (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                                (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                          (member (cons (abs(- a c))(abs (- b d)))(list (cons 0 1)(cons 1 0)(cons 1 1)))
                          (eq?(vector-2d-ref grid c d) #f))
                    (and (not (or
                               (no-path? a b c d)
                               (no-path? b a d c)
                               (no-path? (- 8 a) b (- 8 c) d)
                               (no-path? a (- 8 b) c (- 8 d))
                               (no-path? (- 8 b) a (- 8 d) c)
                               (no-path? b (- 8 a) d (- 8 c))
                               (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                               (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                         (member (cons (abs(- a c))(abs (- b d)))(list (cons 0 2)(cons 2 0)(cons 2 2)))
                         (eq?(vector-2d-ref grid c d) #f)
                         (eq?(vector-2d-ref  grid (/ (+ c a) 2)(/ (+ d b)2)) #\G))
                    (and (eq?(vector-2d-ref grid c d) #f)
                         (member (cons (abs(- a c))(abs (- b d)))(list (cons 1 2)(cons 2 1)))
                         (special-case-for-tiger a b c d grid)))]
                [(odd?(+ a b)) 
                 (or(and (not (or
                               (no-path? a b c d)
                               (no-path? b a d c)
                               (no-path? (- 8 a) b (- 8 c) d)
                               (no-path? a (- 8 b) c (- 8 d))
                               (no-path? (- 8 b) a (- 8 d) c)
                               (no-path? b (- 8 a) d (- 8 c))
                               (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                               (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                         (member (cons (abs(- a c))(abs (- b d)))(list (cons 0 1)(cons 1 0)))
                         (eq?(vector-2d-ref grid c d) #f))
                    (and (not (or
                               (no-path? a b c d)
                               (no-path? b a d c)
                               (no-path? (- 8 a) b (- 8 c) d)
                               (no-path? a (- 8 b) c (- 8 d))
                               (no-path? (- 8 b) a (- 8 d) c)
                               (no-path? b (- 8 a) d (- 8 c))
                               (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                               (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                         (member (cons (abs(- a c))(abs (- b d)))(list (cons 0 2)(cons 2 0)))
                         (eq?(vector-2d-ref grid c d) #f)
                         (eq?(vector-2d-ref grid (/ (+ c a) 2)(/ (+ d b)2)) #\G))
                    (and (eq?(vector-2d-ref grid c d) #f)
                         (member (cons (abs(- a c))(abs (- b d)))(list (cons 1 2)(cons 2 1)))
                         (special-case-for-tiger a b c d grid)))]))))

(define (special-case-for-tiger a b c d grid)
  (cond ((equal? (cons a b) (cons 5 0)) (and 
                                         (equal? (cons c d) (cons 4 2))
                                         
                                         (eq? (vector-2d-ref grid 5 1) #\G)))
        ((equal? (cons a b) (cons 3 0)) (and 
                                         (equal? (cons c d) (cons 4 2))
                                         (eq? (vector-2d-ref grid 3 1) #\G)))
        ((equal? (cons a b) (cons 0 3)) (and 
                                         (equal? (cons c d) (cons 2 4))
                                         (eq? (vector-2d-ref grid 1 3) #\G)))
        ((equal? (cons a b) (cons 0 5)) (and 
                                         (equal? (cons c d) (cons 2 4))
                                         (eq? (vector-2d-ref grid 1 5) #\G)))
        ((equal? (cons a b) (cons 3 8)) (and 
                                         (equal? (cons c d) (cons 4 6))
                                         (eq? (vector-2d-ref grid 3 7) #\G)))
        ((equal? (cons a b) (cons 5 8)) (and 
                                         (equal? (cons c d) (cons 4 6))
                                         (eq? (vector-2d-ref grid 5 7) #\G)))
        ((equal? (cons a b) (cons 8 5)) (and 
                                         (equal? (cons c d) (cons 6 4))
                                         (eq? (vector-2d-ref grid 7 5) #\G)))
        ((equal? (cons a b) (cons 8 3)) (and 
                                         (equal? (cons c d) (cons 6 4))
                                         (eq? (vector-2d-ref grid 7 3) #\G)))
        ((equal? (cons a b) (cons 4 6)) (or
                                         (and 
                                          (equal? (cons c d) (cons 5 8))
                                          (eq? (vector-2d-ref grid 5 7) #\G))
                                         (and 
                                          (equal? (cons c d) (cons 3 8))
                                          (eq? (vector-2d-ref grid 3 7) #\G))))
        ((equal? (cons a b) (cons 6 4)) (or
                                         (and 
                                          (equal? (cons c d) (cons 8 5))
                                          (eq? (vector-2d-ref grid 7 5) #\G))
                                         (and 
                                          (equal? (cons c d) (cons 8 3))
                                          (eq? (vector-2d-ref grid 7 3) #\G))))
        ((equal? (cons a b) (cons 2 4)) (or
                                         (and 
                                          (equal? (cons c d) (cons 0 5))
                                          (eq? (vector-2d-ref grid 1 5) #\G))
                                         (and 
                                          (equal? (cons c d) (cons 0 3))
                                          (eq? (vector-2d-ref grid 1 3) #\G))))
        ((equal? (cons a b) (cons 4 2)) (or
                                         (and 
                                          (equal? (cons c d) (cons 3 0))
                                          (eq? (vector-2d-ref grid 3 1) #\G))
                                         (and 
                                          (equal? (cons c d) (cons 5 0))
                                          (eq? (vector-2d-ref grid 5 1) #\G))))
        (else #f)))


(define ( list-of-next-valid-move-goat grid)
  (lc (cons goat (cons c d)) : 
      goat <- (lc (cons a b) : 
                  a <- (list 0 1 2 3 4 5 6 7 8)
                  b <- (list 0 1 2 3 4 5 6 7 8)
                  @ (eq?(vector-2d-ref grid a b) #\G))
      c <- (list 0 1 2 3 4 5 6 7 8)
      d <- (list 0 1 2 3 4 5 6 7 8)
      @ (let* ((a (car goat))
               (b (cdr goat)))
          (cond [(even?(+ a b)) (and  (not (or
                                            (no-path? a b c d)
                                            (no-path? b a d c)
                                            (no-path? (- 8 a) b (- 8 c) d)
                                            (no-path? a (- 8 b) c (- 8 d))
                                            (no-path? (- 8 b) a (- 8 d) c)
                                            (no-path? b (- 8 a) d (- 8 c))
                                            (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                                            (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                                      (member (cons (abs(- a c))(abs (- b d)))
                                              (list (cons 0 1)(cons 1 0)(cons 1 1)))
                                      (eq?(vector-2d-ref grid c d) #f))]
                [(odd?(+ a b)) (and (not (or
                                          (no-path? a b c d)
                                          (no-path? b a d c)
                                          (no-path? (- 8 a) b (- 8 c) d)
                                          (no-path? a (- 8 b) c (- 8 d))
                                          (no-path? (- 8 b) a (- 8 d) c)
                                          (no-path? b (- 8 a) d (- 8 c))
                                          (no-path? (- 8 a) (- 8 b) (- 8 c) (- 8 d))
                                          (no-path? (- 8 b) (- 8 a) (- 8 d) (- 8 c))))
                                    (member (cons (abs(- a c))(abs (- b d)))(list (cons 0 1)(cons 1 0)))
                                    (eq?(vector-2d-ref grid c d) #f))]))))    

(define (can-eat-list grid)
  (filter (lambda (x) 
            (let* ((a (caar x))
                   (b (cdar x))
                   (c (cadr x))
                   (d (cddr x)))
              (or
               (and (=(- (abs a) (abs c)) 2)
                    (=(- (abs b) (abs d)) 2))
               (and (=(- (abs a) (abs c)) 2)
                    (=(- (abs b) (abs d)) 0))
               (and (=(- (abs a) (abs c)) 0)
                    (=(- (abs b) (abs d)) 2)))))
          (list-of-next-valid-move-tiger grid)))

(define (no-path? a b c d)
  (cond ((equal? (cons a b) (cons 4 0)) (or
                                         (equal? (cons c d) (cons 3 1))
                                         (equal? (cons c d) (cons 5 1))
                                         (equal? (cons c d) (cons 2 2))
                                         (equal? (cons c d) (cons 6 2))))
        ((equal? (cons a b) (cons 3 0))
         (equal? (cons c d) (cons 3 2)))
        ((equal? (cons a b) (cons 3 1)) (or
                                         (equal? (cons c d) (cons 2 2))
                                         (equal? (cons c d) (cons 3 2))
                                         (equal? (cons c d) (cons 1 3))
                                         (equal? (cons c d) (cons 3 3))
                                         (equal? (cons c d) (cons 4 0))))
        ((equal? (cons a b) (cons 3 2)) (or
                                         (equal? (cons c d) (cons 3 1))
                                         (equal? (cons c d) (cons 3 0))))
        ((equal? (cons a b) (cons 3 3)) (or
                                         (equal? (cons c d) (cons 3 1))
                                         (equal? (cons c d) (cons 1 3))))
        ((equal? (cons a b) (cons 2 2)) (or
                                         (equal? (cons c d) (cons 1 3))
                                         (equal? (cons c d) (cons 0 4))
                                         (equal? (cons c d) (cons 3 1))
                                         (equal? (cons c d) (cons 4 0))))
        (else #f)))

(define(place-tiger-virtually conf posn)
  (let* ((new (vector-2d-copy conf)))
    (begin(vector-2d-set! new (car posn)(cdr posn) #\T)
          new)))

(define(place-goat-virtually conf posn)
  (let* ((new (vector-2d-copy conf)))
    (begin(vector-2d-set! new (car posn)(cdr posn) #\G)
          new)))
(define(move-tiger-virtually conf posn1 posn2)
  (let* ((new (vector-2d-copy conf)))
    (begin (vector-2d-set! new (car posn1)(cdr posn1) #f)
           (vector-2d-set! new (car posn2)(cdr posn2) #\T)
           (cond[(member (cons (abs(- (car posn1)(car posn2)))  (abs (-(cdr posn1)(cdr posn2)))) 
                         (list (cons 0 2)(cons 2 0)(cons 2 2)))
                 (vector-2d-set! new (/(+(car posn1)(car posn2))2)(/(+(cdr posn1)(cdr posn2))2) #f)]
                [(member (cons (abs(- (car posn1)(car posn2)))  (abs (-(cdr posn1)(cdr posn2)))) 
                         (list (cons 1 2)(cons 2 1)))
                 (special-move new (car posn1) (cdr posn1) (car posn2) (cdr posn2))]
                )
           new)))
(define (special-move grid a b c d)
  (cond ((and (equal? (cons a b) (cons 5 0)) 
              (equal? (cons c d) (cons 4 2))
              (eq? (vector-2d-ref grid 5 1) #\G))
         (vector-2d-set! grid 5 1 #f))       
        ((and
          (equal? (cons a b) (cons 3 0))  
          (equal? (cons c d) (cons 4 2))
          (eq? (vector-2d-ref grid 3 1) #\G))
         (vector-2d-set! grid 3 1 #f))
        ((and 
          (equal? (cons a b) (cons 0 3)) 
          (equal? (cons c d) (cons 2 4))
          (eq? (vector-2d-ref grid 1 3) #\G))
         (vector-2d-set! grid 1 3 #f))
        ((and (equal? (cons a b) (cons 0 5))  
              (equal? (cons c d) (cons 2 4))
              (eq? (vector-2d-ref grid 1 5) #\G))
         (vector-2d-set! grid 1 5 #f))
        ((and (equal? (cons a b) (cons 3 8))
              (equal? (cons c d) (cons 4 6))
              (eq? (vector-2d-ref grid 3 7) #\G))
         (vector-2d-set! grid 3 7 #f))
        ((and (equal? (cons a b) (cons 5 8))  
              (equal? (cons c d) (cons 4 6))
              (eq? (vector-2d-ref grid 5 7) #\G))
         (vector-2d-set! grid 5 7 #f))
        ((and (equal? (cons a b) (cons 8 5)) 
              (equal? (cons c d) (cons 6 4))
              (eq? (vector-2d-ref grid 7 5) #\G))
         (vector-2d-set! grid 7 5 #f))
        ((and (equal? (cons a b) (cons 8 3)) 
              (equal? (cons c d) (cons 6 4))
              (eq? (vector-2d-ref grid 7 3) #\G))
         (vector-2d-set! grid 7 3 #f))
        ((and (equal? (cons a b) (cons 4 6)) 
              (or
               (and 
                (equal? (cons c d) (cons 5 8))
                (eq? (vector-2d-ref grid 5 7) #\G))
               (and 
                (equal? (cons c d) (cons 3 8))
                (eq? (vector-2d-ref grid 3 7) #\G))))
         (vector-2d-set! grid c 7 #f))
        ((and (equal? (cons a b) (cons 6 4)) 
              (or
               (and 
                (equal? (cons c d) (cons 8 5))
                (eq? (vector-2d-ref grid 7 5) #\G))
               (and 
                (equal? (cons c d) (cons 8 3))
                (eq? (vector-2d-ref grid 7 3) #\G))))
         (vector-2d-set! grid 7 d #f))
        ((and (equal? (cons a b) (cons 2 4)) 
              (or
               (and 
                (equal? (cons c d) (cons 0 5))
                (eq? (vector-2d-ref grid 1 5) #\G))
               (and 
                (equal? (cons c d) (cons 0 3))
                (eq? (vector-2d-ref grid 1 3) #\G))))
         (vector-2d-set! grid 1 d #f))
        ((and (equal? (cons a b) (cons 4 2)) 
              (or
               (and 
                (equal? (cons c d) (cons 3 0))
                (eq? (vector-2d-ref grid 3 1) #\G))
               (and 
                (equal? (cons c d) (cons 5 0))
                (eq? (vector-2d-ref grid 5 1) #\G))))
         (vector-2d-set! grid c 1 #f))
        ))

(define(move-goat-virtually conf posn1 posn2)
  (let* ((new (vector-2d-copy conf)))
    (begin (vector-2d-set! new (car posn1)(cdr posn1) #f)
           (vector-2d-set! new (car posn2)(cdr posn2) #\G)
           new)))
(define(list-of-next-valid-pos-to-put-tiger conf)
  (lc (cons x y) :
      x <- (list 0 1 2 3 4 5 6 7 8)
      y <- (list 0 1 2 3 4 5 6 7 8)
      @ (equal? (vector-2d-ref conf x y) #f)))

(define(list-of-next-valid-pos-to-put-goat conf)
  (lc (cons x y) :
      x <- (list 0 1 2 3 4 5 6 7 8)
      y <- (list 0 1 2 3 4 5 6 7 8)
      @ (equal? (vector-2d-ref conf x y) #f)))

(define (place-tiger)
  (cond ((= tiger-remaining 2) (make-object tiger% (get-coord (cons 4 4)))
                               (set! tiger-remaining 1) )
        ((= tiger-remaining 1) (if (eq? (vector-2d-ref (get-field grid my-board) 2 4) #f)
                                   (make-object tiger% (get-coord (cons 2 4)))
                                   (make-object tiger% (get-coord (cons 4 2))))
                               (set! tiger-remaining 0))
        ((= tiger-remaining 0)(take-step 
                               (cdr (alphabeta (get-field grid my-board) 2 -inf.0 +inf.0 #t goat-remaining 0))))))

(define (place-goat)
  (take-step (cdr (alphabeta (get-field grid my-board) 2 -inf.0 +inf.0 #f goat-remaining tiger-remaining))))



(define (take-step conf)
  ;(display conf)
  (for (define i 0): (< i 9): (set! i (+ i 1)):
    (for (define j 0): (< j 9): (set! j (+ j 1)):
      (cond ((and (not (vector-2d-ref conf i j))
                  (vector-2d-ref (get-field grid my-board) i j))
             (send my-board delete-element-from-list (cons i j)))
            ((and (vector-2d-ref conf i j)
                  (not (vector-2d-ref (get-field grid my-board) i j)))
             (if (eq? (vector-2d-ref conf i j) #\T)
                 (begin
                   (make-object tiger% (get-coord (cons i j)))
                   (if (> tiger-remaining 0)
                       (set! tiger-remaining (- tiger-remaining 1))
                       (void)))
                 (begin 
                   (make-object goat% (get-coord (cons i j)))
                   (if (> goat-remaining 0)
                       (set! goat-remaining (- goat-remaining 1))
                       (void)))))))))
(define (no-of-tiger-outside conf)
  (define num 0)
  (begin
    (for (define i 2): (< i 7): (set! i (+ i 1)):
      (for (define j 2): (< j 7): (set! j (+ j 1)):
        (if (eq? (vector-2d-ref conf i j) #T)
            (set! num (+ num 1))
            (void))))
  (- 2 tiger-remaining num))) 
  
(define (arrange list)
  (let* ((eating-list (filter eating? list)))
    (append eating-list (remove eating-list list))))

(define (eating? x) 
  (let* ((a (caar x))
         (b (cdar x))
         (c (cadr x))
         (d (cddr x)))
    (or
     (and (= (abs (- a c)) 2)
          (= (abs (- b d)) 2))
     (and (= (abs (- a c)) 2)
          (= (abs (- b d)) 0))
     (and (= (abs (- a c)) 0)
          (= (abs (- b d)) 2))
     (and (= (abs (- a c)) 1)
          (= (abs (- b d)) 2))
     (and (= (abs (- a c)) 2)
          (= (abs (- b d)) 1)))))

;-------------------------------------------------------<alpha-beta pruning :)>-------------------------------------------------------------------------
(define (alphabeta conf depth alpha beta player  no-of-goat-remaining no-of-tiger-remaining)
  (define best-conf conf)
  (cond [(or (= depth 0) (terminate? conf no-of-tiger-remaining no-of-goat-remaining))
         (cons (evaluate conf) best-conf)]
        [(eq? player #t)
         (if (> no-of-tiger-remaining 0) 
             (let* ((children (list-of-next-valid-pos-to-put-tiger conf)))
               (define (f list-pos)
                 (if (null? list-pos)
                     (cons alpha best-conf)
                     (begin
                       (let* ((new-conf (place-tiger-virtually conf (car list-pos)))
                              (score (car (alphabeta new-conf (- depth 1) alpha beta #f no-of-goat-remaining (- no-of-tiger-remaining 1)))))
                         ; (display (cons score new-conf))
                         (if (> score alpha)
                             (begin
                               (set! alpha score)
                               (set! best-conf new-conf))
                             (void))
                         (cond
                           [(<= beta alpha) (cons alpha best-conf)]
                           [else(f (cdr list-pos))])))))
               (f children))
             (let* ((children (arrange (list-of-next-valid-move-tiger conf))))
               (define (f list-move)
                 (if (null? list-move)
                     (cons alpha best-conf)
                     (begin
                       (let* ((new-conf (move-tiger-virtually conf (caar list-move) (cdar list-move)))
                              (score (car(alphabeta new-conf (- depth 1) alpha beta #f no-of-goat-remaining no-of-tiger-remaining ))))
                         ;(display (cons score new-conf))
                         (if (eating? (car list-move))
                             (set! score (+ score 50))
                             (void))
                         (if (> score alpha)
                             (begin
                               (set! alpha score)
                               (set! best-conf new-conf))
                             (void))
                         (cond
                           [ (<= beta alpha) (cons alpha best-conf)]
                           [else(f (cdr list-move))])))))
               (f children)))]
        ;----------------------------------------------------------------------------------------------------------------------------------------------------        
        [(eq? player #f)
         (if (> no-of-goat-remaining  0) 
             (let* ((children (list-of-next-valid-pos-to-put-goat conf)))
               (define (f list-pos)
                 (if (null? list-pos)
                     (cons beta best-conf)
                     (begin
                       (let* ((new-conf (place-goat-virtually conf (car list-pos)))
                              (score (car (alphabeta new-conf (- depth 1) alpha beta #t (- no-of-goat-remaining 1)  no-of-tiger-remaining))))
                         (if (< score beta)
                             (begin
                               (set! beta score)
                               (set! best-conf new-conf))
                             (void))
                         (cond
                           [ (<= beta alpha) (cons beta best-conf)]
                           [else(f (cdr list-pos))])))))
               (f children))
             (let* ((children (list-of-next-valid-move-goat conf)))
               (define (f list-move)
                 (if (null? list-move)
                     (cons beta best-conf)
                     (begin
                       (let* ((new-conf (move-goat-virtually conf (caar list-move) (cdar list-move)))
                              (score (car (alphabeta new-conf (- depth 1) alpha beta #t no-of-goat-remaining  no-of-tiger-remaining))))
                         (if (< score beta)
                             (begin
                               (set! beta score)
                               (set! best-conf new-conf))
                             (void))
                         (cond
                           [(<= beta alpha) (cons beta best-conf)]
                           [else(f (cdr list-move))])))))
               (f children)))]))
;--------------------------------------------------</alpha-beta pruning :)>-----------------------------------------------------------------------------------------------

(define (evaluate conf) ;evaluate the current grid
  (+
   (- (* 3 (length (can-eat-list conf)))
      16
      (* 0.2 (no-of-tiger-outside conf)))
   (length (list-of-next-valid-move-tiger conf))))

(define (terminate? grid tiger-remaining goat-remaining)
  (cond[(= goat-eaten 12)
        #\T]
       [(and (= tiger-remaining 0)(= (length (list-of-next-valid-move-tiger grid))0)) #\G]
       [else #f]))


;----------------------------------------------------(graphics part)-----------------------------------------------------------------------------------------        
(define original-pos (cons 0 0))
(define goat-on-hold #f)
(define tiger-on-hold #f)
(define (error x)
  error1)
(define (show-error-message)
  (big-bang 0
          (name "unexpected error")
          (to-draw error)))
(define (change W x y event)
  (cond 
    (start 
     (if (mouse=? event "button-down")
         (cond ((and (< (abs (- x 100)) 50) (< (abs (- y 200)) 50))
                (set! play #\T)
                (set! start #f)
                (play-button-sound))
               ((and (< (abs (- x 600)) 30) (< (abs (- y 50)) 10))
                (set! liked #t))
               ((and (< (abs (- x 100)) 75) (< (abs (- y 600)) 30))
                (error-sound)
                (show-error-message))
               ((and (< (abs (- x 300)) 50) (< (abs (- y 200)) 50))
                (set! play #\G)
                (set! start #f)
                (play-button-sound))
               ((and (< (abs (- x 200)) 80) (< (abs (- y 300)) 10))
                (set! how-to #t)
                (set! start #f)
                (play-button-sound))
               )
         (void)))
    (how-to 
     (if (mouse=? event "button-down")
         (cond ((and (< (abs (- x 40)) 100) (< (abs (- y 40)) 20))
                (set! start #t)
                (set! how-to #f)
                (play-button-sound)))
         (void)))
    ((eq? play #\T) 
     (if (= turn goat-turn)
         (if (= goat-remaining 0)
             (cond  [(mouse=? event "button-down") 
                     (if  (eq? (vector-2d-ref (get-field grid my-board) 
                                              (car (get-pos (cons x y))) (cdr (get-pos (cons x y)))) #\G)
                          (begin 
                            (set! goat-on-hold (send my-board get-goat (get-pos (cons x y))))
                            (set! original-pos (get-field pos goat-on-hold))
                            )
                          (void))]
                    [ (and (mouse=? event "drag") goat-on-hold)
                      (set-field! pos goat-on-hold (cons x y))]
                    
                    [ (and (mouse=? event "button-up") goat-on-hold)
                      (set-field! pos goat-on-hold original-pos)
                      (if (send goat-on-hold can-move (get-pos (cons x y)))
                          (begin
                            (set-field! pos goat-on-hold (get-coord (get-pos (cons x y))))
                            (vector-2d-set! (get-field grid my-board) 
                                            (car (get-pos original-pos)) (cdr (get-pos original-pos)) #f)
                            (vector-2d-set! (get-field grid my-board) 
                                            (car (get-pos (cons x y))) (cdr (get-pos (cons x y))) #\G)
                            (if (eq? (terminate? (get-field grid my-board) tiger-remaining goat-remaining) #\G)
                                (set! terminated #\G)
                                (void))
                            (set! goat-on-hold #f)
                            (set! turn tiger-turn)
                            )
                          (set! goat-on-hold #f))])
             
             
             (cond  [(mouse=? event "button-down")
                     (if  (eq? (vector-2d-ref (get-field grid my-board) 
                                              (car (get-pos (cons x y))) (cdr (get-pos (cons x y)))) #f)
                          (begin
                            (make-object goat%  (get-coord (get-pos (cons x y))))
                            (set! goat-remaining (- goat-remaining 1))
                            ;(display "goat")
                            (set! turn tiger-turn))
                          (void))]))
         ;(display (cons x y))]))
         (begin
           (place-tiger)
           (if (eq? (terminate? (get-field grid my-board) tiger-remaining goat-remaining ) #\T)
               (set! terminated #\T)
               (void))
           (set! turn goat-turn))))
    ((eq? play #\G) 
     (if (= turn tiger-turn)
         (if (= tiger-remaining 0)
             (cond  [(mouse=? event "button-down") 
                     (if  (eq? (vector-2d-ref (get-field grid my-board) 
                                              (car (get-pos (cons x y))) (cdr (get-pos (cons x y)))) #\T)
                          (begin 
                            (set! tiger-on-hold (send my-board get-tiger (get-pos (cons x y))))
                            (set! original-pos (get-field pos tiger-on-hold))
                            )
                          (void))]
                    [ (and (mouse=? event "drag") tiger-on-hold)
                      (set-field! pos tiger-on-hold (cons x y))]
                    
                    [ (and (mouse=? event "button-up") tiger-on-hold)
                      (set-field! pos tiger-on-hold original-pos)
                      (if (member (cons (get-pos original-pos) (get-pos (cons x y))) (list-of-next-valid-move-tiger (get-field grid my-board)))  
                          (begin
                            (take-step (move-tiger-virtually (get-field grid my-board) (get-pos original-pos) (get-pos (cons x y))))
                            
                            (if (eq? (terminate? (get-field grid my-board) tiger-remaining goat-remaining) #\T)
                                (set! terminated #\T)
                                (void))
                            (set! tiger-on-hold #f)
                            (set! turn goat-turn))
                          (set! tiger-on-hold #f))])
             
             
             (cond  [(mouse=? event "button-down")
                     (if  (eq? (vector-2d-ref (get-field grid my-board) 
                                              (car (get-pos (cons x y))) (cdr (get-pos (cons x y)))) #f)
                          (begin
                            (make-object tiger%  (get-coord (get-pos (cons x y))))
                            (set! tiger-remaining (- tiger-remaining 1))
                            ;(display "tiger")
                            (set! turn goat-turn))
                          (void))]))
         ;(display (cons x y))]))
         (begin
           (place-goat)
           (if (eq? (terminate? (get-field grid my-board) tiger-remaining goat-remaining ) #\G)
               (set! terminated #\G)
               (void))
           (set! turn tiger-turn))))))


(define (create-image W)
  (cond 
    (start (place-image (if liked fbu fbl) 600 50 
                        (place-image instr-button 200 300 
                                     (place-image play-button 200 100 
                                                  (place-image goat 100 200 
                                                               (place-image play-store 100 600 
                                                               (place-image tiger 300 200 back)))))))
    (how-to (place-image back-button 40 40 instruction))
    (play
     (define (place-goat li)
       (foldr (lambda(x l) (place-image goat-image (car (get-field pos x )) (cdr (get-field pos x )) l ))
              (background) li))
     (define (place-tiger li) 
       (foldr (lambda(x l) (place-image tiger-image (car (get-field pos x )) (cdr (get-field pos x )) l )) 
              (place-goat (get-field  goat-list my-board)) li))
     (cond ((eq? terminated #\T)
            (begin 
              (clap-sound)
             (place-image (text "Leopard won" 36 "indigo") 350 620 (place-tiger (get-field tiger-list my-board)))))
           ((eq? terminated #\G)
            (begin
              (clap-sound)
              (place-image (text "Goat won" 36 "indigo") 350 620 (place-tiger (get-field tiger-list my-board)))))
           (else (place-tiger (get-field tiger-list my-board)))))))



;------------------------------------------------------------------------------------------------------------------------------------------------------
(big-bang 0 
          (name "bagh-chal")
          (to-draw create-image)
          (on-mouse change)
          (stop-when end))



 