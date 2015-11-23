
int indexN(int index, int lignes, int colonnes) {
	int n = index - colonnes;
	if(n < 0) {
		return 0;
	}
	return n;
}

int indexS(int index, int lignes, int colonnes) {
	int n = index + colonnes;
	if(n > lignes * colonnes) {
		return lignes * colonnes - 1;
	}
	return n;
}

int indexE(int index, int lignes, int colonnes) {
	int n = index + 1;
	if(n > lignes * colonnes) {
		return lignes * colonnes - 1;
	}
	return n;
}

int indexW(int index, int lignes, int colonnes) {
	int n = index - 1;
	if(n < 0) {
		return 0;
	}
	return n;
}


int coul_indexN(int index, int lignes, int colonnes) {
	int n = index - colonnes * 3;
	if(n < 0) {
		return 0;
	}
	return n;
}

int coul_indexS(int index, int lignes, int colonnes) {
	int n = index + colonnes * 3;
	if(n > lignes * colonnes * 3) {
		return lignes * colonnes * 3 - 1;
	}
	return n;
}

int coul_indexE(int index, int lignes, int colonnes) {
	int n = index + 3;
	if(n > lignes * colonnes * 3) {
		return lignes * colonnes * 3 - 1;
	}
	return n;
}

int coul_indexW(int index, int lignes, int colonnes){
	int n = index - 3;
	if(n < 0) {
		return 0;
	}
	return n;
}

int coul_indexNW(int index, int lignes, int colonnes) {
	return coul_indexW(coul_indexN(index, lignes, colonnes), lignes, colonnes);
}

int coul_indexNE(int index, int lignes, int colonnes) {
	return coul_indexE(coul_indexN(index, lignes, colonnes), lignes, colonnes);
}

int coul_indexSE(int index, int lignes, int colonnes) {
	return coul_indexE(coul_indexS(index, lignes, colonnes), lignes, colonnes);
}

int coul_indexSW(int index, int lignes, int colonnes) {
	return coul_indexW(coul_indexS(index, lignes, colonnes), lignes, colonnes);
}

int between (int value, int cmp1, int cmp2) {
	return value > cmp1 && value < cmp2;
}

int min(int a, int b) {
	if(a < b) {
		return a;
	}
	return b;
}

int max(int a, int b) {
	if(a > b) {
		return a;
	}
	return b;
}

void histo_coul(OCTET *in, int lignes, int colonnes) {
	int i;

	int histor[256] = {0};
	int histog[256] = {0};
	int histob[256] = {0};

	for (i=0; i < lignes * colonnes * 3; i += 3) {
		histor[in[i]]++;
		histog[in[i+1]]++;
		histob[in[i+2]]++;	
	}

	for (i=0; i < 256; i++) {
		printf("%d %d %d %d\n", i, histor[i], histog[i], histob[i]);
	}
}


void histo(OCTET *in, int lignes, int colonnes) {
	int i;
	int histo[256] = {0};

	for (i=0; i < lignes * colonnes; i++) {
		histo[in[i]]++;	
	}

	for (i = 0; i < 255; ++i)
	{
		printf("%d %d\n", i, histo[i]);
	}
}

int getAvgOfNeighbors(OCTET *in, int lignes, int colonnes, int index, int nbN) {
	nbN = (nbN % 2 == 1)?nbN+1:nbN;

	int topLeft = index;
	int i, j, iTemp;
	int sum = 0;

	for (i = 0; i < nbN / 2; ++i) {
		topLeft = coul_indexW(topLeft, lignes, colonnes);	
	}

	for (i = 0; i < nbN / 2; ++i) {
		topLeft = coul_indexN(topLeft, lignes, colonnes);	
	}

	for (i = 0; i < nbN; ++i) {
		iTemp = topLeft;
		for (j = 0; j < nbN; ++j) {
			sum += in[iTemp];
			iTemp = coul_indexE(iTemp, lignes, colonnes);
		}
		topLeft = coul_indexS(topLeft, lignes, colonnes);
	}
	return sum / (nbN * nbN);
}