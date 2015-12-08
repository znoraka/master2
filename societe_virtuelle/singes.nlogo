extensions [table]

globals [
  tick-count
  days
  turtle-id
  total-tick-count
  saluts-count
  leader
  i-count
  last-tick
]

turtles-own [
  strength
  min-strength
  leader-strength
  has-token?
  combat-ability
  name
  mates
  mates-with-count
  salut-count
  salut-count-list
  strengths-list
  last-strengths-mean
  last-saluts-mean
  charisma
  number-of-ticks-as-leader
  x
  y
]

to setup
  clear-all
  setup-monkeys
  setup-globals
  reset-ticks
  ask turtles [set-mates]
end

to setup-globals
  set tick-count 0
  set days 0
  set turtle-id 0
  set total-tick-count 0
  set saluts-count 0
end

;; plus le groupe est grand, plus la force apportée par un salut doit être faible
to setup-monkeys
  create-turtles initial-monkeys
    [
      setxy random-xcor random-ycor
      set color one-of base-colors
      set shape "ghost"
      set strength random 1000
      set combat-ability random 200
      set leader-strength strength
      set charisma random 100 - 50
      set min-strength 0
      set has-token? true
      set name word "turtle-" turtle-id
      set number-of-ticks-as-leader 0
      set-current-plot "Strength"
      create-temporary-plot-pen name
      set-current-plot "Saluts"
      create-temporary-plot-pen name
      set-current-plot "Time-as-leader"
      create-temporary-plot-pen name
      set salut-count 0
      set turtle-id turtle-id + 1
      set salut-count-list (list 0)
      set strengths-list (list 0)

      set x sin ((360 / (count turtles)) * i-count) * 6
      set y cos ((360 / (count turtles)) * i-count) * 6

      print x
      print y
      print "****"

      set i-count (i-count + 1)
    ]
end

;;;
;;; GO PROCEDURES
;;;

to go
  if display-relations? [ask turtles [erase-relations]]
  ask turtles [move]
  ask turtles [fight]
  ask turtles [check-salut]
  ask turtles [check-strength]
  ask turtles [decrease-leader-strength-memory]
  ask turtles [plot-strength]
  ask turtles [plot-saluts]
  ask turtles [plot-time-as-leader]
  if display-relations? [ask turtles [draw-relations]]
  time
  set leader turtles with-max [strength]
  ask leader [
    set number-of-ticks-as-leader number-of-ticks-as-leader + 1
  ]
  tick
  if total-tick-count >= ticks-per-simulation
  [
    set simulations-count (simulations-count - 1)
    export-results
    ifelse simulations-count > 0
    [
      setup
    ]
    [
      stop
    ]
  ]
end

to set-mates
  set mates-with-count (list)
  set mates turtles with [name != [name] of myself]
  let i 0
  let temp (list)
  let dict table:make
  foreach [self] of (turtles with [name != [name] of myself])
  [
    table:put dict ([name] of ?) (list ? 0 1)
  ]
  set mates-with-count dict
end

to move  ;; turtle procedure
  rt random-float 180 - 90
  if has-token?
  [
    face min-one-of mates [distance self]
  ]

  let monkeys mates in-radius 10 with-max [strength]
    if any? monkeys
    [
      let monkey one-of monkeys
      if abs ([strength] of monkey - strength) < 100
      [
        face monkey
      ]
    ]
  fd 1
end

to check-salut
  if has-token?
  [
    let monkeys mates in-radius 3 with-max [strength]
    if any? monkeys
    [
      let monkey one-of monkeys
      if [strength] of monkey > strength and [strength] of monkey > leader-strength
        [
          ask self [salut monkey]
        ]
    ]
  ]
end

to salut [monkey]
  set has-token? false
  set saluts-count (saluts-count + 1)
  ask monkey [set strength strength + (strength-per-salut / 20)]
  ask monkey [set salut-count (salut-count + 1)]
  ;ask patch-here [set pcolor blue]
  let temp table:get mates-with-count [name] of monkey
  table:put mates-with-count ([name] of monkey) (list (first temp) min (list 10 ((first (butfirst temp)) + 1)) 1)
end

to decrease-leader-strength-memory
  set leader-strength (leader-strength - memory-of-strength-decay)
  set strength (strength - memory-of-strength-decay / 2)
end

to fight
  ;if has-token?
  ;[
    let monkeys mates in-radius 5 with-max [strength]
    if any? monkeys
    [
      let monkey one-of monkeys
      if abs ([strength] of monkey - strength) < 50
        [
          ;ask patch-here [set pcolor red]
          ifelse random combat-ability + strength > random [combat-ability] of monkey + [strength] of monkey
          [
            set strength strength + (strength-per-salut * 1.5 / 20)
          ]
          [
            ask monkey [set strength strength - (strength / 4) + charisma]
          ]
        ]
    ]
  ;]
end

to check-strength
  if strength < min-strength
    [
      set strength min-strength
    ]
  if strength > 1000
  [
   set strength 1000
  ]
end


to time
  set tick-count (tick-count + 1)
  set total-tick-count (total-tick-count + 1)
  if tick-count > ticks-per-day
  [
    ask turtles [
      set last-saluts-mean ((mean salut-count-list) / saluts-count) * 100
      set last-strengths-mean (mean strengths-list)
    ]
    set tick-count 0
    ask turtles [set has-token? true]
    ask turtles [set salut-count-list lput salut-count salut-count-list]
    ask turtles [set strengths-list lput strength strengths-list]
    ask turtles [
      if length salut-count-list > 10
      [
        set salut-count-list butfirst salut-count-list
        foreach table:keys mates-with-count
        [
          let temp table:get mates-with-count ?
          ifelse first bf bf temp = 1
          [
            table:put mates-with-count ? (list (first temp) (first (butfirst temp)) 0)
          ]
          [
            table:put mates-with-count ? (list (first temp) (max (list ((first (butfirst temp)) - 1) 0)) 0)
          ]

        ]
      ]
      if length strengths-list > 10
      [
        set strengths-list butfirst strengths-list
      ]
    ]
    ask turtles [set salut-count 0]
    set saluts-count 0
  ]
end

to draw-relations
  let xtemp xcor
  let ytemp ycor

  let tempcolor [color] of self

  set size (strength / 200) + 1
  foreach table:keys mates-with-count
  [
    setxy x y
    let temp table:get mates-with-count ?
   ; print butfirst temp
    if saluts-count > 0
    [
      set color [color] of (first temp)
      set pen-size ((first (butfirst temp)) * 20 / (count turtles)) + 3
      pd
      setxy [x] of (first temp) [y] of (first temp)
      pu
    ]
  ]
  set color tempcolor
  setxy xtemp ytemp
end

to erase-relations
  let xtemp [xcor] of self
  let ytemp [ycor] of self
  if last-tick != total-tick-count
  [
    set pen-size 500
    pe
    setxy 0 0
    set last-tick total-tick-count
    pu
  ]
  setxy xtemp ytemp
end

to plot-strength
  set-current-plot "Strength"
  set-current-plot-pen name
  set-plot-pen-color color
  set-plot-x-range (total-tick-count - 1000) total-tick-count
  plotxy total-tick-count last-strengths-mean
end

to plot-saluts
  set-current-plot "Saluts"
  set-current-plot-pen name
  set-plot-pen-color color
  set-plot-x-range (total-tick-count - 1000) total-tick-count
  plot last-saluts-mean
end


to plot-time-as-leader
  if total-tick-count > 0
  [
    set-current-plot "Time-as-leader"
    set-current-plot-pen name
    set-plot-pen-color color
    set-plot-x-range (total-tick-count - 1000) total-tick-count
    plot (number-of-ticks-as-leader / total-tick-count) * 100
  ]
end

to export-results
  file-open (word "out1.txt")
  ask turtles [export-monkey]
  file-close
end

to export-monkey
  file-print (word (combat-ability / 2) " " (charisma + 50) " " ((number-of-ticks-as-leader / total-tick-count) * 100))
end
@#$#@#$#@
GRAPHICS-WINDOW
288
10
723
466
12
12
17.0
1
10
1
1
1
0
1
1
1
-12
12
-12
12
1
1
1
ticks
30.0

BUTTON
12
81
95
114
setup
setup
NIL
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

BUTTON
96
81
179
114
go
go
T
1
T
OBSERVER
NIL
NIL
NIL
NIL
1

SLIDER
7
37
276
70
initial-monkeys
initial-monkeys
2
100
9
1
1
NIL
HORIZONTAL

PLOT
746
11
1015
210
Strength
weeks
Strength
0.0
52.0
0.0
350.0
false
true
"" "set-plot-y-range 0 500"
PENS

SLIDER
14
134
202
167
strength-per-salut
strength-per-salut
0
1000
514
1
1
NIL
HORIZONTAL

PLOT
746
213
1015
413
Saluts
weeks
Saluts
0.0
100.0
0.0
100.0
false
true
"" ""
PENS

SLIDER
14
170
203
203
ticks-per-day
ticks-per-day
1
100
16
1
1
NIL
HORIZONTAL

SLIDER
13
211
202
244
memory-of-strength-decay
memory-of-strength-decay
0
100
18
1
1
NIL
HORIZONTAL

MONITOR
288
475
402
520
leader strength
[strength] of leader
17
1
11

MONITOR
411
475
541
520
leader combat-ability
[combat-ability] of leader
17
1
11

MONITOR
549
474
715
519
leader charisma
[charisma] of leader
17
1
11

SWITCH
13
253
194
286
display-relations?
display-relations?
0
1
-1000

MONITOR
725
474
848
519
leader name
[name] of leader
17
1
11

PLOT
12
300
277
499
Time-as-leader
NIL
NIL
0.0
10.0
0.0
100.0
false
true
"" ""
PENS

SLIDER
1026
10
1211
43
simulations-count
simulations-count
1
100
37
1
1
NIL
HORIZONTAL

SLIDER
1026
52
1211
85
ticks-per-simulation
ticks-per-simulation
1
50000
30000
1
1
NIL
HORIZONTAL

@#$#@#$#@
@#$#@#$#@
default
true
0
Polygon -7500403 true true 150 5 40 250 150 205 260 250

airplane
true
0
Polygon -7500403 true true 150 0 135 15 120 60 120 105 15 165 15 195 120 180 135 240 105 270 120 285 150 270 180 285 210 270 165 240 180 180 285 195 285 165 180 105 180 60 165 15

arrow
true
0
Polygon -7500403 true true 150 0 0 150 105 150 105 293 195 293 195 150 300 150

box
false
0
Polygon -7500403 true true 150 285 285 225 285 75 150 135
Polygon -7500403 true true 150 135 15 75 150 15 285 75
Polygon -7500403 true true 15 75 15 225 150 285 150 135
Line -16777216 false 150 285 150 135
Line -16777216 false 150 135 15 75
Line -16777216 false 150 135 285 75

bug
true
0
Circle -7500403 true true 96 182 108
Circle -7500403 true true 110 127 80
Circle -7500403 true true 110 75 80
Line -7500403 true 150 100 80 30
Line -7500403 true 150 100 220 30

butterfly
true
0
Polygon -7500403 true true 150 165 209 199 225 225 225 255 195 270 165 255 150 240
Polygon -7500403 true true 150 165 89 198 75 225 75 255 105 270 135 255 150 240
Polygon -7500403 true true 139 148 100 105 55 90 25 90 10 105 10 135 25 180 40 195 85 194 139 163
Polygon -7500403 true true 162 150 200 105 245 90 275 90 290 105 290 135 275 180 260 195 215 195 162 165
Polygon -16777216 true false 150 255 135 225 120 150 135 120 150 105 165 120 180 150 165 225
Circle -16777216 true false 135 90 30
Line -16777216 false 150 105 195 60
Line -16777216 false 150 105 105 60

car
false
0
Polygon -7500403 true true 300 180 279 164 261 144 240 135 226 132 213 106 203 84 185 63 159 50 135 50 75 60 0 150 0 165 0 225 300 225 300 180
Circle -16777216 true false 180 180 90
Circle -16777216 true false 30 180 90
Polygon -16777216 true false 162 80 132 78 134 135 209 135 194 105 189 96 180 89
Circle -7500403 true true 47 195 58
Circle -7500403 true true 195 195 58

circle
false
0
Circle -7500403 true true 0 0 300

circle 2
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240

cow
false
0
Polygon -7500403 true true 200 193 197 249 179 249 177 196 166 187 140 189 93 191 78 179 72 211 49 209 48 181 37 149 25 120 25 89 45 72 103 84 179 75 198 76 252 64 272 81 293 103 285 121 255 121 242 118 224 167
Polygon -7500403 true true 73 210 86 251 62 249 48 208
Polygon -7500403 true true 25 114 16 195 9 204 23 213 25 200 39 123

cylinder
false
0
Circle -7500403 true true 0 0 300

dot
false
0
Circle -7500403 true true 90 90 120

face happy
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 255 90 239 62 213 47 191 67 179 90 203 109 218 150 225 192 218 210 203 227 181 251 194 236 217 212 240

face neutral
false
0
Circle -7500403 true true 8 7 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Rectangle -16777216 true false 60 195 240 225

face sad
false
0
Circle -7500403 true true 8 8 285
Circle -16777216 true false 60 75 60
Circle -16777216 true false 180 75 60
Polygon -16777216 true false 150 168 90 184 62 210 47 232 67 244 90 220 109 205 150 198 192 205 210 220 227 242 251 229 236 206 212 183

fish
false
0
Polygon -1 true false 44 131 21 87 15 86 0 120 15 150 0 180 13 214 20 212 45 166
Polygon -1 true false 135 195 119 235 95 218 76 210 46 204 60 165
Polygon -1 true false 75 45 83 77 71 103 86 114 166 78 135 60
Polygon -7500403 true true 30 136 151 77 226 81 280 119 292 146 292 160 287 170 270 195 195 210 151 212 30 166
Circle -16777216 true false 215 106 30

flag
false
0
Rectangle -7500403 true true 60 15 75 300
Polygon -7500403 true true 90 150 270 90 90 30
Line -7500403 true 75 135 90 135
Line -7500403 true 75 45 90 45

flower
false
0
Polygon -10899396 true false 135 120 165 165 180 210 180 240 150 300 165 300 195 240 195 195 165 135
Circle -7500403 true true 85 132 38
Circle -7500403 true true 130 147 38
Circle -7500403 true true 192 85 38
Circle -7500403 true true 85 40 38
Circle -7500403 true true 177 40 38
Circle -7500403 true true 177 132 38
Circle -7500403 true true 70 85 38
Circle -7500403 true true 130 25 38
Circle -7500403 true true 96 51 108
Circle -16777216 true false 113 68 74
Polygon -10899396 true false 189 233 219 188 249 173 279 188 234 218
Polygon -10899396 true false 180 255 150 210 105 210 75 240 135 240

ghost
false
0
Polygon -7500403 true true 30 165 13 164 -2 149 0 135 -2 119 0 105 15 75 30 75 58 104 43 119 43 134 58 134 73 134 88 104 73 44 78 14 103 -1 193 -1 223 29 208 89 208 119 238 134 253 119 240 105 238 89 240 75 255 60 270 60 283 74 300 90 298 104 298 119 300 135 285 135 285 150 268 164 238 179 208 164 208 194 238 209 253 224 268 239 268 269 238 299 178 299 148 284 103 269 58 284 43 299 58 269 103 254 148 254 193 254 163 239 118 209 88 179 73 179 58 164
Line -16777216 false 189 253 215 253
Circle -16777216 true false 102 30 30
Polygon -16777216 true false 165 105 135 105 120 120 105 105 135 75 165 75 195 105 180 120
Circle -16777216 true false 160 30 30

house
false
0
Rectangle -7500403 true true 45 120 255 285
Rectangle -16777216 true false 120 210 180 285
Polygon -7500403 true true 15 120 150 15 285 120
Line -16777216 false 30 120 270 120

leaf
false
0
Polygon -7500403 true true 150 210 135 195 120 210 60 210 30 195 60 180 60 165 15 135 30 120 15 105 40 104 45 90 60 90 90 105 105 120 120 120 105 60 120 60 135 30 150 15 165 30 180 60 195 60 180 120 195 120 210 105 240 90 255 90 263 104 285 105 270 120 285 135 240 165 240 180 270 195 240 210 180 210 165 195
Polygon -7500403 true true 135 195 135 240 120 255 105 255 105 285 135 285 165 240 165 195

line
true
0
Line -7500403 true 150 0 150 300

line half
true
0
Line -7500403 true 150 0 150 150

pentagon
false
0
Polygon -7500403 true true 150 15 15 120 60 285 240 285 285 120

person
false
0
Circle -7500403 true true 110 5 80
Polygon -7500403 true true 105 90 120 195 90 285 105 300 135 300 150 225 165 300 195 300 210 285 180 195 195 90
Rectangle -7500403 true true 127 79 172 94
Polygon -7500403 true true 195 90 240 150 225 180 165 105
Polygon -7500403 true true 105 90 60 150 75 180 135 105

person lefty
false
0
Circle -7500403 true true 170 5 80
Polygon -7500403 true true 165 90 180 195 150 285 165 300 195 300 210 225 225 300 255 300 270 285 240 195 255 90
Rectangle -7500403 true true 187 79 232 94
Polygon -7500403 true true 255 90 300 150 285 180 225 105
Polygon -7500403 true true 165 90 120 150 135 180 195 105

person righty
false
0
Circle -7500403 true true 50 5 80
Polygon -7500403 true true 45 90 60 195 30 285 45 300 75 300 90 225 105 300 135 300 150 285 120 195 135 90
Rectangle -7500403 true true 67 79 112 94
Polygon -7500403 true true 135 90 180 150 165 180 105 105
Polygon -7500403 true true 45 90 0 150 15 180 75 105

plant
false
0
Rectangle -7500403 true true 135 90 165 300
Polygon -7500403 true true 135 255 90 210 45 195 75 255 135 285
Polygon -7500403 true true 165 255 210 210 255 195 225 255 165 285
Polygon -7500403 true true 135 180 90 135 45 120 75 180 135 210
Polygon -7500403 true true 165 180 165 210 225 180 255 120 210 135
Polygon -7500403 true true 135 105 90 60 45 45 75 105 135 135
Polygon -7500403 true true 165 105 165 135 225 105 255 45 210 60
Polygon -7500403 true true 135 90 120 45 150 15 180 45 165 90

square
false
0
Rectangle -7500403 true true 30 30 270 270

square 2
false
0
Rectangle -7500403 true true 30 30 270 270
Rectangle -16777216 true false 60 60 240 240

star
false
0
Polygon -7500403 true true 151 1 185 108 298 108 207 175 242 282 151 216 59 282 94 175 3 108 116 108

target
false
0
Circle -7500403 true true 0 0 300
Circle -16777216 true false 30 30 240
Circle -7500403 true true 60 60 180
Circle -16777216 true false 90 90 120
Circle -7500403 true true 120 120 60

tree
false
0
Circle -7500403 true true 118 3 94
Rectangle -6459832 true false 120 195 180 300
Circle -7500403 true true 65 21 108
Circle -7500403 true true 116 41 127
Circle -7500403 true true 45 90 120
Circle -7500403 true true 104 74 152

triangle
false
0
Polygon -7500403 true true 150 30 15 255 285 255

triangle 2
false
0
Polygon -7500403 true true 150 30 15 255 285 255
Polygon -16777216 true false 151 99 225 223 75 224

truck
false
0
Rectangle -7500403 true true 4 45 195 187
Polygon -7500403 true true 296 193 296 150 259 134 244 104 208 104 207 194
Rectangle -1 true false 195 60 195 105
Polygon -16777216 true false 238 112 252 141 219 141 218 112
Circle -16777216 true false 234 174 42
Rectangle -7500403 true true 181 185 214 194
Circle -16777216 true false 144 174 42
Circle -16777216 true false 24 174 42
Circle -7500403 false true 24 174 42
Circle -7500403 false true 144 174 42
Circle -7500403 false true 234 174 42

turtle
true
0
Polygon -10899396 true false 215 204 240 233 246 254 228 266 215 252 193 210
Polygon -10899396 true false 195 90 225 75 245 75 260 89 269 108 261 124 240 105 225 105 210 105
Polygon -10899396 true false 105 90 75 75 55 75 40 89 31 108 39 124 60 105 75 105 90 105
Polygon -10899396 true false 132 85 134 64 107 51 108 17 150 2 192 18 192 52 169 65 172 87
Polygon -10899396 true false 85 204 60 233 54 254 72 266 85 252 107 210
Polygon -7500403 true true 119 75 179 75 209 101 224 135 220 225 175 261 128 261 81 224 74 135 88 99

wheel
false
0
Circle -7500403 true true 3 3 294
Circle -16777216 true false 30 30 240
Line -7500403 true 150 285 150 15
Line -7500403 true 15 150 285 150
Circle -7500403 true true 120 120 60
Line -7500403 true 216 40 79 269
Line -7500403 true 40 84 269 221
Line -7500403 true 40 216 269 79
Line -7500403 true 84 40 221 269

x
false
0
Polygon -7500403 true true 270 75 225 30 30 225 75 270
Polygon -7500403 true true 30 75 75 30 270 225 225 270

@#$#@#$#@
NetLogo 5.2.1
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
@#$#@#$#@
default
0.0
-0.2 0 0.0 1.0
0.0 1 1.0 0.0
0.2 0 0.0 1.0
link direction
true
0
Line -7500403 true 150 150 90 180
Line -7500403 true 150 150 210 180

@#$#@#$#@
0
@#$#@#$#@
