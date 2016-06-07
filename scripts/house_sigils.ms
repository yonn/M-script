(defvar house/sigils [["Stark"     "Direwolf"]
                      ["Lannister" "Lion"    ]
		      ["Baratheon" "Stag"    ]
		      ["Greyjoy"   "Kraken"  ]
		      ["Targaryen" "Dragon"  ]])

(foreach (ho/si house/sigils)
  (printf "~a: ~a~%~%" (elt 0 ho/si) (elt 1 ho/si)))