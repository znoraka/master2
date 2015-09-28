#lang racket/gui

(define cell-count-width 440)
(define cell-count-height 300)
(define cell-width 3)
(define cell-height 3)

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
    (draw-cell (* i cell-width) (* j cell-height) (at cells i j))))

(define (create-walls x y)
  (list (wall x y 1 0)
        (wall x y -1 0)
        (wall x y 0 1)
        (wall x y 0 -1)))

(define (face-wall-empty? wall)
  (define (neighbour-count x y)
    (- 4
       (at cells (max 1 (- x 1)) y)
       (at cells (min (- cell-count-width 2) (+ x 1)) y)
       (at cells x (max 1 (- y 1)))
       (at cells x (min (- cell-count-height 2) (+ y 1)))))
  
  (let ([x (+ (wall-i wall) (wall-right? wall))]
        [y (+ (wall-j wall) (wall-up? wall))])
    (cond [(or (< x 1) (>= x (- cell-count-width 1))) #f]
          [(or (< y 1) (>= y (- cell-count-height 1))) #f]
          [(= 0 (at cells x y)) #f]
          [(> (neighbour-count x y) 1) #f]
          [else #t])))

(define (move-loop walls x1 y1 x2 y2)
  (draw-cell (* x1 cell-width) (* y1 cell-height) (at cells x1 y1))
  (draw-cell (* x2 cell-width) (* y2 cell-height) (at cells x2 y2))

  (let move-loop ([walls walls]
                  [x x1]
                  [y y1])
  (let ([walls (if (> (random 100) 80)
                   (append (shuffle (create-walls x y)) walls)
                   (shuffle (append walls (create-walls x y))))])
    (let f ([wall (car walls)] [walls walls])
      (cond [(face-wall-empty? wall)
             (let ([x (+ (wall-i wall) (wall-right? wall))]
                   [y (+ (wall-j wall) (wall-up? wall))])
               (set-matrix! cells x y 0)
               (draw-cell (* x cell-width) (* y cell-height) (at cells x y))
               (move-loop (cdr walls) x y))]
            [else (if (empty? (cdr walls))
                      ;; (draw-grid (get-game-canvas-dc))
                      (displayln "done")
                      (f (cadr walls) (cdr walls)))])))))

(define (draw-entrances x1 y1 x2 y2)
  (send (get-game-canvas-dc) set-brush "red" 'opaque)
  (draw-cell (* x1 cell-width) (* y1 cell-height) (at cells x1 y1))
  (draw-cell (* x2 cell-width) (* y2 cell-height) (at cells x2 y2)))

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
     (let ([x1 (inexact->exact (* cell-count-width 0.25))]
           [y1 0]
           [x2 (inexact->exact (* cell-count-width 0.75))]
           [y2 (- cell-count-height 1)])
       (draw-entrances x1 y1 x2 y2)
       (send (get-game-canvas-dc) set-brush "white" 'opaque)
       (move-loop '() x1 (+ y1 1) x2 (- y2 1))))))

(start)
