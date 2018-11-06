;    Initialization file for TinySCHEME 1.41

;;;; Utility to ease macro creation
(define (macro-expand form)
     ((eval (get-closure-code (eval (car form)))) form))

(define (macro-expand-all form)
   (if (macro? form)
      (macro-expand-all (macro-expand form))
      form))

(define *compile-hook* macro-expand-all)


(macro (unless form)
     `(if (not ,(cadr form)) (begin ,@(cddr form))))

(macro (when form)
     `(if ,(cadr form) (begin ,@(cddr form))))

; DEFINE-MACRO Contributed by Andy Gaynor
(macro (define-macro dform)
  (if (symbol? (cadr dform))
    `(macro ,@(cdr dform))
    (let ((form (gensym)))
      `(macro (,(caadr dform) ,form)
         (apply (lambda ,(cdadr dform) ,@(cddr dform)) (cdr ,form))))))

; Utilities for math. Notice that inexact->exact is primitive,
; but exact->inexact is not.
(define exact? integer?)
(define (inexact? x) (and (real? x) (not (integer? x))))
(define (abs n) (if (>= n 0) n (- n)))
(define (exact->inexact n) (* n 1.0))
(define (<> n1 n2) (not (= n1 n2)))

; min and max must return inexact if any arg is inexact; use (+ n 0.0)
(define (max . lst)
  (foldr (lambda (a b)
           (if (> a b)
             (if (exact? b) a (+ a 0.0))
             (if (exact? a) b (+ b 0.0))))
         (car lst) (cdr lst)))
(define (min . lst)
  (foldr (lambda (a b)
           (if (< a b)
             (if (exact? b) a (+ a 0.0))
             (if (exact? a) b (+ b 0.0))))
         (car lst) (cdr lst)))


(define (string . charlist)
     (list->string charlist))

(define (list->string charlist)
     (let* ((len (length charlist))
            (newstr (make-string len))
            (fill-string!
               (lambda (str i len charlist)
                    (if (= i len)
                         str
                         (begin (string-set! str i (car charlist))
                         (fill-string! str (+ i 1) len (cdr charlist)))))))
          (fill-string! newstr 0 len charlist)))

(define (string-fill! s e)
     (let ((n (string-length s)))
          (let loop ((i 0))
               (if (= i n)
                    s
                    (begin (string-set! s i e) (loop (succ i)))))))

(define (string->list s)
     (let loop ((n (pred (string-length s))) (l '()))
          (if (= n -1)
               l
               (loop (pred n) (cons (string-ref s n) l)))))

(define (string->anyatom str pred)
     (let* ((a (string->atom str)))
       (if (pred a) a
         (error "string->xxx: not a xxx" a))))

(define (string->number str . base)
    (let ((n (string->atom str (if (null? base) 10 (car base)))))
        (if (number? n) n #f)))

(define (anyatom->string n pred)
  (if (pred n)
      (atom->string n)
      (error "xxx->string: not a xxx" n)))

(define (number->string n . base)
    (atom->string n (if (null? base) 10 (car base))))



(define (list . x) x)

(define (foldr f x lst)
     (if (null? lst)
          x
          (foldr f (f x (car lst)) (cdr lst))))


(define (map proc . lists)
  (if (null? lists)
      (apply proc)
      (if (null? (car lists))
        '()
        (let* ((unz (apply unzip1-with-cdr lists))
               (cars (car unz))
               (cdrs (cdr unz)))
          (cons (apply proc cars) (apply map (cons proc cdrs)))))))

(define (for-each proc . lists)
  (if (null? lists)
      (apply proc)
      (if (null? (car lists))
        #t
        (let* ((unz (apply unzip1-with-cdr lists))
               (cars (car unz))
               (cdrs (cdr unz)))
          (apply proc cars) (apply map (cons proc cdrs))))))

(define (list-tail x k)
    (if (zero? k)
        x
        (list-tail (cdr x) (- k 1))))

(define (list-ref x k)
    (car (list-tail x k)))

(define (last-pair x)
    (if (pair? (cdr x))
        (last-pair (cdr x))
        x))




;;;;; atom? and equal? written by a.k

;;;; atom?
(define (atom? x)
  (not (pair? x)))

;;;;    equal?
(define (equal? x y)
     (cond
          ((pair? x)
               (and (pair? y)
                    (equal? (car x) (car y))
                    (equal? (cdr x) (cdr y))))
          ((string? x)
               (and (string? y) (string=? x y)))
          (else (eqv? x y))))

;;;; (do ((var init inc) ...) (endtest result ...) body ...)
;;
(macro do
  (lambda (do-macro)
    (apply (lambda (do vars endtest . body)
             (let ((do-loop (gensym)))
               `(letrec ((,do-loop
                           (lambda ,(map (lambda (x)
                                           (if (pair? x) (car x) x))
                                      `,vars)
                             (if ,(car endtest)
                               (begin ,@(cdr endtest))
                               (begin
                                 ,@body
                                 (,do-loop
                                   ,@(map (lambda (x)
                                            (cond
                                              ((not (pair? x)) x)
                                              ((< (length x) 3) (car x))
                                              (else (car (cdr (cdr x))))))
                                       `,vars)))))))
                  (,do-loop
                    ,@(map (lambda (x)
                             (if (and (pair? x) (cdr x))
                               (car (cdr x))
                               '()))
                        `,vars)))))
      do-macro)))

;;;; generic-member
(define (generic-member cmp obj lst)
  (cond
    ((null? lst) #f)
    ((cmp obj (car lst)) lst)
    (else (generic-member cmp obj (cdr lst)))))

(define (memq obj lst)
     (generic-member eq? obj lst))
(define (memv obj lst)
     (generic-member eqv? obj lst))
(define (member obj lst)
     (generic-member equal? obj lst))

;;;; generic-assoc
(define (generic-assoc cmp obj alst)
     (cond
          ((null? alst) #f)
          ((cmp obj (caar alst)) (car alst))
          (else (generic-assoc cmp obj (cdr alst)))))

(define (assq obj alst)
     (generic-assoc eq? obj alst))
(define (assv obj alst)
     (generic-assoc eqv? obj alst))
(define (assoc obj alst)
     (generic-assoc equal? obj alst))

(define (acons x y z) (cons (cons x y) z))

;;;; Handy for imperative programs
;;;; Used as: (define-with-return (foo x y) .... (return z) ...)
(macro (define-with-return form)
     `(define ,(cadr form)
          (call/cc (lambda (return) ,@(cddr form)))))

;;;; Simple exception handling
;
;    Exceptions are caught as follows:
;
;         (catch (do-something to-recover and-return meaningful-value)
;              (if-something goes-wrong)
;              (with-these calls))
;
;    "Catch" establishes a scope spanning multiple call-frames
;    until another "catch" is encountered.
;
;    Exceptions are thrown with:
;
;         (throw "message")
;
;    If used outside a (catch ...), reverts to (error "message)

(define *handlers* (list))

(define (push-handler proc)
     (set! *handlers* (cons proc *handlers*)))

(define (pop-handler)
     (let ((h (car *handlers*)))
          (set! *handlers* (cdr *handlers*))
          h))

(define (more-handlers?)
     (pair? *handlers*))

(define (throw . x)
     (if (more-handlers?)
          (apply (pop-handler))
          (apply error x)))

(macro (catch form)
     (let ((label (gensym)))
          `(call/cc (lambda (exit)
               (push-handler (lambda () (exit ,(cadr form))))
               (let ((,label (begin ,@(cddr form))))
                    (pop-handler)
                    ,label)))))

(define *error-hook* throw)


;;;;; Definition of MAKE-ENVIRONMENT, to be used with two-argument EVAL

(macro (make-environment form)
     `(apply (lambda ()
               ,@(cdr form)
               (current-environment))))

(define-macro (eval-polymorphic x . envl)
  (display envl)
  (let* ((env (if (null? envl) (current-environment) (eval (car envl))))
         (xval (eval x env)))
    (if (closure? xval)
      (make-closure (get-closure-code xval) env)
      xval)))

(gc-verbose #f)
