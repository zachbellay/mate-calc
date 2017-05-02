#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct BRIGGSTYPE_TAG{
	
	char type[8];
	char cptypes[10][10];
	int percentmen;
	int percentwomen;
	struct BRIGGSTYPE_TAG *next;
	
} BRIGGSTYPE;

typedef struct{
	int **ageandpop;
	char *name;
	double pctcollege, pcthighschool;
} POPULATION;

typedef struct{
	char name[100], maleorfemale, personalitytype[10], hsorcollegeorboth, temp[100];
	int iqorsat, age, yearsolder, yearsyounger;	
} USER;


int main(void);
BRIGGSTYPE *createList();
void printFile(char *filename);
USER initialQuestions();
void printList(BRIGGSTYPE *header);
void printUserStats(USER user);
void printCpTypes(BRIGGSTYPE *p);
void printInfo(BRIGGSTYPE *header, USER user);
int totalPercentCpWithPopulation(BRIGGSTYPE *header, BRIGGSTYPE *type, char maleorfemale);
int percentCpWithPopulation(BRIGGSTYPE *header, char *type, char maleorfemale);
void searchAndPrint(BRIGGSTYPE *header, char *type);
BRIGGSTYPE *searchAndReturn(BRIGGSTYPE *header, char *type);
POPULATION createPopulation(char *filename);
int getNumOfLinesInFile(char *filename);
int getNumCp(USER input, BRIGGSTYPE *header, POPULATION population);
int getPopBetweenAge(POPULATION population, int minAge, int maxAge);
double getPctEducation(POPULATION population, USER user);
bool inRange(POPULATION population, int minAge, int maxAge);
bool ageIsInRange(POPULATION population, int age);

int main(void){

	BRIGGSTYPE *header = createList();
	POPULATION pop = createPopulation("SantaClaraCounty.txt");
	USER user = initialQuestions();
	int totalCp = getNumCp(user, header, pop);
	printf("%s, you have %d very compatible matches in %s.\n", user.name, totalCp, pop.name);
	puts("Press enter to continue...");
	getchar();
	getchar();
	return 0;
}

int getNumCp(USER input, BRIGGSTYPE *header, POPULATION population){
	int minAge, maxAge;
	double totalpercentcp = 0;
	int totalPop = 0;
	BRIGGSTYPE *userType = searchAndReturn(header, input.personalitytype);

	minAge = maxAge = 0;
	
	minAge = input.age - input.yearsyounger;
	maxAge = input.age + input.yearsolder;

	totalpercentcp = (totalPercentCpWithPopulation(header, userType, input.maleorfemale)/100.0);
	printf("Percent Compatible by Personality: %2.1lf%\n", (totalpercentcp*100));
	
	printf("Percent Compatible by Education: %2.1lf%\n", (getPctEducation(population, input)*100));
	totalpercentcp *= getPctEducation(population, input);
	
	totalPop = getPopBetweenAge(population, minAge, maxAge);
	printf("Total %c Population between ages: %d-%d: %d\n", input.maleorfemale, minAge, maxAge, totalPop);

	totalPop *= totalpercentcp;
	
	return totalPop;
}

double getPctEducation(POPULATION population, USER user){
	char test = user.hsorcollegeorboth;
	switch(test){
		case 'B':
		return (population.pcthighschool/100.0);

		case 'C':
		return (population.pctcollege/100.0);

		case 'H':
		return ((population.pcthighschool - population.pctcollege)/100.0);
	
		default:
		return 1.0;
	}
}
int getPopBetweenAge(POPULATION population, int minAge, int maxAge){
	
	int i, currAge, sum, numAges;
	i = sum = numAges = 0;
	currAge = minAge;

	if(!inRange(population, minAge, maxAge)){
		printf("Data is not sufficient for the range %d-%d.\nPlease enter a smaller range.\n", minAge, maxAge);
		exit(255);
	}

	while(currAge != (maxAge + 1)){
		numAges = ((population.ageandpop[i][1]-population.ageandpop[i][0])+1);
		if(currAge >= population.ageandpop[i][0] && currAge <= population.ageandpop[i][1]){
			sum += (population.ageandpop[i][2]/(5 * 2)); 
			currAge++;
			//division by 5 assumes equal distribution amongst
			//all ages in census
			//division by 2 assumes equal 50/50 ratio of M/F
		}else{
			i++;
			
		}
	}
	return sum;
}

bool inRange(POPULATION population, int minAge, int maxAge){
	return (ageIsInRange(population, minAge) && ageIsInRange(population, maxAge));
}

bool ageIsInRange(POPULATION population, int age){
	int i, j;
	int rows = 0;
	rows = sizeof(population.ageandpop) * 3;
	for(i = 0; i < rows; i++){
		for(j = population.ageandpop[i][0]; j < (population.ageandpop[i][1]+1); j++){
			if(age == j)
				return true;
		}
	}
	return false;
}

int getNumOfLinesInFile(char *filename){
	char *ch, s[100];
	int i = 0;	
	FILE *fp;
	if((fp = fopen(filename, "r")) == NULL){
		puts("Could not open file.");
		exit(255);
	}
	while((ch = fgets(s, 100, fp)) != NULL){
		i++;
	}
	
	fclose(fp);
	return i;
}
	
POPULATION createPopulation(char *filename){
	FILE *fp;
	POPULATION output; 
	char *s = (char *)calloc(1000, sizeof(char));
	int i, j, rows, cols;
		
	rows = getNumOfLinesInFile(filename);
	rows-=2; //removes name line & % College/% High School
	cols = 3;
	
	if((fp = fopen(filename, "r")) == NULL){
		printf("Could not open %s.\n", filename);
		exit(255);
	}

	//get name from file
	fscanf(fp, "%[^\n]%*c ", s);
	output.name = (char *)malloc(sizeof(char) * strlen(s));
	strcpy(output.name, s);
	
	//get college & high school percentages
	fscanf(fp, "%[^\n, ]%*c", s);
	output.pcthighschool = strtod(s, NULL);	

	fscanf(fp, "%[^\n, ]%*c", s);
	output.pctcollege = strtod(s, NULL);

	//allocate memory for numbers
	output.ageandpop = (int **)malloc(rows * sizeof(int *));
	for(i = 0; i < rows; i++)
		output.ageandpop[i] = (int *)malloc(cols * sizeof(int));	
	
	//get numbers from file
	for(i = 0; i < rows; i++){
		for(j = 0; j < cols; j++){
			fscanf(fp, "%[^\n, ]%*c", s);
			output.ageandpop[i][j] = atoi(s);
		}
	}
	return output;
}

void printInfo(BRIGGSTYPE *header, USER user){
	int i, j;
	char ch;
	BRIGGSTYPE *pos = header;
	printf("Hello %s! You are a %s.\n", user.name, user.personalitytype);
	while(strcmp(pos->type, user.personalitytype) !=0)
		pos = pos->next;	
	
	puts("The following personality types are highly compatible with yours:");
	printCpTypes(pos);
}

void searchAndPrint(BRIGGSTYPE *header, char *type){
	BRIGGSTYPE *pos = header;
	while(strcmp(pos->type, type) != 0){
		if(pos->next == NULL){
			pos = NULL;
			break;
		}
		pos = pos->next;
	}
	
	if(pos != NULL){
		printf("Current Type: %s\n", pos->type);
		printCpTypes(pos);
		printf("Percent men: %d\n", pos->percentmen);
		printf("Percent women: %d\n", pos->percentwomen);
		putchar('\n');
	}else{
		printf("No results for %s found.\n", type);
	}
}

BRIGGSTYPE *searchAndReturn(BRIGGSTYPE *header, char *type){
	BRIGGSTYPE *pos = header;
	while(strcmp(pos->type, type) != 0){
		if(pos->next == NULL){
			pos = NULL;
			break;
		}
		pos = pos->next;
	}
	return pos;
}

//returns the total total percentage of compatible types 
//i.e. INTJ/M is cp with ESTJ, INTJ, ISTP, ENTJ
//maleorfemale input should be gender you're interested in, not your own
// Total %F = 6 + 1 + 2 + 1 = 10%
//returns 10
int totalPercentCpWithPopulation(BRIGGSTYPE *header, BRIGGSTYPE *type, char maleorfemale){
	int i, numRows, total;
	numRows = (sizeof(type->cptypes)/sizeof(type->cptypes[0]));
	total = 0;
	
	for(i = 0; i < numRows; i++){
		if(type->cptypes[i][0] == '\0')
			break;
		total += percentCpWithPopulation(header, type->cptypes[i], maleorfemale);	
	}
	
	return total;	
}

int percentCpWithPopulation(BRIGGSTYPE *header, char *type, char maleorfemale){
	BRIGGSTYPE *pos = header;
	while(strcmp(pos->type, type) != 0){
		if(pos->next == NULL){
			pos = NULL;
			break;
		}
		pos = pos->next;
	}
	
	if(pos == NULL)
		return -1;
	if(maleorfemale == 'M')
		return pos->percentmen;
	if(maleorfemale == 'F')
		return pos->percentwomen;
	return -1;	
}

void printUserStats(USER user){
	printf("Name: %s\n", user.name);
	printf("Interested in: %c\n", user.maleorfemale);
	printf("Personality Type: %s\n", user.personalitytype);
	printf("Interested in High School, College, or Graduates of Both: %c\n", user.hsorcollegeorboth);
	
	printf("IQ/SAT Score: %d\n", user.iqorsat);
	printf("Age: %d\n", user.age);
	printf("Years Older: %d\n", user.yearsolder);
	printf("Years Younger: %d\n", user.yearsyounger);
}

void printFile(char *filename){
	int c;
	FILE *file;
	file = fopen(filename, "r");
	if(file){
		while((c = getc(file)) != EOF)
			putchar(c);
	putchar('\n');
	fclose(file);
	}
}

void printCpTypes(BRIGGSTYPE *p){
	int numRows = (sizeof(p->cptypes)/sizeof(p->cptypes[0]));
	int i;
	char ch;
	puts("Compatible Types:");
	for(i = 0; i < numRows; i++){
		ch = p->cptypes[i][0];
		if( ch != '\0' && ch != ',' )
			printf("%s\n", p->cptypes[i]);
	}
}

void printList(BRIGGSTYPE *header){
	int i, j;
	BRIGGSTYPE *p = header;
		
	while(p != NULL){
		printf("Current Type: %s\n", p->type);
		printCpTypes(p);
		printf("Percent men: %d\n", p->percentmen);
		printf("Percennt women: %d\n", p->percentwomen);
		putchar('\n');
		p = p->next;	
	}
}

BRIGGSTYPE *createList(){
	BRIGGSTYPE *header = NULL;
	FILE *fp;
	char s[100], t[10], ch;
	int i, j;
	
	if((fp = fopen("BriggsMeyersInfo.txt", "r")) == NULL){
		puts("Could not open file");
		exit(255);
	}
	
	while(1){
		//read data from text file and insert it into node
		
		BRIGGSTYPE *node = ((BRIGGSTYPE *)calloc(1, sizeof(BRIGGSTYPE)));

		//read first line and insert it into char *type
		if(fscanf(fp, "%s", &node->type) == EOF){break;}	
		
		//read second line and insert each element delimited by a comma into a 2D array
		if(fscanf(fp, "%s", s) == EOF){break;}
		
		
		for(i = 0; i < (strlen(s)/4); i++){
			for(j = 0; j < 5; j++){
				ch = s[(5*i)+j];
				if( ch != ',' && ch != '\n')
					node->cptypes[i][j] = ch;				
			}	
		}
		
		//read the third line and insert the integer into percentmen
		if(fscanf(fp, "%d", &node->percentmen) == EOF) break;
		
		//read the fourth line and insert the integer into percentwomen
		if(fscanf(fp, "%d", &node->percentwomen) == EOF) break;
		
		//take node and insert it into linked list
		if(header == NULL){
			node->next = NULL;
			header = node;
		}else{
			node->next = header;
			header = node;
		}
		
	}
	fclose(fp);
	return header;
}



USER initialQuestions(){
	USER output;
	//Name
	puts("What is your name?");
	scanf("%[^\n]%*c", &output.name);

	//Males or Females
	puts("Are you interested in Males or Females? (M/F) :");
	scanf(" %c", &output.maleorfemale);

	//Personality Type
	puts("What is your Briggs Meyers personality type? (XXXX):");
	scanf("%s", &output.personalitytype);
	
	//IQ/SAT
	//puts("What is your IQ/SAT score? (IQ:0-140) (SAT: 400-1600) :");
	//scanf("%s", &output.temp);
	//output.iqorsat = atoi(output.temp);
	
	//College vs. High School
	puts("Are you interested in high school graduates, college grads, or both? (H, C, B) :");
	scanf(" %c", &output.hsorcollegeorboth);

	//Age
	puts("How old are you?");
	scanf("%s", &output.temp);
	output.age = atoi(output.temp);
	
	//Age tolerance +X 
	puts("How much older can your partner be than you? (In years) :");
	scanf("%s", &output.temp);
	output.yearsolder = atoi(output.temp);

	//Age tolerance -Y
	puts("How much younger can your partner be than you? (In years) :");
	scanf("%s", &output.temp);
	output.yearsyounger = atoi(output.temp);
	putchar('\n');
	
	return output;
}