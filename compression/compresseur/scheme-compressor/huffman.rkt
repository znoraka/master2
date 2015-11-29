#lang racket

(require "heap.rkt")
(require "utils.rkt")

(provide h-compress)
(provide h-decompress)

(define (make-frequences in)
  (let f ([frequences (make-vector 256 0)])
    (let ((x (read-byte in)))
      (if (eof-object? x)
          frequences
          (let ((n x))
            (vector-set! frequences n (+ (vector-ref frequences n) 1))
            (f frequences))))))

(define (sort-frequences frequences)
  (let f ((l '()) (i 0))
    (if (eq? i (vector-length frequences))
        (map (lambda (x)
               (make-node (cadr x) (car x) '() '()))
             (sort l (lambda (x y)
                       (< (cadr x) (cadr y)))))
        (if (eq? (vector-ref frequences i) 0)
            (f l (+ i 1))
            (f (append l (list (list i (vector-ref frequences i)))) (+ i 1))))))

(define (make-huffman-heap l)
  (let ((v (make-heap (length l) (lambda (x y) (< (car x) (car y))))))
    (let f ((l l))
      (if (empty? l)
          v
          (begin
            (insert v (car l))
            (f (cdr l)))))))

(define (make-huffman l)
  (let f ([v (make-huffman-heap l)])
    (if (eq? (size v) 2)
        (first v)
        (let ([right (first-and-remove v)] [left (first-and-remove v)])
          (insert v (make-node (+ (car right) (car left)) '() left right))
          (f v)))))

(define (make-node freq char left right)
  (list freq char left right))

(define (make-dictionnary huffman)
  (let f ((huffman huffman) (s '()))
    (if (empty? (caddr huffman))
        (list (list (cadr huffman) s))
        (append (f (cadddr huffman) (append s '(0))) (f (caddr huffman) (append s '(1)))))))

(define (make-vector-dictionnary l)
  (let ([dico (make-vector 256)])
    (map (lambda (i)
           (vector-set! dico (car i) (cadr i)))
         l)
    dico))

(define (get-encoding-length frequences)
  (let ([m (vector-ref frequences 0)])
    (for ([i (in-range 256)])
      (set! m (max (vector-ref frequences i) m)))
    (inexact->exact (* 8 (ceiling (/ (/ (log m) (log 2)) 8))))))

(define (export-dico frequences encoding-length out)
  (write-result (binary-with-encoding-length (to-binary encoding-length) 8) out)
  (for ([i (in-range 256)])
    (when (> (vector-ref frequences i) 0)
          (write-result (binary-with-encoding-length (to-binary i) 8) out)
          (write-result (binary-with-encoding-length (to-binary (vector-ref frequences i)) encoding-length) out)))
  (write-result (binary-with-encoding-length (to-binary 0) (+ encoding-length 8)) out))

(define (write-result l out)
  (if (> 8 (length l))
      l
      (let ((n (to-integer (take l 8))))
        (write-bytes (bytes n) out)
        (write-result (drop l 8) out)))) 

(define (read-frequence in encoding-length)
  (let f ([data (binary-with-encoding-length (to-binary (read-byte in)) 8)])
    (if (= (length data) encoding-length)
        (to-integer data)
        (f (append data (binary-with-encoding-length (to-binary (read-byte in)) 8))))))

(define (parse-dico in)
  (let ([encoding-length (read-byte in)] [frequences (make-vector 256 0)])
    (let f ()
      (let ([index (read-byte in)] [freq (read-frequence in encoding-length)])
        (if (and (= 0 index) (= 0 freq))
            frequences
            (begin
              (vector-set! frequences index freq)
              (f)))))))

(define (read-data data in)
  (if (> (length data) 32)
      data
      (if (port-closed? in)
          data
          (let ([x (read-byte in)])
            (if (eof-object? x)
                (let ([to-skip (to-integer (drop data (- (length data) 8)))])
                  (close-input-port in)
                  (take data (- (length data) 8 to-skip)))
                (read-data (append data (binary-with-encoding-length (to-binary x) 8)) in))))))

(define (get-char-from-dico data dico)
  (if (empty? (caddr dico))
      (cons (cadr dico) data)
      (if (empty? data)
          '()
          (get-char-from-dico (cdr data)
                              (if (eq? (car data) 1)
                                  (caddr dico)
                                  (cadddr dico))))))

(define (encode in out dico)
  (let f ([data '()])
    (let ([x (read-byte in)])
      (if (eof-object? x)
          (let ([data (write-result data out)])
            (let ([remaining (- 8 (length data))])
              (write-byte (to-integer (append data (make-list remaining 1))) out)
              (write-byte remaining out)))
          (f (write-result (append data (vector-ref dico x)) out))))))

(define (h-compress in in1 out)
  (let ([frequences (make-frequences in)])
    (let ([dico (make-dictionnary 
                 (make-huffman 
                  (sort-frequences 
                   frequences)))])
      (export-dico frequences (get-encoding-length frequences) out)
      (encode in1 out (make-vector-dictionnary dico))
      (close-output-port out))))

(define (h-decompress in out)
  (let ([huffman (make-huffman
                  (sort-frequences
                   (parse-dico in)))])
    (let f ([data (read-data '() in)])
      (let ([res (get-char-from-dico data huffman)])
        (if (empty? res)
            (close-output-port out)
            (begin
              (write-byte (car res) out)
              (f (read-data (cdr res) in))))))))