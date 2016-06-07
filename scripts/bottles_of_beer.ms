(foreach (i (range 99 0))
  (cond
    ((= i 0) (printf "No more bottles of beer on the wall, no more bottles of beer.~%Go to the store and buy some more, 99 bottles of beer on the wall.~%"))
    ((= i 1) (printf "1 bottle of beer on the wall, 1 bottle of beer.~%Take one down and pass it around, no more bottles of beer on the wall.~%"))
    (true    (printf "~a bottles of beer on the wall, ~a bottles of beer.~%Take one down and pass it around, ~a bottles of beer on the wall.~%" i i (- i 1)))))