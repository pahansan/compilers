#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("%s\n", getenv("LC_NUMERIC"));
    float a;
    scanf("%f", &a);
    printf("%f\n", a);
}