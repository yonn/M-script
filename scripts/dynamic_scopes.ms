(defvar x 0)

(printfln "0: ~a" x)
(let ((x 1))
 (printfln "1: ~a" x)
 (let ((x 2))
   (printfln "2: ~a" x))
 (printfln "1: ~a" x))
(printfln "0: ~a" x)