#lang racket

(provide make-heap)
(provide insert)
(provide first)
(provide remove-first)
(provide sort-heap)
(provide size)
(provide first-and-remove)

(define (make-heap size proc)
  (list (box 1) proc (make-vector (+ 1 size) 0)))

(define (at heap i)
  (vector-ref (caddr heap) i))

(define (size heap)
  (unbox (car heap)))

(define (compare heap x y)
  ((cadr heap) x y))

(define (set heap i value)
  (vector-set! (caddr heap) i value))

(define (insert heap value)
  (set heap (size heap) value)
  (up heap)
  (set-box! (car heap) (+ 1 (size heap))))


(define (remove-first heap)
  (set-box! (car heap) (- (unbox (car heap)) 1))
  (set heap 1 (at heap (size heap)))
  (down heap))

(define (first heap)
  (at heap 1))

(define (first-and-remove heap)
  (let ((x (first heap)))
    (remove-first heap)
    x))

(define (up heap)
    (let f ((i (size heap)))
      (if (eq? i 1)
          (void)
          (begin
            (if (father-greater? heap i)
                (switch-with-father heap i)
                (void))
            (f (father i))))))

(define (father i)
  (floor (/ i 2)))

(define (left-son i)
  (* i 2))

(define (right-son i)
  (+ (* i 2) 1))

(define (father-greater? heap i)
  (compare heap (at heap i) (at heap (father i))))

(define (switch-with-father heap i)
  (let ((temp (at heap i)))
    (set heap i (at heap (father i)))
    (set heap (father i) temp)))


(define (down heap)
    (let f ([i 1])
      (let ((ss (smaller-son heap i)))
        (if (> ss (size heap))
            (void)
            (begin
              (if (compare heap (at heap ss) (at heap i))
                  (begin
                    (switch-with-father heap ss)
                    (if (< (+ 1(* 2 ss)) (vector-length (caddr heap)))
                        (f ss)
                        (void)))
                  (void)))))))

(define (smaller-son heap i)
  (if (> (right-son i) (size heap))
      (left-son i)
      (void))
  (if (compare heap (at heap (left-son i)) (at heap (right-son i)))
      (left-son i)
      (right-son i)))

(define (sort-heap heap)
  (let ((v (make-vector (- (size heap) 1))))
    (let f ()
      (if (eq? (size heap) 1)
          v
          (begin
            (vector-set! v (- (vector-length v) (- (size heap) 1)) (first heap))
            (remove-first heap)
            (f))))))
