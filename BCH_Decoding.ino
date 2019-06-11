bool codeword[15];
int value_of_n = 15;
bool minimal_polynomials[][5] = {{true, true, false, false, true},
                                 {true, true, true, true, true},
                                 {false, false, true, true, true}
                                 };
bool minimal_polynomials_wiki[][5] = {{true, false, false, true, true},
                                      {true, true, true, true, true},
                                      {false, false, true, true, true}
                                     };
int roots_for_syndromes[][3] = {{ 1, 2 , 4},
                                { -1, 3, 6},
                                { -1, -1, 5}
                               };
bool intermediate_syndrome[3][15];
int syndrome_values[6];
int error_locating_polynomial[3];
int error_locations[3];
int roots_of_elp[3];
int location_of_syndrome_values[6];
bool intermediate_array[15];
int counter = 0;
int beginning_pointer = 0;
int required_field[] =      {1, 2, 4, 8, 9, 11, 15, 7, 14, 5, 10, 13, 3, 6, 12};
int required_field_wiki[] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9};

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
    delay(10);//Because otherwise it was not cleaning out the buffer completely when more than 10 digits were entered.
    //We don't know why this is happening.
  }
}

void zeroingOut() {
  for (int i = 0; i < sizeof(codeword) / sizeof(codeword[0]); i++) {
    codeword[i] = false;
  }
}

bool getMessage() {
  int flag = 0;
  int length_of_message = 0;
  Serial.println("Please enter the message: ");
  while (!Serial.available()) {  }
  int i = 0;
  while (true) {
    if (Serial.available() > 0) {
      int temp_value = Serial.read();
      if (temp_value == 48) {
        codeword[i] = false;
      } else {
        codeword[i] = true;
      }
      length_of_message += 1;
      i += 1;
    }
    if (length_of_message == value_of_n) {
      return true;
    }
  }
}

int getNewBeginningPointer(bool processed_array[], int arrSize) {
  for (int i = 0; i < arrSize; i++) {
    if (processed_array[i] == true) {
      int newpointer = i;
      return newpointer;
    }
  }
  return -1;
}

void calculateSyndromes() {
  for (int i = 0; i < sizeof(codeword) / sizeof(codeword[0]); i++) {
    intermediate_array[i] = codeword[i];
  }

  int pointer_for_minpolynomial = getNewBeginningPointer(minimal_polynomials[counter], sizeof(minimal_polynomials[counter]) / sizeof(minimal_polynomials[counter][0]));
  int size_of_interarray = sizeof(intermediate_array) / sizeof(intermediate_array[0]);
  int size_of_minpolynomial =  sizeof(minimal_polynomials[counter]) / sizeof(minimal_polynomials[counter][0]);
  int size_of_divisor = sizeof(minimal_polynomials[counter]) / sizeof(minimal_polynomials[counter][0]) - pointer_for_minpolynomial;

  beginning_pointer = getNewBeginningPointer(intermediate_array, size_of_interarray);
  while (size_of_interarray - beginning_pointer >= size_of_divisor & beginning_pointer != -1) {
    int count = beginning_pointer;
    for (int i = pointer_for_minpolynomial; i < size_of_minpolynomial ; i++) {
      intermediate_array[count] = intermediate_array[count] ^ minimal_polynomials[counter][i];
      count += 1;
    }
    beginning_pointer = getNewBeginningPointer(intermediate_array, size_of_interarray);
  }
  counter += 1;
}

int findingSyndromes() {
  int syndrome_value = 0;
  for (int i = 0; i < sizeof(roots_for_syndromes) / sizeof(roots_for_syndromes[0]); i++) {
    for (int j = 0; j < sizeof(roots_for_syndromes[i]) / sizeof(roots_for_syndromes[i][0]); j++) {
      int total_value = 0;
      if (roots_for_syndromes[i][j] != -1) {
        if (intermediate_syndrome[i][0] == true) {
          syndrome_values[(roots_for_syndromes[i][j]) - 1] ^= 1;
        }
        for (int k = 1; k < sizeof(intermediate_syndrome[i]) / sizeof(intermediate_syndrome[i][0]); k++) {
          if (intermediate_syndrome[i][k] == true) {
            int value_of_syndrome = roots_for_syndromes[i][j] * k;
            if ( value_of_syndrome > 15) {
              value_of_syndrome %= 15;
            }
            syndrome_values[(roots_for_syndromes[i][j]) - 1] ^= required_field[value_of_syndrome];
          }
        }
      }
    }
  }
}
int findingSyndromeLocations(int value_to_find) {
  for (int j = 0; j < sizeof(required_field) / sizeof(required_field[0]); j++) {
    if (value_to_find == required_field[j]) {
      return j;
    }
  }
  return -1;
}

void findingErrorLocatingPoly(){
  int final_value_numerator = 0;
  error_locating_polynomial[0] = location_of_syndrome_values[0];
  int temporary_variable = location_of_syndrome_values[0] * 2;
  temporary_variable += location_of_syndrome_values[2];
  if(temporary_variable >= 15){
    temporary_variable %= 15;
  }
  if(location_of_syndrome_values[4] == -1){
    final_value_numerator = required_field[temporary_variable]^0;
  }else{
    final_value_numerator = required_field[temporary_variable]^required_field[location_of_syndrome_values[4]];
  }
  int temporary_variable_two = location_of_syndrome_values[0]*3;
  if(temporary_variable_two >= 15){
    temporary_variable_two %= 15;
  }
  int final_value_denominator = required_field[temporary_variable_two]^ required_field[location_of_syndrome_values[2]];
  int final_value_numposition = findingSyndromeLocations(final_value_numerator);
  int final_value_denposition = findingSyndromeLocations(final_value_denominator);
  error_locating_polynomial[1] = final_value_numposition - final_value_denposition;
  int third_locating_value = location_of_syndrome_values[0] + error_locating_polynomial[1];
  if(third_locating_value >= 15){
    third_locating_value %= 15;
  }
  int third_intermediate_value = required_field[final_value_denposition]^required_field[third_locating_value];
  error_locating_polynomial[2] = findingSyndromeLocations(third_intermediate_value);
}

void findingRootsOfELP(){
  int another_counter = 0;
  for(int i=0;i<15;i++){
    int multiplier = 1;
    int final_value = 1;
    for(int j=0;j<3;j++){
      int temporary_value = error_locating_polynomial[j]+(i*multiplier);
      if(temporary_value >= 15){
        temporary_value %= 15;
      }
      final_value ^= required_field[temporary_value];
      multiplier += 1;
    }
    if(final_value == 0){
      roots_of_elp[another_counter] = i;
      another_counter += 1;
    }
  }
}

void findingErrorLocations(){
  for(int i=0;i<3;i++){
    int temporary_variable = 15 - roots_of_elp[i];
    temporary_variable %= 15;
    error_locations[i] = temporary_variable;
    Serial.print("Error is at : ");
    Serial.println(14 - error_locations[i]);
  }
}

void fixingMessage(){
  
  for(int i=0;i<3;i++){
    codeword[14 - error_locations[i]] ^= 1;
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  getMessage();
  serialFlush();
  for (int i = 0; i < sizeof(minimal_polynomials) / sizeof(minimal_polynomials[0]); i++) {
    calculateSyndromes();
    int reverse_counter = 0;
    for (int j = sizeof(intermediate_array) / sizeof(intermediate_array[0]) - 1; j >= 0; j--) {
      intermediate_syndrome[i][reverse_counter] = intermediate_array[j];
      reverse_counter += 1;
    }
  }
  /*
  for (int i = 0; i < sizeof(intermediate_syndrome) / sizeof(intermediate_syndrome[0]); i++) {
    for (int j = 0; j < sizeof(intermediate_syndrome[0]) / sizeof(intermediate_syndrome[0][0]); j++) {
      Serial.print(intermediate_syndrome[i][j]);
    }
    Serial.println();
  }
  */
  findingSyndromes();
  Serial.println("Your Syndrome looks something like : [ ");
  for (int i = 0; i < sizeof(syndrome_values) / sizeof(syndrome_values[0]); i++) {
    Serial.print(syndrome_values[i]);
    Serial.print(" ");
    location_of_syndrome_values[i] = findingSyndromeLocations(syndrome_values[i]);
    //Serial.print("Location of ");
    //Serial.print(syndrome_values[i]);
    //Serial.print(" is at ");
    //Serial.println(location_of_syndrome_values[i]);
  }
  Serial.println("]");
  findingErrorLocatingPoly();
  /*
  for(int i=0;i<sizeof(error_locating_polynomial)/sizeof(error_locating_polynomial[0]);i++){
    Serial.println(error_locating_polynomial[i]);
  }
  */
  Serial.println("Found polynomial, calculating roots!");
  findingRootsOfELP();
  Serial.println("Found roots, finding error locations!");
  findingErrorLocations();
  fixingMessage();
  Serial.println("Corrected message is : ");
  for(int i=0;i<15;i++){
    Serial.print(codeword[i]);
  }
  Serial.println();
  zeroingOut();
}
