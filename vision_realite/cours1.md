#Introduction générale

##Rappels de géométrie

Repère direct ou indirect

norme euclidienne : ||→OP|| = sqrt(x^2 + y^2 + z^3)
Dans le repère R1 →O1P = X1→i1 + Y1→j1 + Z1→k1
Dans le repère R2 →O2P = X2→i2 + Y2→j2 + Z2→k2

Pour changer de repère : Il faut un vecteur connu dans les deux repères et connaitre l'orientation de chaque repère

|X2|   |   | |X1|      	    	 R est une matrice 3×3
|Y2| = | R | |Y1| + T		 R est la matrice de rotation
|z2|   |   | |Z1|
 ↑            ↑
 P2           P1  

L'inverse d'une matrice est notée (R)^-1
(AA^-1) est la matrice identité
Il que la matrice stoi carré et que le déterminant soit différent de 0
AB = C
B = A^-1 × C
L'inverse d'une matrice est la transposée (echanger les lignes et les colonnes)
Les matrices de rotation sont toujours inversibles

P2 = R × P1 + T
P2 - T = R × P1
R^-1 × (P2 - T) = P1
R^T × (P2 - T) = P1
R^T × P2 - R^T = P1

|X2|   ||   | | || |X1|      	    	 
|Y2| = || R | |T|| |Y1|
|z2|   ||   | | || |Z1|
|1 |   |0 0 0  1 | |1 |

|X2|   ||     | |      || |X1|      	    	 
|Y2| = || R^T | |-R^T×T|| |Y1|
|z2|   ||     | |      || |Z1|
|1 |   |0 0 0      1    | |1 |

Aller voir OpenCV

## Formation des images

https://en.wikipedia.org/wiki/Lens_%28optics%29

Modèle sténopée / pin-hole
https://en.wikipedia.org/wiki/Pinhole_camera

Z est l'axe optique

## Modèle des caméras