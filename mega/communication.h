

void handlepacket(char var, String returned){
    Serial.print("received packet (");
    Serial.print(var);
    Serial.print("): ");

    switch(var){

        case apos:{
            posd.tupdate(returned);
        }

        default:{
            break;
        }
    }
}