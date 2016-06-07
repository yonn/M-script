(fn fib (n)
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))

(foreach (i (range 25))
  (printf "~a| ~a~%" i (fib i)))
