/**
 * URL½âÂëº¯Êý
 * 
 * @author ÍõÁÕ
 */
#include "urlencode.h"

void URL_Decode(char* pin, char* pout)
{
	int indexin = 0,indexout = 0;
	while(pin[indexin]!='\0')
	{
		if(pin[indexin] == '%') {
			indexin++;
			if(pin[indexin] >='0' && pin[indexin] <= '9') {
				pout[indexout] = pin[indexin] - 0x30;
				pout[indexout] *= 16;	
			}
			else if(pin[indexin] >= 'A'&&pin[indexin] <= 'F') {
				pout[indexout] = pin[indexin] - 0x37;
				pout[indexout] *= 16;
			}
			else {
			}
			indexin++;
			if(pin[indexin] >='0' && pin[indexin] <= '9') {
				pout[indexout] = pout[indexout] + pin[indexin] - 0x30;
			}
			else if(pin[indexin] >= 'A' && pin[indexin] <= 'F') {
				pout[indexout] = pout[indexout] + pin[indexin] - 0x37;
			}
			else {
			}
			indexin++;
			indexout++;
		}
		else {
			pout[indexout] = pin[indexin];
			indexout++;
			indexin++;
		}
	}
	pout[indexout] = '\0';
}
