#include "packet-types.h"


String pulstomm(String in){
    float pulsen = in.toInt() * 0.02;
    int nature = pulsen;
    int frac = (pulsen - nature) * 100;
    return String(nature) + ',' + String(frac);
}

void handlepacket(char var, String returned){

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
            break;
        }

        case apos:{
            posd.tupdate(pulstomm(returned));
            break;
        }

        
        case setdikte:{
            dikte = returned.toInt();
            diktei.tupdate(pulstomm(returned));
            break;
        }

        case setdiepte:{
            diepte = returned.toInt();
            dieptei.tupdate(pulstomm(returned));
            break;
        }

        case phoogte:{
            hoogte = returned.toInt();
            hoogtei.tupdate(pulstomm(returned));
            break;
        }

        case pmes:{
            mes = returned.toInt();
            mesi.tupdate(pulstomm(returned));
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