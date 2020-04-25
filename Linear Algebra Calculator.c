/*This is a linear algebra calculator. It can perform addition, multiplication, outerproduct among scalars,
vectors, matrices. Variables will be identified by a name - capital case letter 'A'-'Z'. The variables types
are marked with lower case letters: 'm'(matrix), 'v'(vector) and 's'(scalar). The matrices and vectors
can come with different dimensions. It is able to catch all kinds of errors that might occur during 
different computations. This project belongs to Jae Seong Kim ID: 928506963 TAU student. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

typedef struct {
	char* input;
	char* cur;
}buffer;

typedef struct {
	char* command;
	char var_name;
	char type;
	int dim;
	double* value;
}object;

typedef struct {
	char var_name;
	char type;
	int dim;
	double* value;
	int used;
}variable;

//Returns true if buffer pointer reached the end of line else return false
int end_of_line_parser(buffer* buf) {
	if ((buf->cur)[0] == '\0') {
		return true;
	}
	else {
		printf("Line not ending properly on %d\n", strlen(buf->input) - strlen(buf->cur));
		return false;
	}
}

//bump the pointer up by the length of the command
int try_grab_and_compare(char* target, char** buf_str) {
	int str_len = strlen(*buf_str);
	int target_len = strlen(target);
	if (str_len >= target_len && !strncmp(*buf_str, target, target_len)) {
		//bump the pointer
		*buf_str = *buf_str + target_len;
		return true;
	}
	else {
		return false;
	}
}

//return true if the command in user's input matches allowed commands else return false
int command_parser(buffer* buf, object* obj) {

	if (try_grab_and_compare("SET", &(buf->cur))) {
		obj->command = "SET";
		return true;
	}
	else if (try_grab_and_compare("PRINT", &(buf->cur))) {
		obj->command = "PRINT";
		return true;
	}

	else if (try_grab_and_compare("ADD", &(buf->cur))) {
		obj->command = "ADD";
		return true;
	}
	else if (try_grab_and_compare("MULTIPLY", &(buf->cur))) {
		obj->command = "MULTIPLY";
		return true;
	}
	else if (try_grab_and_compare("OUTER_PRODUCT", &(buf->cur))) {
		obj->command = "OUTER_PRODUCT";
		return true;
	}
	else if (try_grab_and_compare("HALT", &(buf->cur))) {
		obj->command = "HALT";
		return true;
	}
	else if (try_grab_and_compare("HELP", &(buf->cur))) {
		obj->command = "HELP";
		return true;
	}
	else {
		printf("Invalid command\n");
		return false;
	}
}

//skips spaces 
int space_parser(buffer* buf) {
	int i = 0;
	int space_cnt = 0;
	while ((buf->cur)[i] == ' ') {
		space_cnt++;
		i++;
	}
	if (space_cnt >= 1) {
		buf->cur = buf->cur + space_cnt;
		return true;
	}
	else {
		printf("error occurence at location:%d\n", strlen(buf->input) - strlen(buf->cur));
		printf("there should be at least one space between each argument\n");
		return false;
	}
}

//returns true if the variable name is in between A and Z inclusively else return false
int name_parser(buffer* buf, object* obj) {
	if ('A' <= (buf->cur)[0] && (buf->cur)[0] <= 'Z') {
		obj->var_name = buf->cur[0];
		buf->cur++;
		return true;
	}
	else {
		printf("Variable Name is invalid\n");
		return false;
	}
}

//returns true if user input type matches allowed types m/s/v else return false
int type_parser(buffer* buf, object* obj) {
	if ((buf->cur)[0] == 'm' || (buf->cur)[0] == 's' || (buf->cur)[0] == 'v') {
		obj->type = (buf->cur)[0];
		buf->cur++;
		return true;
	}
	else {
		printf("type is invalid\n");
		return false;
	}
}

//returns true if user input dimension is between 1 to 9 (maximum can be assumed to be 6 by project requirement)
int dim_parser(buffer* buf, object* obj) {

	if ('1' <= (buf->cur)[0] && (buf->cur)[0] <= '9') {

		obj->dim = (buf->cur)[0] - '0'; //converting char to int
		buf->cur++;
		return true;
	}
	else {
		printf("dim is invalid\n");
		return false;
	}
}

//returns true if the current number in the value part of the set command is saved correctly, else return false
int number_parser(buffer* buf, object* obj, double** values, int* size, int* num_parser_called_cnt) {
	int count;
	int char_used;

	double value;
	double* new_values;

	count = sscanf(buf->cur, "%lf%n", &value, &char_used);

	if (count == 1) {
		buf->cur = buf->cur + char_used;
		if (*num_parser_called_cnt < *size) {
			(*values)[*num_parser_called_cnt] = value;
			obj->value = *values;
			*num_parser_called_cnt += 1;
			return true;
		}
		if (*num_parser_called_cnt == *size) {
			*size += 1;
			new_values = (double*)realloc(*values, sizeof(double)*(*size));
			if (new_values == NULL) {
				free(*values);
				printf("realllocation error in number_parser\n");
				return false;
			}

			new_values[*num_parser_called_cnt] = value;
			*values = new_values;
			obj->value = *values;

			*num_parser_called_cnt += 1;
			return true;
		}
		else {
			printf("Invalid value\n");
			return false;
		}

	}
	else {
		printf("Invalid value\n");
		return false;
	}
}

//skips a space if there is one
void non_strict_space_parser(buffer *buf) {
	while ((buf->cur)[0] == ' ') {
		buf->cur++;
	}
}

//return true if one comma is skipped succesfully else return false
int comma_parser(buffer *buf) {
	if (buf->cur[0] != ',') {
		printf("missing a comma or additional spaces in the end \n");
		return false;
	}
	else {
		buf->cur++;
		return true;
	}
}

//return true if the value part of the set command is handled correctly else return false
int value_parser(buffer *buf, object* obj) {
	double* values = (double*)malloc(sizeof(double)); //set 1
	int size = 1;
	int num_parser_called_cnt = 0;

	if (!number_parser(buf, obj, &values, &size, &num_parser_called_cnt)) {
		return false;
	}

	while (*(buf->cur) != '\0') {
		non_strict_space_parser(buf);

		if (!comma_parser(buf)) {
			return false;
		}
		non_strict_space_parser(buf);

		if (!number_parser(buf, obj, &values, &size, &num_parser_called_cnt)) {
			return false;
		}
	}

	//check whether dimension matches number of input values
	//for scalar dimension is ignored, but should only have one value
	if (obj->type == 's' && size != 1) {
		printf("scalar should only have one value\n");
		return false;
	}
	else if (obj->type == 'v'  && size != obj->dim) {
		printf("number of values don't match the dimension\n");
		return false;
	}
	else if (obj->type == 'm' && size != (obj->dim)*(obj->dim)) {
		printf("number of values don't match the dimension\n");
		return false;
	}
	else {
		return true;
	}

	return true;
}

void copy_object_to_variable(object* obj, variable* var) {
	var->var_name = obj->var_name;
	var->type = obj->type;
	var->dim = obj->dim;
	var->value = obj->value;
}

//return index when there exists variable with the same name else return -1
int index_of_same_var_name(variable* collection, variable* var) {
	int index;
	for (index = 0; index < 26; index++) {
		if (var->var_name == collection[index].var_name) {
			return index;
		}
	}
	return -1;
}

//inserts variable to the collection of variables
void insert_var_to_collection(variable* collection, variable* var) {
	int index = index_of_same_var_name(collection, var);
	int i;

	if (index==-1) {
		for (i = 0; i < 26; i++) {
			if (!collection[i].used) {
				collection[i] = *var;
				collection[i].used = true;
				break;
			}
		}
	}
	else {
		free(collection[index].value); // free memory space
		collection[index] = *var;
		collection[index].used = true;
	}

}
//initialize newly made varaible collection by changing variable status as not used
void intialize_vars_to_unused(variable* collection) {
	int i;

	for (i = 0; i < 26; i++) {
		collection[i].used = false;
	}
}

//returns true if SET command is handled without error else return false
int SET_parser(buffer* buf, object* obj, variable* var, variable* collection) {
	if (!space_parser(buf)) {
		return false;
	}

	if (!name_parser(buf, obj)) {
		return false;
	}

	if (!space_parser(buf)) {
		return false;
	}

	if (!type_parser(buf, obj)) {
		return false;
	}

	if (!space_parser(buf)) {
		return false;
	}

	if (!dim_parser(buf, obj)) {
		return false;
	}

	if (!space_parser(buf)) {
		return false;
	}

	if (!value_parser(buf, obj)) {
		return false;
	}
	if (!end_of_line_parser(buf)) {
		return false;
	}

	copy_object_to_variable(obj, var);
	insert_var_to_collection(collection, var);
	return true;
}

//return true if given variable in the parameter is defined else return false
int is_variable_defined(char varName, variable* collection, variable* var) {
	int i;
	for (i = 0; i < 25; i++) {
		if (collection[i].var_name == varName) {
			*var = collection[i];
			return true;
		}
	}
	printf("%c variable is not defined\n", varName);
	return false;
}

//return true if given variable in the parameter has correct variable name and whether
//it's defined or not, else return false
int existing_variable_name_parser(buffer* buf, variable* collection, variable* var) {
	char variable_name = (buf->cur)[0];
	if ('A' <= variable_name && variable_name <= 'Z') {
		if (is_variable_defined(variable_name, collection, var)) {
			buf->cur++;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		printf("%c variable is invalid\n", variable_name);
		return false;
	}
}

//return true if given variable in the parameter has correct variable name else return false
int resultant_name_parser(buffer* buf, variable* resultant) {
	char variable_name = (buf->cur)[0];
	if ('A' <= variable_name && variable_name <= 'Z') {
		resultant->var_name = variable_name;
		buf->cur++;
		return true;
	}
	else {
		printf("%c variable is invalid\n", variable_name);
		return false;
	}
}

//return true if variable one and two have the same dimension and initialize resultant variable's dimension
//else return false
int check_dimension_parser(variable* one, variable* two, variable* resultant) {
	if (one->dim == two->dim) {
		resultant->dim = one->dim;
		return true;
	}
	else {
		printf("Dimension is different for %c and %c \n", one->var_name, two->var_name);
		return false;
	}
}

//return true variable one and two have the same type and initialize resultant variable's type
int check_type_parser(variable* one, variable* two, variable* resultant) {
	if (one->type == two->type) {
		resultant->type = one->type;
		return true;
	}
	else {
		printf("type is different for %c and %c \n", one->var_name, two->var_name);
		return false;
	}
}
//addition between two vectors
void add_computation_for_v(variable*one, variable* two, double* value) {
	int i;
	for (i = 0; i < (one->dim); i++) {
		value[i] = (one->value)[i] + (two->value)[i];
	}
}
//addition between two matrices
void add_computation_for_m(variable* one, variable* two, double* value) {
	int i;
	for (i = 0; i < (one->dim)*(one->dim); i++) {
		value[i] = (one->value)[i] + (two->value)[i];
	}
}

int add_computation(variable* one, variable* two, variable* resultant, variable* collection) {
	if (one->type == 's') {
		double* value = (double*)malloc(sizeof(double)); //one->dim == 1
		*value = *(one->value) + *(two->value);
		resultant->value = value;
		insert_var_to_collection(collection, resultant);
		return true;
	}
	if (one->type == 'v') {
		double* value = (double*)malloc(sizeof(double)*(resultant->dim));
		add_computation_for_v(one, two, value);
		resultant->value = value;
		insert_var_to_collection(collection, resultant);
		return true;
	}
	if (one->type == 'm') {
		double* value = (double*)malloc(sizeof(double)*(resultant->dim)*(resultant->dim));
		add_computation_for_m(one, two, value);
		resultant->value = value;
		insert_var_to_collection(collection, resultant);
		return true;
	}
	printf("Computation error\n");
	return false;
}

int add_resultant_parser(buffer* buf, variable* one, variable* two, variable* resultant, variable* collection) {
	if (!resultant_name_parser(buf, resultant)) {
		return false;
	}
	if (!check_dimension_parser(one, two, resultant)) {
		return false;
	}
	if (!check_type_parser(one, two, resultant)) {
		return false;
	}
	
	if (!add_computation(one, two, resultant, collection)) {
		return false;
	}
	
	return true;
}

//returns true if ADD command is handled correctly else return false
int ADD_parser(buffer* buf, variable* collection, variable* resultant) {
	variable one;
	variable second;

	if (!space_parser(buf)) {
		return false;
	}

	if (!existing_variable_name_parser(buf, collection, &one)) { // variable one initialized here
		return false;
	}

	if (!space_parser(buf)) {
		return false;
	}

	if (!existing_variable_name_parser(buf, collection, &second)) {//variable two initalized here
		return false;
	}

	if (!space_parser(buf)) {
		return false;
	}

	if (!add_resultant_parser(buf, &one, &second, resultant, collection)) {
		return false;
	}
	if (!end_of_line_parser(buf)) {
		return false;
	}

	return true;
}

//return true if variable one and two have the correct types to do multiplication 
//and initialize resultant variable with correct type else return false
int check_type_parser_for_mul(variable* one, variable* two, variable* resultant) {
	if (one->type == 'm' && two->type == 'm') {
		resultant->type = 'm';
		return true;
	}
	if (one->type == 'm' && two->type == 'v') {
		resultant->type = 'v';
		return true;
	}
	if (one->type == 'v' && two->type == 'v') {
		resultant->type = 's';
		return true;
	}
	if (one->type == 's' && ((two->type == 's') || (two->type == 'v') || (two->type == 'm'))) {
		resultant->type = two->type;
		return true;
	}

	printf("Multiplication between type:%c and type:%c is not allowed\n", one->type, two->type);
	return false;
}

//return true if variable one and two have the correct dimension to do multiplication
//and initialize resultant variable with correct dimension else return false
int check_dimension_parser_for_mul(variable* one, variable* two, variable* resultant) {
	if (one->type == 'm' && two->type == 'm') {
		if (one->dim == two->dim) {
			resultant->dim = one->dim;
			return true;
		}
		else {
			printf("dimension of two matrices don't match\n");
			return false;
		}
	}

	if (one->type == 'm' && two->type == 'v') {
		if (one->dim == two->dim) {
			resultant->dim = one->dim;
			return true;
		}
		else {
			printf("dimension of matrix and vector don't match\n");
			return false;
		}
	}

	if (one->type == 'v' && two->type == 'v') {
		if (one->dim == two->dim) {
			resultant->dim = 1;
			return true;
		}
		else {
			printf("dimension of the two vectors don't match\n");
			return false;
		}
	}

	if (one->type == 's' && ((two->type == 's') || (two->type == 'v') || (two->type == 'm'))) {
		resultant->dim = two->dim;
		return true;
	}
	
	printf("dimension invalid\n");
	return false;
}

//return empty 2D arr
double** create_2D_arr(variable* var) {
	int i;
	double** array;
	int dim = var->dim; 
	array = (double**)malloc(dim * sizeof(double*));
	
	if (array == NULL) {
		return NULL;
	}

	for (i = 0; i < dim; i++) {
		array[i] = (double*)malloc(dim * sizeof(double));
		
		if (array[i] == NULL) {
			return NULL;
		}

	}
	return array;
}

void initialize_2D_arr(double** matrix, variable* var) {
	int dim = var->dim;
	double* values = var->value;
	int counter = 0;
	int i;
	int j;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			matrix[i][j] = values[counter];
			counter++;
		}
	}

}

void multiply_matrix_matrix(double** matrixOne, double** matrixTwo, double* result, variable* one) {
	int dim = one->dim;
	int counter = 0;
	double sum = 0;
	int i;
	int j;
	int k;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			for (k = 0; k < dim; k++) {
				sum += matrixOne[i][k] * matrixTwo[k][j];
			}
			result[counter] = sum;
			counter++;
			sum = 0;
		}
	}
}

void multiply_matrix_vector(double* matrixValues, double* vectorValues, double* result, variable* one) {
	int dim = one->dim;
	int counter = 0;
	double sum = 0;
	int i;
	int j;

	for (i = 0; i <= dim*dim - dim; i+=dim) {
		for (j = 0; j < dim; j++) {
			sum += matrixValues[i + j] * vectorValues[j];
		}
		printf("sum is %f", sum);
		result[counter] = sum;
		sum = 0;
		counter++;
	}
}

void multiply_vector_vector(double* vectorOneValues, double* vectorTwoValues, double* result, variable* one){
	int dim = one->dim;
	double sum = 0;
	int i;
	
	for (i = 0; i < dim; i++) {
		sum += vectorOneValues[i] * vectorTwoValues[i];
	}
	result[0] = sum;
}

void multiply_scalar_matrix(double* scalar, double* matrix, double* result, variable* two) {
	int dim = two->dim;
	int i;
	for (i = 0; i < dim*dim; i++) {
		result[i] = scalar[0] * matrix[i];
	}
}

void multiply_scalar_vector(double* scalar, double* vector, double* result, variable* two) {
	int dim = two->dim;
	int i;
	for (i = 0; i < dim; i++) {
		result[i] = scalar[0] * vector[i];
	}
}

void multiply_scalar_scalar(double* scalarOne, double* scalarTwo, double* result, variable* two) {
	result[0] = scalarOne[0] * scalarTwo[0];
}


void free_2D_arr(double*** arr, variable* var) {
	int i;
	int dim = var->dim;
	for (i = 0; i < dim; i++) {
		free(*arr[i]);
	}
	free(*arr);
}


int multiply_computation(variable* one, variable* two, variable* resultant, variable* collection) {
	double** matrixOne;
	double** matrixTwo; 
	

	if (one->type == 'm' && two->type == 'm') {
		double* result = (double*)malloc((one->dim)*(one->dim) * sizeof(double));
		if (result == NULL) {
			printf("dynamic allocation problem in multiply_computation\n");
			return false;
		}

		matrixOne = create_2D_arr(one);
		matrixTwo = create_2D_arr(two);
		
		if (matrixOne == NULL || matrixTwo == NULL) {
			printf("dynamic allocation problem in multiply_computation\n");
			return false;
		}

		initialize_2D_arr(matrixOne, one);
		initialize_2D_arr(matrixTwo, two);

		multiply_matrix_matrix(matrixOne, matrixTwo, result, one);

		resultant->value = result;

		insert_var_to_collection(collection, resultant);

		free_2D_arr(&matrixOne, one);
		free_2D_arr(&matrixTwo, two);
		return true;
	}

	if (one->type == 'm' && two->type == 'v') {
		double* matrixValues = one->value;
		double* vectorValues = two->value; 

		double* result = (double*)malloc((two->dim)* sizeof(double));
		if (result == NULL) {
			printf("dynamic allocation problem in multiply_computation\n");
			return false;
		}

		multiply_matrix_vector(matrixValues, vectorValues, result, one);
		
		resultant->value = result;

		insert_var_to_collection(collection, resultant);
		return true;
	}

	if (one->type == 'v' && two->type == 'v') {
		double* vectorOneValues = one->value;
		double* vectorTwoValues = two->value;

		double* result = (double*)malloc(sizeof(double));
		if (result == NULL) {
			printf("dynamic allocation problem in multiply_computation\n");
			return false;
		}
		multiply_vector_vector(vectorOneValues, vectorTwoValues, result, one);

		resultant->value = result;

		insert_var_to_collection(collection, resultant);
		return true;
	}

	if (one->type == 's') {
		if (two->type == 'm') {
			double* scalar = one->value;
			double* matrix = two->value;

			double* result = (double*)malloc((two->dim)*(two->dim) * sizeof(double));
			if (result == NULL) {
				printf("dynamic allocation problem in multiply_computation\n");
				return false;
			}

			multiply_scalar_matrix(scalar, matrix, result, two);
			resultant->value = result;
			insert_var_to_collection(collection, resultant);
			return true;
		}
		if (two->type == 'v') {
			double* scalar = one->value;
			double* vector = two->value;

			double* result = (double*)malloc((two->dim) * sizeof(double));
			if (result == NULL) {
				printf("dynamic allocation problem in multiply_computation\n");
				return false;
			}

			multiply_scalar_vector(scalar, vector, result, two);
			resultant->value = result;
			insert_var_to_collection(collection, resultant);
			return true;
		}

		if (two->type == 's') {
			double* scalarOne = one->value;
			double* scalarTwo = two->value;

			double* result = (double*)malloc(sizeof(double));
			if (result == NULL) {
				printf("dynamic allocation problem in multiply_computation\n");
				return false;
			}

			multiply_scalar_scalar(scalarOne, scalarTwo, result, two);
			resultant->value = result;
			insert_var_to_collection(collection, resultant);
			return true;
		
		}
	}

	printf("problem with multipling computation\n");
	return false;
}


int multiply_resultant_parser(buffer* buf, variable* one, variable* two, variable* resultant, variable* collection) {
	if (!resultant_name_parser(buf, resultant)) {
		return false;
	}
	if (!check_type_parser_for_mul(one, two, resultant)) {
		return false;
	}
	if (!check_dimension_parser_for_mul(one, two, resultant)) {
		return false;
	}
	if (!multiply_computation(one, two, resultant, collection)) {
		return false;
	}

	return true;
}

//returns true if MULTIPLY command is performed correctly else return false
int MULTIPLY_parser(buffer* buf, variable* collection, variable* resultant) {
	variable one;
	variable two; 
	
	if (!space_parser(buf)) {
		return false;
	}
	if (!existing_variable_name_parser(buf, collection, &one)) {
		return false;
	}
	if (!space_parser(buf)) {
		return false;
	}
	if (!existing_variable_name_parser(buf, collection, &two)) {
		return false;
	} 
	if (!space_parser(buf)) {
		return false;
	}

	if (!multiply_resultant_parser(buf, &one, &two, resultant, collection)) {
		return false;
	}

	if (!end_of_line_parser(buf)) {
		return false;
	}

	return true;
}

//return true if variable one and two have correct types to perform multiplication
//initialize variable resultant's type else return false
int check_type_parser_for_outer_product(variable* one, variable* two, variable* resultant) {
	if (one->type == 'v'&&two->type == 'v') {
		resultant->type = 'm';
		return true;
	}
	else {
		printf("The type of all variables have to be vectors");
		return false;
	}
}

int outer_product_computation(variable* one, variable* two, variable* resultant, variable* collection) {
	double* vectorOneValues = one->value;
	double* vectorTwoValues = two->value;
	int dim = one->dim;
	int counter = 0;
	int i;
	int j;

	double* result = (double*)malloc(dim*dim* sizeof(double));
	if (result == NULL) {
		printf("problem with dynamic allocation for result");
		return false;
	}

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			result[counter] = vectorOneValues[i] * vectorTwoValues[j];
			counter++;
		}
	}

	resultant->value = result;
	insert_var_to_collection(collection, resultant);
	return true;
}


int outer_product_resultant_parser(buffer* buf, variable* one, variable* two, variable* resultant, variable* collection) {
	if (!resultant_name_parser(buf, resultant)) {
		return false;
	}
	if (!check_dimension_parser(one, two, resultant)) {
		return false;
	}
	if (!check_type_parser_for_outer_product(one, two, resultant)) {
		return false;
	}
	if (!outer_product_computation(one, two, resultant, collection)) {
		return false;
	}
	return true;
}

//return true if OUTER_PRODUCT command is performed correctly, else return false
int OUTER_PRODUCT_parser(buffer* buf, variable* collection, variable* resultant) {
	variable one;
	variable two;
	if (!space_parser(buf)) {
		return false;
	}
	if (!existing_variable_name_parser(buf, collection, &one)) {
		return false;
	}
	if (!space_parser(buf)) {
		return false;
	}
	if (!existing_variable_name_parser(buf, collection, &two)) {
		return false;
	}
	if (!space_parser(buf)) {
		return false;
	}
	if (!outer_product_resultant_parser(buf, &one, &two, resultant, collection)) {
		return false;
	}
	if (!end_of_line_parser(buf)) {
		return false;
	}

	return true;
}

//return true if given variable is printed correctly else return false
int print_target(variable* target) {
	int i;
	
	printf("name: %c\n", target->var_name);
	printf("dim : %d\n", target->dim);
	if (target->type == 's') {
		for (i = 0; i < 1; i++) {
			printf("%.2f\t", (target->value)[0]);
		}
		printf("\n\n");
		return true;
	}
	if (target->type == 'v'){
		for (i = 0; i < target->dim; i++) {
			printf("%.2f\t", (target->value)[i]);
		}
		printf("\n\n");
		return true;
	}
	if (target->type == 'm') {
		for (i = 0; i < (target->dim)*(target->dim); i++) {
			if (i%target->dim == 0) {
				printf("\n");
			}
			printf("%.2f\t", (target->value)[i]);
		}
		printf("\n\n");
		return true;
	}

	printf("print invalid\n");
	return false;
}

//return true if command PRINT is performed correctly else return false
int PRINT_parser(buffer* buf, variable* collection, variable* target) {
	if (!space_parser(buf)) {
		return false;
	}
	
	if (!existing_variable_name_parser(buf, collection, target)) {
		return false;
	}

	if (!end_of_line_parser(buf)) {
		return false;
	}
	if (!print_target(target)) {
		return false;
	}

	return true;
}

//exectues HELP command
void print_help(void) {
	printf("SET <variable_name> <variable_type> <dimension> <comma separated values>: \n");
	printf("Ex: SET X v 5 10,20,30, 40, 55.8\n\n");
	printf("PRINT <variable_name> \n\n");
	printf("ADD <variable_name1> <variable_name2> <variable_name3> \n");
	printf("Ex: ADD A B C (C = A + B)\n\n");
	printf("MULTIPLY <variable_ame1> <variable_name2> <variable_name3> \n");
	printf("Ex: MUTLIPLY A B C ( C = A * B )\n\n");
	printf("OUTER_PRODUCT <variable_name1> <variable_name2> <variable_name3>\n");
	printf("Ex: OUTER_PRODUCT A B C\n\n");
	printf("HALT (to terminate the program)\n\n");
	printf("HELP (to see commands)\n");
}

//free all the dynamically allocated values for each variable
void free_all(variable* collection) {
	int i;
	for (i = 0; i < 26; i++) {
		if (collection[i].used) {
			free(collection[i].value);
		}
	}
}

//return true if the given filename exists else return false
int file_exists(char* filename) {
	FILE* file = fopen(filename, "rt");
	if (file != NULL) {
		fclose(file);
		return true;
	}
	else
		return false;
}

//pass in an empty string pointer and intialize it with the command to perform
// Ex: read_from (stdin, &result); read_from (file, &result);
int read_from(FILE* input, char** buf_ptr) {
	int buf_size;
	int c;

	*buf_ptr = (char*)malloc(sizeof(char));	// initial size of 1 byte/char
	if (*buf_ptr == NULL) {
		printf("dynamic allocation error in read_from\n");
		return false;
	}
	buf_size = 1;

	c = 0;

	while (true) {
		c = fgetc(input);
		if (c != '\n' && c != EOF) {
			// get a character
			(*buf_ptr)[buf_size - 1] = c;
			// grow a dyanmic char array
			buf_size++;
			*buf_ptr =(char*) realloc(*buf_ptr, buf_size);
			if (*buf_ptr == NULL) {
				printf("realloc error from read_from\n");
				free(*buf_ptr);
				return false;
			}
		}
		else {
			(*buf_ptr)[buf_size - 1] = '\0';
			break;
		}
	}
	return true;
}

int main(void) {
	char * user_input_method; //string to determine keyboard or file
	FILE* file;
	variable collection[26]; // There are 26 possible 'A'-'Z' variables
	intialize_vars_to_unused(collection);
	
	do {
		printf("User input from keyboard or files?(Type keyboard or file_name) \n");
		read_from(stdin, &user_input_method);

		if (!strcmp(user_input_method, " ")) {
			break;
		}
		if(user_input_method == NULL){
			break;
		}
		if (!strcmp(user_input_method, "keyboard")) {
			break;
		}
		if (file_exists(user_input_method)) {
			break;
		}

	} while (1);
	
	if (file_exists(user_input_method)) {
		file = fopen(user_input_method, "rt"); //open file in read-text mode
		if(file==NULL){
			perror("error opening file");
		}
	}

	while (true) {
		char *user_input;
		object obj;  
		variable var; 
		buffer input;

		//strcmp returns 0 if two parameter strings are equal
		if (!strcmp(user_input_method, " ") || !strcmp(user_input_method, "keyboard")) {
			read_from(stdin, &user_input);
		}
		
		if (file_exists(user_input_method)) {
			read_from(file, &user_input);
		}
		
		//initialize buffer
		input.input = user_input;
		input.cur = user_input;

		if (!command_parser(&input, &obj)) {
			continue;
		}
		
		if (!strcmp(obj.command, "SET")) {
			if (!SET_parser(&input, &obj, &var, collection)) {
				continue;
			}
		}
	
		if (!strcmp(obj.command, "ADD")) {
			if (!ADD_parser(&input,collection, &var)) {
				continue;
			}
		}
		if (!strcmp(obj.command, "MULTIPLY")) {
			if (!MULTIPLY_parser(&input, collection, &var)) {
				continue;
			}
		}

		if (!strcmp(obj.command, "OUTER_PRODUCT")) {
			if (!OUTER_PRODUCT_parser(&input, collection, &var)) {
				continue;
			}
		}

		if (!strcmp(obj.command, "PRINT")) {
			if (!PRINT_parser(&input, collection, &var));
			continue;
		}

		if (!strcmp(obj.command, "HALT")) {
			//free all dynamically allocated components
			free_all(collection);
			free(user_input);
			free(user_input_method);
			break;
		}

		if (!strcmp(obj.command, "HELP")) {
			print_help();
			continue;
		}

	}
	return 0;
}