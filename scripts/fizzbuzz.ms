(let ((res nil))
  (foreach (i (range 1 101))
    (set res (cond
               ((= 0 (rem i 15)) "FizzBuzz")
               ((= 0 (rem i 3))  "Fizz")
               ((= 0 (rem i 5))  "Buzz")
               (true              i)))
    (printfln "~a" res)))