/*===============================================================================================
 Ce fichier est issu du programme "User Created Sound Example" de l'API FMODEx (c)
 Son but est d'effectuer diverses synthèses sonores en programmation de bas niveau.
===============================================================================================*/
#include "inc/fmod.hpp"
#include "inc/fmod_errors.h"
#include "inc/wincompat.h"
#include "inc/erreurs.h"
#include <math.h>
#include <string.h>
#define PI 3.1415926535

/* QUESTION 1 : */

float freq = 440.0f ;		// toutes questions
float ampGauche=0.5, ampDroite=0.5 ;
float distorsion = 1 ;

FMOD_RESULT F_CALLBACK cb_question1 (FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    static float  t = 0 ;
    signed short *stereo16bitbuffer = (signed short *)data;

    for (count=0; count<datalen>>2; count++)
    {
	// un échantillon à gauche puis à droite (format "signed short" = entier entre -32768 et 32767)

        *stereo16bitbuffer++ = (signed short) 0;
        *stereo16bitbuffer++ = (signed short) 0;
	    t += 0 ;
    }

    return FMOD_OK;
}

/* EXERCICE 3 */

float Attack=0.1, Decay=0.1, Duree=2, Sustain=0.8, Release=1.5 ;

float adsr (float x)
{
    return 0 ;
}

/* Question 4 */

#define numFrequences 4
float TempsFrequences[2][numFrequences]={{0,2,3,6},{200,400,500,200}};

float frequence (float t) 
{
    return 440.0f ;
}

/* Question 5 */

#define PUR 0
#define CARRE 1
#define TRIANGLE 2
#define ADDITIF 3

#define nbSin 9
float harmo[nbSin] = {1. , 0 , 1./3, 0, 1./5, 0, 1./7, 0, 1./9} ;

float timbre (int type, float t)
{
	float resultat=0 ;
	
        switch (type)
            {
            case PUR :
            {
              resultat = 0 ;
              break;
            }
            case CARRE :
            {
              resultat = 0 ;
              break;
            }
            case TRIANGLE :
            {
              resultat = 0 ;
              break;
            }
            case ADDITIF :
            {
              resultat = 0 ;
            }
		}
         return resultat ;
}

/* Question 6 */

FMOD_RESULT F_CALLBACK cb_globale (FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    unsigned int  count;
    static float  t1 = 0;
    signed short *stereo16bitbuffer = (signed short *)data;

    for (count=0; count<datalen>>2; count++)
    {
        freq = frequence(t1) ;
        *stereo16bitbuffer++ = (signed short) 0 ;
        *stereo16bitbuffer++ = (signed short) 0 ;
        t1 += 0 ;
    }
    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return FMOD_OK;
}

int main(int argc, char *argv[])
{
    FMOD::System           *system;
    FMOD::Sound            *sound;
    FMOD::Channel          *channel = 0;
    FMOD_RESULT             result;
    FMOD_MODE               mode = FMOD_2D | FMOD_OPENUSER | FMOD_HARDWARE | FMOD_CREATESTREAM ;
    int                     key;
    int                     channels = 2;
    FMOD_CREATESOUNDEXINFO  createsoundexinfo;
    unsigned int            version;

    /* Initialisation du moteur audio */

    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = system->init(32, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    /* Choix de la question */

    printf("Numéro de question (ou toute autre touche pour quitter) : \n");
    key = getch();
    if (key != '1' && key !='6')
    {
        return 0 ;
    }

    printf("%d\n",key-48) ;

    /* Création d'un buffer circulaire */

    memset(&createsoundexinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    createsoundexinfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);
    createsoundexinfo.decodebuffersize  = 3000;
    createsoundexinfo.length            = 44100 * channels * sizeof(signed short) * 5;
    createsoundexinfo.numchannels       = channels;
    createsoundexinfo.defaultfrequency  = 44100;
    createsoundexinfo.format            = FMOD_SOUND_FORMAT_PCM16;
    createsoundexinfo.pcmsetposcallback = pcmsetposcallback;

    if (key == '1')
    {
        mode |= FMOD_LOOP_NORMAL ;
	    createsoundexinfo.pcmreadcallback   = cb_question1;
    }
    else if (key == '6')
    {
        mode |= FMOD_LOOP_OFF ;
	    createsoundexinfo.pcmreadcallback   = cb_globale;
    }

    result = system->createSound(0, mode, &createsoundexinfo, &sound);
    ERRCHECK(result);

    /* Lecture du buffer */

    printf("Espace = pause, Echap = quitter\n");

    result = system->playSound(FMOD_CHANNEL_FREE, sound, 0, &channel);
    ERRCHECK(result);

    /* Boucle principale */

    do
    {
        /* Lecture des commandes de l'utilisateur pendant la lecture 
           et définition des changements à appliquer */

        if (kbhit())
        {
            key = getch();

            switch (key)
            {
            case ' ' :
            {
                bool paused;
                channel->getPaused(&paused);
                channel->setPaused(!paused);
                break;
            }


            /*    QUESTION 2    */
            
            case '*' :
            {
                break;
            }
            case '/' :
            {
                break;
            }
            
             case '+' :
            {
                break;
            }
            case '-' :
            {
                break;
            }
            case '>' :
            {
                break;
            }
            case '<' :
            {
                break;
            }
            case 'g' :
            {
                break;
            }
            case 'd' :
            {
                break;
            }
            
            }
        }

	/* Application des changements */

        system->update();

	/* Récupération et affichage de l'état de la lecture (en pause, arrêté, etc.) */

        if (channel)
        {
            unsigned int ms;
            unsigned int lenms;
            bool         playing;
            bool         paused;

            result = channel->isPlaying(&playing);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPaused(&paused);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = sound->getLength(&lenms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            printf("Temps %02d:%02d:%02d/%02d:%02d:%02d : %s\r", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Pause  " : playing ? "Lecture" : "Arret  ");
            fflush(stdout);
        }

        Sleep(20);

    }
    while (key != 27);

    printf("\n");

    /* Libération et fermeture du système audio */

    result = sound->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    return 0;
}



