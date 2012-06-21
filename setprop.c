#include <stdio.h>

#include <cutils/properties.h>

int setprop_main(int argc, char *argv[])
{
    if(argc != 3) {
        fprintf(stderr,"usage: setprop <key> <value>\n");
        return 1;
    }

    if(property_set(argv[1], argv[2])){
        fprintf(stderr,"could not set property\n");
        return 1;
    }

    return 0;
}

int getprop_main(int argc, char *argv[])
{
	if(argc != 2) {
		fprintf(stderr,"usage: getprop <key>\n");
		return 1;
    }
	char value[PROPERTY_VALUE_MAX];
	char *default_value;
	if(argc > 2) {
		default_value = argv[2];
	} else {
		default_value = "";
	}

    property_get(argv[1], value, default_value);
    printf("%s\n", value);
	return 0;
}
