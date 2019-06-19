#include "bareBench.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"


FILE *fpout;

//------------------------------------------------------------------------
unsigned int prand32 ( unsigned int x )
{
    if(x&1)
    {
        x=x>>1;
        x=x^0xBF9EC099;
    }
    else
    {
        x=x>>1;
    }
    return(x);
}


int main ( void )
{
    unsigned int prand;
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;
    unsigned int rd;

    prand=0x1234;

    DEBUG_PRINT(fpout=fopen("abcmath.c","wt");
    if(fpout==NULL) return(1);

    fprintf(fpout,"\n");
    fprintf(fpout,"unsigned int prand32 ( unsigned int x )\n");
    fprintf(fpout,"{\n");
    fprintf(fpout,"    if(x&1)\n");
    fprintf(fpout,"    {\n");
    fprintf(fpout,"        x=x>>1;\n");
    fprintf(fpout,"        x=x^0xBF9EC099;\n");
    fprintf(fpout,"    }\n");
    fprintf(fpout,"    else\n");
    fprintf(fpout,"    {\n");
    fprintf(fpout,"        x=x>>1;\n");
    fprintf(fpout,"    }\n");
    fprintf(fpout,"    return(x);\n");
    fprintf(fpout,"}\n");
    fprintf(fpout,"\n");
    fprintf(fpout,"unsigned int prand;\n");
    for(ra=0;ra<8;ra++) fprintf(fpout,"unsigned int %c;\n",'a'+ra);
    fprintf(fpout,"\n");
    fprintf(fpout,"unsigned int abcmath ( void )\n");
    fprintf(fpout,"{\n");
    for(;ra<16;ra++) fprintf(fpout,"unsigned int %c;\n",'a'+ra);
    fprintf(fpout,"\n");
    fprintf(fpout,"prand=0xABC;\n");
    for(ra=0;ra<16;ra++)
    {
        fprintf(fpout,"prand=prand32(prand); %c=prand;\n",'a'+ra);
    })
    for(ra=0;ra<2000;ra++)
    {
        prand=prand32(prand);
        prand=prand32(prand);
        prand=prand32(prand);
        rb=(prand>> 0)&15;
        while(1)
        {
            prand=prand32(prand);
            prand=prand32(prand);
            prand=prand32(prand);
            rc=(prand>> 8)&15;
            if(rb==rc) continue;
            break;
        }
        while(1)
        {
            prand=prand32(prand);
            prand=prand32(prand);
            prand=prand32(prand);
            prand=prand32(prand);
            rd=(prand>>16)&15;
            if(rb==rc) continue;
            if(rb==rd) continue;
            break;
        }
        prand=prand32(prand);
        prand=prand32(prand);
        prand=prand32(prand);
        switch((prand>>24)&15)
        {
            case  0: DEBUG_PRINT(fprintf(fpout,"%c = %c + %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  1: DEBUG_PRINT(fprintf(fpout,"%c = %c - %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  2: DEBUG_PRINT(fprintf(fpout,"%c = %c * %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  3: DEBUG_PRINT(fprintf(fpout,"%c = %c ^ %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  4: DEBUG_PRINT(fprintf(fpout,"%c = %c & %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  5: DEBUG_PRINT(fprintf(fpout,"%c = %c | %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  6: DEBUG_PRINT(fprintf(fpout,"%c = %c + %c;\n",rb+'a',rc+'a',rd+'a');) break;
            case  7: DEBUG_PRINT(fprintf(fpout,"prand=prand32(prand); %c = prand;\n",rb+'a');) break;

            case  8: DEBUG_PRINT(fprintf(fpout,"if(%c>%c) { prand=prand32(prand); %c = prand; }\n",rb+'a',rc+'a',rd+'a');) break;
            case  9: DEBUG_PRINT(fprintf(fpout,"if(%c<%c) { prand=prand32(prand); %c = prand; }\n",rb+'a',rc+'a',rd+'a');) break;
            case 10: DEBUG_PRINT(fprintf(fpout,"if(%c==%c) { prand=prand32(prand); %c = prand; }\n",rb+'a',rc+'a',rd+'a');) break;
            case 11: DEBUG_PRINT(fprintf(fpout,"if(%c>=%c) { prand=prand32(prand); %c = prand; }\n",rb+'a',rc+'a',rd+'a');) break;
            case 12: DEBUG_PRINT(fprintf(fpout,"if(%c<=%c) { prand=prand32(prand); %c = prand; }\n",rb+'a',rc+'a',rd+'a');) break;
            default:
                DEBUG_PRINT(fprintf(fpout,"%c = %c | %c;\n",rb+'a',rc+'a',rd+'a'); break;)
                break;
        }
    }

    DEBUG_PRINT(
    for(ra=1;ra<16;ra++)
    {
        fprintf(fpout,"a += %c;\n",'a'+ra);
    }
    fprintf(fpout,"return(a);\n");)

    DEBUG_PRINT(fprintf(fpout,"}\n");)
    DEBUG_PRINT(fprintf(fpout,"\n");)
    return(0);
}
