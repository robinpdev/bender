//metacom is a header library for communication between serial ports on 2 arduinos
//the main use of this is transferring variables form 1 board to another
#include <arduino.h>

#ifndef METACOM_MAXTRIES
#define METACOM_MAXTRIES 1
#endif

void handlepacket(char var, String returned);

void sendchar(char var, char input)
{
	cp.write('!');
	cp.write(var);
	cp.write(input);
	cp.write('#');
	//npackets ++;
	//nrpackets ++;
}

void sendstring(char var, String input)
{
	const int len = input.length() + 1;
	cp.write('!');
	cp.write(var);
	char buf[len];
	input.toCharArray(buf, len);
	for (int n = 0; n <= len - 1; n++)
	{
		cp.write(buf[n]);
	}
	cp.write('#');
	//npackets ++;
	//nrpackets ++;
}

void sendint(char var, int input)
{
	cp.write('!');
	cp.write(var);
	cp.print(String(input));
	cp.write('#');
}

template <typename T>
void send(char var, T input)
{
	cp.write('!');
	cp.write(var);
	cp.print(input);
	cp.write('#');
}

void send(char var)
{
	cp.write('!');
	cp.write(var);
	cp.write('#');
}

float stringtofloat(String in)
{
	float out = 0.0;
	short commaindex = in.length();
	for (short i = 0; i < in.length(); i++)
	{
		if (in.charAt(i) == ',')
		{
			commaindex = i;
			Serial.print("commaindex: "); Serial.println(commaindex);
			break;
		}
	}
	for (short i = 0; i < commaindex; i++)
	{
		out += ((int)(in.charAt(i) - '0')) * pow(10, commaindex - 1 - i);
	}
	for (short i = commaindex + 1; i < in.length(); i++){
		Serial.print("adding "); Serial.println((int)(in.charAt(i) - '0') * pow(10, -i + commaindex));
		out += ((int)(in.charAt(i) - '0')) * pow(10, -i + commaindex);
	}
	Serial.println(out);
	return out;
}

void updatepackets()
{
	while (cp.read() == '!' && cp.available() >= 2)
	{ //TODO: research how to wait for incomplete packets the best
		//pakcet detected, reading in packet
		String returned = "";
		char var = cp.read();

		bool got_end_character = false;
		short tries = 0;
		while (!got_end_character)
		{
			while (cp.available())
			{
				char inchar = cp.read();
				if (inchar == '#')
				{
					handlepacket(var, returned);
					got_end_character = true;
					break;
				}
				else if (inchar == '!')
				{
					Serial.println("corrupted packet!: " + var);
					//ask for this variable again because it's corrupted
					send('f', var);
					send('f', cp.read());
					got_end_character = true; //actually not true, but not making another bool makes it faster
					break;
				}
				returned += inchar;
			}

			//check if packet is complete
			if (got_end_character)
			{
				break;
			}

			//when not complete, retry, but do not exceed maximum number of tries
			if (tries >= METACOM_MAXTRIES)
			{
				Serial.println("could not save fractured packet");
				break;
			}
			Serial.println("w");
			tries++;
			delay(1);
		}
	}
}