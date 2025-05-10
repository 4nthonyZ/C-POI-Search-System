/* A simple inverted index implementation:
 *
 * Skeleton code written by Jianzhong Qi, May 2024
 * Edited by: [ShuYuan Zhang 1330944]
 *
 */
//algorithms are fun
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define STAGE_NUM_ONE 1							/* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n"	/* stage header format string */

#define DATA_SET_SIZE 50						/* number of input POIs */
#define DATA_DIM 2								/* POI point dimensionality */
#define QUERY_BOUNDS 4							/* query range bounds */

typedef struct {
	int id;
	double x;
	double y;
	char categories[5][20];
	int cat_count;
} poi_t;

typedef struct {
	double x1;
	double y1;
	double x2;
	double y2;
	char categorie[20];
} query_t;

typedef struct {
	char categorie[21];
	int poi[DATA_SET_SIZE];
	int match_count;
} index_t;

typedef query_t data_t;								

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
list_t *insert_at_foot(list_t *list, data_t value);
void process_queries();

void print_stage_header(int stage_num);
int is_poi_in_query_range(poi_t poi, query_t query);
int is_poi_matching_category(poi_t poi, char *category);
int is_category_in_uni_cat(char *category);

void stage_one();
void stage_two();
void stage_three();
void stage_four();
int poi_count = 0;
poi_t pois[DATA_SET_SIZE];
list_t *s2_list;
char uni_cat[DATA_SET_SIZE][21];
int num_uni = 0;
index_t index[50];
/* add your own function prototypes here */

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	/* stage 1: read POI records */
	stage_one();
	
	s2_list = make_empty_list();
	stage_two();
	
	/* stage 3: compute unique POI categories */
	stage_three();
	
	/* stage 4: construct an inverted index */
	stage_four();
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* stage 1: read POI records */
void stage_one() {
	int max_categories = 0;
	int max_poi = -1;

	print_stage_header(STAGE_NUM_ONE);

	while (scanf("%d %lf %lf", &pois[poi_count].id, 
			&pois[poi_count].x, &pois[poi_count].y) == 3) {
		getchar();  // skip the space after the y-coordinate
		int category_count = 0;
		char tmp1[20];
		int ch_num = 0;
		char ch;

		while ((ch = getchar()) != '#') {
			if (ch == ' ') {
				if (ch_num > 0) {
					tmp1[ch_num] = '\0';
					strcpy(pois[poi_count].categories[category_count], tmp1);
					category_count++;
					ch_num = 0;
				}
			} else {
				if (ch_num < 19) {
					tmp1[ch_num] = ch;
					ch_num = ch_num+1;
				}
			}
		}

		if (ch_num > 0) {  
			tmp1[ch_num] = '\0';
			strcpy(pois[poi_count].categories[category_count], tmp1);
			category_count++;
		}

		pois[poi_count].cat_count = category_count;
		if (category_count > max_categories ||  // Check for max categories
			(category_count == max_categories && pois[poi_count].id < pois[max_poi].id)) {
			max_categories = category_count;
			max_poi = poi_count;
		}
		poi_count++;
	}

	printf("Number of POIs: %d\n", poi_count);

	if (max_poi != -1) {
		printf("POI #%d has the largest number of categories:\n", pois[max_poi].id);
		for (int i = 0; i < pois[max_poi].cat_count; i++) {
			printf("%s", pois[max_poi].categories[i]);
			if (i < pois[max_poi].cat_count - 1) {
				printf(" ");
			}
		}
		printf("\n");
	}
	printf("\n");
}


/* stage 2: read and process queries */
void stage_two() {
	print_stage_header(STAGE_NUM_TWO);
	char buffer[256];
	fgets(buffer, sizeof(buffer), stdin);
	data_t query_data;
	while (scanf("%lf %lf %lf %lf %s", &query_data.x1, &query_data.y1, &query_data.x2, 
			&query_data.y2, query_data.categorie) == 5) {
		s2_list = insert_at_foot(s2_list, query_data);
	}

	process_queries();
	printf("\n");
}

/* stage 3: compute unique POI categories */
void stage_three() {
	print_stage_header(STAGE_NUM_THREE);
	int i, j;

	for (i = 0; i < poi_count; i++) {
		for (j = 0; j < pois[i].cat_count; j++) {
			if (!is_category_in_uni_cat(pois[i].categories[j])) {
				strcpy(uni_cat[num_uni], pois[i].categories[j]);
				num_uni++;
			}
		}
	}

	// Output all unique POI categories
	printf("%d unique POI categories:\n", num_uni);
	for (i = 0; i < num_uni; i++) {
		if (i == num_uni - 1 || ((i + 1) % 5) == 0) {
			printf("%s\n", uni_cat[i]);
		} else {
			printf("%s, ", uni_cat[i]);
		}
	}
	printf("\n");
}




/* stage 4: construct an inverted index */
void stage_four() {
	print_stage_header(STAGE_NUM_FOUR);

	for (int i = 0; i < num_uni; i++) {
		strcpy(index[i].categorie, uni_cat[i]);
	}

	// Sort the index using insertion sort
	for (int i = 1; i < num_uni; i++) {
		index_t temp = index[i];
		int j = i;
		while (j > 0 && strcmp(index[j - 1].categorie, temp.categorie) > 0) {
			index[j] = index[j - 1];
			j--;
		}
		index[j] = temp;
	}

	for (int i = 0; i < num_uni; i++) {
		int matched_pois = 0;
		for (int j = 0; j < poi_count; j++) {
			if (is_poi_matching_category(pois[j], index[i].categorie)) {
				index[i].poi[matched_pois] = pois[j].id;
				matched_pois++;
			}
		}
		index[i].match_count = matched_pois;
	}

	// print output
	for (int i = 0; i < num_uni; i++) {
		printf("%s:", index[i].categorie);
		for (int j = 0; j < index[i].match_count; j++) {
			printf(" %d", index[i].poi[j]);
		}
		printf("\n");
	}
}



/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/* insert a new data element into the end of a linked list
*/
list_t
*insert_at_foot(list_t *list, data_t value) {
	node_t *new;

	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->data = value;
	new->next = NULL;

	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}

	return list;
}

/* process queries in a list (or in an array if using array-based implementation) */
void process_queries() {
	node_t *current_node = s2_list->head;
	int query_counter = 0;

	while (current_node != NULL) {
		printf("POIs in Q%d:", query_counter);
		int found = 0;

		for (int i = 0; i < poi_count; i++) {
			if (is_poi_in_query_range(pois[i], current_node->data)) {
				if (is_poi_matching_category(pois[i], current_node->data.categorie)) {
					printf(" %d", pois[i].id);
					found = 1;
				}
			}
		}

		if (!found) {
			printf(" none");
		}

		printf("\n");
		query_counter++;
		current_node = current_node->next;
	}
}
//Determine whether poi is in range
int is_poi_in_query_range(poi_t poi, query_t query) {
	return query.x1 <= poi.x && poi.x <= query.x2 &&
			query.y1 <= poi.y && poi.y <= query.y2;
}
//Determine whether poi is in cat
int is_poi_matching_category(poi_t poi, char *category) {
	for (int j = 0; j < poi.cat_count; j++) {
		if (strcmp(poi.categories[j], category) == 0) {
			return 1;
		}
	}
	return 0;
}

//Determine whether poi is in unicat
int is_category_in_uni_cat(char *category) {
	for (int k = 0; k < num_uni; k++) {
		if (strcmp(category, uni_cat[k]) == 0) {
			return 1;
		}
	}
	return 0;
}

/****************************************************************/
/*
	Using N to denote the number of POIs, 
	C to denote the maximum number of categories per POI, 
	L to denote the maximum length of a category, and
	U to denote the number of all unique POI categories:

	(1) What is the worst-case time complexity to process a single query 
	using the linear search algorithm in Stage 2, and why?
	1.Outer while loop: This loop traverses all queries, but here we are only 
	  discussing the time complexity of a single query, so this loop can be ignored.
	2.The inner for loop: traverses all POIs, the time complexity is O(N).
	3.Conditional judgment if statement: Check whether the current POI 
	   is within the query range, the time complexity is O(1).
	4.The inner for loop: traverses all categories of the current POI, the time complexity is O(C).
	5.String comparison strcmp: compares categorical strings, the time complexity is O(L).
	Therefore, the worst-case time complexity of a single query is:
	O(N)×O(C)×O(L)=O(N×C×L)
	(2) What is the worst-case time complexity to process a single query 
	using the query algorithm described in Stage 4, and why? 
	1.The inner for loop: traverses all POIs, the time complexity is O(N).
	2.The inner for loop: traverses all categories of the current POI, the time complexity is O(C).
	3.String comparison strcmp: compares categorical strings, the time complexity is O(L).
	Therefore, the worst-case time complexity of a single query is:
	O(N)×O(C)×O(L)=O(N×C×L)



*/
