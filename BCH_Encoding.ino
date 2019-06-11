bool generator_polynomial[] = {true, true, true, false, true, true, false, false, true, false, true};
int value_of_n = 5;
bool message[15];
bool original_message[5];
int encoded_message[15];
int beginning_pointer = 0;


void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
    delay(10);//Because otherwise it was not cleaning out the buffer completely when more than 10 digits were entered.
    //We don't know why this is happening.
  }
}

void zeroingOut(){
  for(int i=0;i<sizeof(message)/sizeof(message[0]);i++){
    message[i] = false;
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
        message[i] = false;
      } else {
        message[i] = true;
      }
      original_message[i] = message[i];
      length_of_message += 1;
      i += 1;
    }
    if (length_of_message == value_of_n) {
      return true;
    }
  }
}

int getNewBeginningPointer() {
  for (int i = 0; i < sizeof(message) / sizeof(message[0]); i++) {
    if (message[i] == true) {
      int newpointer = i;
      return newpointer;
    }
  }
  return -1;
}

void getChecksumBits() {
  beginning_pointer = getNewBeginningPointer();
  while ((sizeof(message) / sizeof(message[0])) - beginning_pointer >= (sizeof(generator_polynomial) / sizeof(generator_polynomial[0])) & beginning_pointer != -1) {
    int count = beginning_pointer;
    for (int i = 0; i < sizeof(generator_polynomial) / sizeof(generator_polynomial[0]); i++) {
      message[count] = message[count] ^ generator_polynomial[i];
      count += 1;
    }
    beginning_pointer = getNewBeginningPointer();
  }
}

void getFinalEncoding(){
  for(int i=0;i<sizeof(original_message)/sizeof(original_message[0]);i++){
    encoded_message[i] = original_message[i];
  }
  int final_pointer = getNewBeginningPointer();
  for(int i = final_pointer;i<sizeof(message)/sizeof(message[0]);i++){
    encoded_message[i] = message[i];
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  getMessage();
  for(int i=0;i<5;i++){
    Serial.print(message[i]);
  }
  serialFlush();
  getChecksumBits();
  getFinalEncoding();
  Serial.print("Your final encoded message is = ");
  for (int i = 0; i < sizeof(encoded_message) / sizeof(encoded_message[0]); i++) {
    Serial.print(encoded_message[i]);
  }
  Serial.println();
  zeroingOut();
}
