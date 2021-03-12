//APRO I FILE IN LETTURA
	if ((ipftxt = fopen(argv[1],"r"))==NULL) {
		perror("opening mat_dump.txt");
		return -1;
	}
	if ((ipfdat = fopen(argv[2],"r"))==NULL) {
		perror