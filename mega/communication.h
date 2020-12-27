void handlepacket(char var, String returned){
    Serial.print("received packet (");
    Serial.print(var);
    Serial.print("): ");

    switch(var){
        case 'a':{
            char a = returned.charAt(0);
            Serial.println(a);
            break;
        }
        
        case 'b':{
            int b = returned.toInt();
            Serial.println(b);
            break;
        }
        
        default:{
            break;
        }
    }
}