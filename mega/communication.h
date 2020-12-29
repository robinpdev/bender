#include "packet-types.h"

void handlepacket(char var, String returned){
    Serial.print("received packet (");
    Serial.print(var);
    Serial.print("): ");
    Serial.println(returned);
    switch(var){
        case startup:{
            statusl.tupdate("status: due gestart, beweeg tot ref");
        }

        case vstate:{
            state = returned.toInt();
            if(state == 1){
                statusl.tupdate("status: ref gevonden, geef doel in");
            }else if (state == 2){
                statusl.tupdate("status: beweeg naar doel");
            }
        }

        case apos:{
            Serial.println("got apos");
            posd.tupdate(returned);
            break;
        }

        
        case setdikte:{
            dikte = returned.toInt();
            diktei.tupdate(returned);
            break;
        }

        case setdiepte:{
            diepte = returned.toInt();
            dieptei.tupdate(returned);
            break;
        }

        case phoogte:{
            hoogte = returned.toInt();
            hoogtei.tupdate(returned);
            break;
        }

        case pmes:{
            mes = returned.toInt();
            mesi.tupdate(returned);
            break;
        }

        case pmaxspd:{
            maxspd = returned.toInt();
            maxspdi.tupdate(returned);
            break;
        }

        case pminspd:{
            minspd = returned.toInt();
            minspdi.tupdate(returned);
            break;
        }

        default:{
            break;
        }
    }
}