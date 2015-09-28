/* Fichier extrait des exemples de l'API FModEx */

#ifndef _ERREURS_H
#define _ERREURS_H

#include <stdio.h>
#include "fmod.hpp"
#include "fmod_errors.h"

void ERRCHECK(FMOD_RESULT result) ;

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

#endif

