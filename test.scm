(define (contains? lst i)
  (if (null? lst) #f
      (or (eq? (car lst) i)
	  (contains? (cdr lst) i))))

(define (weekend? day)
  (contains? '(sat sun)
	     day))

(define (workday? day)
  (contains? '(mon tue wed thu fri)
	     day))


(define cnt 0)

(define (p var)
  (display var)
  (display "\n"))


(define (timer)
  (set! cnt (+ cnt 1)))
;;  (display "timer\n"))


(display "load init...\n")
;(p (weekend? 'mon))
;(p (weekend? 'sun))
;(p (workday? 'mon))
;(p (workday? 'sun))
