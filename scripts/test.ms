(defvar res1 (let ((x 0))
               (set x 1)
	       x))


(defvar res2 (let ((x 0) (y 1))
               (+ x y)))

(printfln "res1 = res2? ~a; = ~a" (= res1 res2) res1)
