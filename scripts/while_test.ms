(let ((x 0) (end (+ 1 (s->n (input "Enter a number: ")))))
  (while (< x end)
    (printfln "~a" x)
    (set x (+ x 1))))