usage : 
encoding : 
	-i <input file>
	-o <output file> [default ./out.off]
	-m <binary message>
	-a <alpha> [default 0.1]
	-d <delta> [default 0.9]
decoding : 
	-r <file to decode>
	-l <message length>


L'image "20.png" est tatouée avec le message "00011110101010001101" de longueur 20
L'image "128.png" est tatouée avec le message "01000101011110001100010110011111101100010100011011110101011111111010000010100111111101000111101011111010001101101000101001011000" de longueur 128

Elles sont tatouées avec les mêmes paramêtres par défaut, à savoir alpha = 0.1 et delta = 0.9

On peut voir que plus le message est long, plus la distorstion est faible

L'image "base.png" sert de référence

Pour compiler :
      g++ -std=c++11 *.cpp	
