#lang racket
(require racket/date)
(require net/url)

(define days (hash "Monday," 0 "Tuesday," 1 "Wednesday," 2 "Thursday," 3 "Friday," 4 "Saturday," -2 "Sunday," -1))
(define subjects (hash "SUMMARY:HMIN322\r" "SUMMARY:HMIN322 : Codage et compression multimedia\n"
                       "SUMMARY:HMIN316\r" "SUMMARY:HMIN316 : Son et musique\n"
                       "SUMMARY:HMIN321\r" "SUMMARY:HMIN321 : Société virtuelle\n"
                       "SUMMARY:HMIN319\r" "SUMMARY:HMIN319 : Jeux sérieux\n"
                       "SUMMARY:HMIN317\r" "SUMMARY:HMIN317 : Vision, réalités virtuelles et augmentées\n"
                       "SUMMARY:HMIN320\r" "SUMMARY:HMIN320 : Moteur de jeux\n"))

#|Proxy pour la fac|#
;; (current-proxy-servers '(("http" "162.38.218.204" 3128)))

(define (add-days-to-date d n)
  (date-display-format 'iso-8601)
  (date->string (seconds->date (+ (find-seconds 0 0 0 (caddr d) (cadr d) (car d)) (* n 86400))))) 

(define (dates-of-current-week)
  (date-display-format 'iso-8601)
  (let ([today (string-split (date->string (current-date)) "-")])
    (date-display-format 'american)
    (let ([today (map string->number today)]
          [elapsed (hash-ref days (car (string-split (date->string (current-date)) " ")))])
      `(,(add-days-to-date today (- elapsed))
        ,(add-days-to-date today (+ 6 (- elapsed)))))))

;;726 = codage et compression multimedia

(define (make-url-from-bounds bounds)
  (string->url (string-join `("http://www.tom2ade.univ-montp2.fr/jsp/custom/modules/plannings/anonymous_cal.jsp?resources=726,731,738,748,763,770,1514&projectId=1&calType=ical&firstDate="
                              ,(car bounds)
                              "&lastDate="
                              ,(cadr bounds)) "")))

(define (create-new-schedule)
  (current-locale "C")
  (let ([p (get-pure-port (make-url-from-bounds (dates-of-current-week)))]
        [out (open-output-file "out.ics" #:mode 'binary #:exists 'replace)])
    (let f ()
      (let ([data (read-line p)])
        (cond [(eof-object? data)
                (close-output-port out)
                (displayln "exported schedule as \"out.ics\"")]
               [else
                (write-string (hash-ref subjects data (string-replace data "\r" "\n")) out)
                (f)])))))

(create-new-schedule)
