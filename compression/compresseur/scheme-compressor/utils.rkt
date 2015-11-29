#lang racket

(provide to-binary)
(provide to-integer)
(provide binary-with-encoding-length)
(provide pair)
(provide size-of)

(define (to-binary n)
  (let div ([n n])
    (let ([n n] [rest (modulo n 2)])
      (if (equal? (- n rest) 0)
          (list rest)
          (append (div (/ (- n rest) 2)) (list rest))))))

(define (to-integer l)
    (let f ((n 0) (l (reverse l)) (result 0))
      (if (empty? l)
          result
          (f (+ 1 n) (cdr l) (+ result (* (car l) (expt 2 n)))))))

(define (binary-with-encoding-length l encoding-length)
  (append (make-list (- encoding-length (length l)) 0) l))

(define (pair x y)
  (append x (list y)))

(define (size-of in)
  (let f ([n 0])
    (if (void? (file-position in n))
        n
        (f (+ 1 n)))))