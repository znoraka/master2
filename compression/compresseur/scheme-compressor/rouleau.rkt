#lang racket

(require "lzw-compressor.rkt")
(require "lzw-decompressor.rkt")
(require "huffman.rkt")
(require "b-w.rkt")
(require "move-to-front.rkt")

(define (compress-file input output bwt)
  (compress
   (open-input-file input #:mode 'binary)
   (open-output-file "temp.bin" #:mode 'binary #:exists 'replace))
  (let ([output (open-output-file output #:mode 'binary #:exists 'replace)])
    (write-byte bwt output)
    (h-compress (open-input-file "temp.bin" #:mode 'binary)
                (open-input-file "temp.bin" #:mode 'binary)
                output))
  (delete-file "temp.bin"))

(define (decompress-file input output)
  (letrec ([in (open-input-file input #:mode 'binary)]
           [bwt (read-byte in)]
           [out-lzw (if (= bwt 0) output "temp1.bin")])
    (h-decompress in (open-output-file "temp.bin" #:mode 'binary #:exists 'replace))
    (decompress 
     (open-input-file "temp.bin" #:mode 'binary)
     (open-output-file out-lzw #:mode 'binary #:exists 'replace))
    (when (= 1 bwt)
      (decompress-bwt "temp1.bin" output)
      (delete-file "temp1.bin"))
    (delete-file "temp.bin")))

(define (compress-bwt input output)
  (let ([out (open-output-file "temp1.bin" #:mode 'binary #:exists 'replace)])
    (c-mtf (c-bwt (open-input-file input #:mode 'binary) out) out))
  (compress-file "temp1.bin" output 1)
  (delete-file "temp1.bin"))

(define (decompress-bwt input output)
  (letrec ([in (open-input-file input #:mode 'binary)] [index (get-index in)])
    (d-bwt (open-output-file output #:mode 'binary #:exists 'replace) (d-mtf in) index)))

(define (display-help)
  (displayln "rouleau -c[ompress] input_file output_file")
  (displayln "rouleau -d[ecompress] input_file output_file")
  (displayln "rouleau -b[urrows-wheeler] input_file output_file"))

(define (main)
  (let ([args (current-command-line-arguments)])
    (cond
      [(< (vector-length args) 3) (display-help)]
      [(or
        (equal? (vector-ref args 0) "-c")
        (equal? (vector-ref args 0) "-compress"))
       (compress-file (vector-ref args 1) (vector-ref args 2) 0)]
      [(or
        (equal? (vector-ref args 0) "-d")
        (equal? (vector-ref args 0) "-decompress"))
       (decompress-file (vector-ref args 1) (vector-ref args 2))]
      [(or
        (equal? (vector-ref args 0) "-b")
        (equal? (vector-ref args 0) "-burrows-wheeler"))
       (compress-bwt (vector-ref args 1) (vector-ref args 2))]
      [else (display-help)])))

(main)

;(compress-bwt "/home/noe/Téléchargements/test.txt" "/home/noe/Téléchargements/out.bin")
;(display "compressed")
;(decompress-bwt "/home/noe/Téléchargements/bwt.bin" "/home/noe/Téléchargements/out.txt")