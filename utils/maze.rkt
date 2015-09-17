#lang racket/gui

(define cell-count-width 1200)
(define cell-count-height 700)
(define cell-width 1)
(define cell-height 1)

(struct wall (i j right? up?))

(define cells #(#()))
(define walls '())

(define (get-width)
  (* cell-width cell-count-width))

(define (get-height)
  (* cell-height cell-count-height))

(define (at matrix x y)
  (vector-ref (vector-ref matrix x) y))

(define (set-matrix! matrix x y v)
  (vector-set! (vector-ref matrix x) y v))


(define frame (new frame%
                   [label "Maze"]
                   [width (get-width)]
                   [height (get-height)]))

(define game-canvas%
  (class canvas%
    (define/override (on-event event)
      void)
    (define/override (on-paint)
      (draw (get-game-canvas-dc)))
    (super-new)))

(define (draw dc)
  (void))

(define (get-game-canvas-dc)
  (send game-canvas get-dc))

(define game-canvas (new game-canvas% [parent frame]))

(define (draw-cell x y value)
  ;; (send (get-game-canvas-dc) set-brush "yellow" 'opaque)
  (send (get-game-canvas-dc) set-pen "white" 1 'transparent)
  (send (get-game-canvas-dc) draw-rectangle x y cell-width cell-height))

(define (draw-grid dc)
  (for* ([i (in-range cell-count-width)]
         [j (in-range cell-count-height)])
    (draw-cell (* i cell-width) (* j cell-height) (at cells i j) dc)))

(define (create-walls x y)
  (list (wall x y 1 0)
        (wall x y -1 0)
        (wall x y 0 1)
        (wall x y 0 -1)))

(define (face-wall-empty? wall)
  (define (neighbour-count x y)
    (- 4
       (at cells (max 0 (- x 1)) y)
       (at cells (min (- cell-count-width 1) (+ x 1)) y)
       (at cells x (max 0 (- y 1)))
       (at cells x (min (- cell-count-height 1) (+ y 1)))))
  
  (let ([x (+ (wall-i wall) (wall-right? wall))]
        [y (+ (wall-j wall) (wall-up? wall))])
    (cond [(or (< x 0) (>= x cell-count-width)) #f]
          [(or (< y 0) (>= y cell-count-height)) #f]
          [(= 0 (at cells x y)) #f]
          [(> (neighbour-count x y) 1) #f]
          [else #t])))

(define (move-loop walls x y)
  ;; (let ([walls (shuffle (append walls (create-walls x y)))])
  (let ([walls (append (shuffle (create-walls x y)) walls)])
    (let f ([wall (car walls)] [walls walls])
      (cond [(empty? walls) (send game-canvas on-paint)]
            [(face-wall-empty? wall)
             (let ([x (+ (wall-i wall) (wall-right? wall))]
                   [y (+ (wall-j wall) (wall-up? wall))])
               (set-matrix! cells x y 0)
               (draw-cell (* x cell-width) (* y cell-height) (at cells x y))
               (move-loop (cdr walls) x y))]
            [else (f (cadr walls) (cdr walls))]))))

(define (start)
  (set! cells (let ([v (make-vector (get-width) 1)])
                (vector-map (λ (i)
                              (make-vector (get-height) 1)) v)))

  ;; (draw-grid dc)
  (send frame show #t)
  (thread
   (lambda ()
     (sleep 1)
     (send (get-game-canvas-dc) set-brush "black" 'opaque)
     (send (get-game-canvas-dc) draw-rectangle 0 0 (get-width) (get-height))
     (send (get-game-canvas-dc) set-brush "white" 'opaque)
     (move-loop '() 0 0))))

(start)
