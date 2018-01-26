#include <conio.h>

int main(int argc, char **argv) {
    Keycode k;
    do{
    	k = (Keycode) Get_Key();
    	if ((k & KEY_SPECIAL_FLAG) || (k & KEY_RELEASE_FLAG))
            continue;
    	if (k == '\r')
            k = '\n';
    	k &= 0xff;
    	if(k == '@'){
    		Put_Char(k);
    		break;
    	}
    	else
    		Put_Char(k);
    }while(true);
    return 1;
}
