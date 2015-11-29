#lang racket

(require "heap.rkt")
(require "utils.rkt")

(provide c-bwt)
(provide d-bwt)
(provide get-index)

(define (bwt s)
  (letrec ([size (length s)] [v (make-vector size)] [s-vec (list->vector s)])
    (for ([i (in-range size)])
      (vector-set! v i i))
    (last-column (sort (vector->list v) (lambda (x y) (compare x y s-vec size))) s-vec size)))

(define (compare x y l size)
  (let f ([x x] [y y])
    (if (eq? (* 2 size) x)
        #f
        (let ([x-byte (vector-ref l (modulo x size))] [y-byte (vector-ref l (modulo y size))])
          (if (= x-byte y-byte)
              (f (+ 1 x) (+ 1 y))
              (< x-byte y-byte))))))

(define (last-column l s size)
  (let ([index 0])
    (list (map (lambda (i)
                 (when (eq? i 0)
                   (set! index (- size (length (member i l)))))
                 (vector-ref s (modulo (- (+ i size) 1) size))) l) index)))

(define (make-precedings s)
  (let ([v (make-vector 256 0)])
    (vector-map (lambda (i)
                  (vector-set! v i (+ 1 (vector-ref v i)))
                  (list i (- (vector-ref v i) 1)))
                s)))

(define (make-less-than s)
  (let ([sorted (list->vector (sort s <))] [lt (make-vector 256 -1)])
    (for ([i (in-range (vector-length sorted))])
      (let ([x (vector-ref sorted i)])
        (when (eq? (vector-ref lt x) -1)
          (vector-set! lt x i))))
    lt))

(define (sort-data s)
  (let ([heap (make-heap (length s) <)])
    (let f ([s s])
      (if (empty? s)
          (sort-heap heap)
          (begin
            (insert heap (car s))
            (f (cdr s)))))))

(define (reverse-bwt s first out)
  (letrec ([size (length s)] [vec (make-vector size)] [prec (make-precedings (list->vector s))] [lt (make-less-than s)])
    (let f ([n 0] [last first])
      (if (= size n)
          (write-bytes (list->bytes (vector->list vec)) out)
          (letrec ([x (vector-ref prec last)]
                   [current (+ (cadr x) (vector-ref lt (car x)))])
            (let ([last-char (vector-ref prec last)])
              (vector-set! prec last (list (car last-char) (- (cadr last-char) 1))))
            (vector-set! vec (- size n 1) (car x))
            (f (+ 1 n) current))))))

(define (write-result l out)
  (if (> 8 (length l))
      l
      (let ((n (to-integer (take l 8))))
        (write-bytes (bytes n) out)
        (write-result (drop l 8) out)))) 

(define (read-file in)
  (bytes->list (port->bytes in)))

(define (get-index in)
  (let ([size (read-byte in)])
    (let f ([data '()])
      (if (eq? (length data) size)
          (to-integer data)
          (f (append data (binary-with-encoding-length (to-binary (read-byte in)) 8)))))))

(define (c-bwt in out)
  (letrec ([x (bwt (read-file in))] [index (cadr x)] [size (if (= 0 index) 8 (inexact->exact (* 8 (ceiling (/ (integer-length index) 8)))))])
    (write-byte size out)
    (write-result (binary-with-encoding-length (to-binary index) size) out)
    (car x)))

(define (d-bwt out s index)
  (reverse-bwt s index out)
  (close-output-port out))