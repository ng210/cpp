	FILE* fp = fopen("mathe.csv", "w");

	for (int i = 0; i < 20; i++) {
		int a = (int)Utils::random(50);
		int b = (int)Utils::random(50);
		switch (Utils::random(3)) {
			case 0:
				fprintf(fp, "%d;+;?;=;%d\n", a, a + b);
				break;
			case 1:
				fprintf(fp, "?;+;%d;=;%d\n", b, a + b);
				break;
			case 2:
				fprintf(fp, "%d;+;%d;=;?\n", a, b);
				break;
		}
	}

	for (int i = 0; i < 20; i++) {
		int a = (int)Utils::random(50);
		int b = (int)Utils::random(50);
		if (a < b) {
			int tmp = a;
			a = b;
			b = tmp;
		}
		switch (Utils::random(3)) {
		case 0:
			fprintf(fp, "%d;-;?;=;%d\n", a, a - b);
			break;
		case 1:
			fprintf(fp, "?;-;%d;=;%d\n", b, a - b);
			break;
		case 2:
			fprintf(fp, "%d;-;%d;=;?\n", a, b);
			break;
		}
	}

	for (int i = 0; i < 20; i++) {
		int a = (int)Utils::random(10);
		int b = (int)Utils::random(10);
		switch (Utils::random(3)) {
		case 0:
			fprintf(fp, "%d;*;?;=;%d\n", a, a * b);
			break;
		case 1:
			fprintf(fp, "?;*;%d;=;%d\n", b, a * b);
			break;
		case 2:
			fprintf(fp, "%d;*;%d;=;?\n", a, b);
			break;
		}
	}

	for (int i = 0; i < 20; i++) {
		int a = (int)Utils::random(10);
		int b = (int)Utils::random(10);
		int r = (int)Utils::random(10) % a;
		switch (Utils::random(3)) {
		case 0:
			fprintf(fp, "%d;:;?;=;%d;R%d\n", a*b+r, b, r);
			break;
		case 1:
			fprintf(fp, "?;:;%d;=;%d;R%d\n", a, b, r);
			break;
		case 2:
			fprintf(fp, "%d;:;%d;=;?;R%d\n", a*b + r, a, r);
			break;
		}
	}


	fclose(fp);