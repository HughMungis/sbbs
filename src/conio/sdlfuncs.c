#include <stdlib.h>	/* getenv()/exit()/atexit() */
#include <stdio.h>	/* NULL */

#include "gen_defs.h"
#include "threadwrap.h"
#include <SDL.h>
#include "sdlfuncs.h"
#include "sdl_con.h"
extern int sdl_video_initialized;

struct sdlfuncs sdl;

/* Make xp_dl do static linking */
#ifdef STATIC_SDL
#define STATIC_LINK
#endif
#include <xp_dl.h>

static int sdl_funcs_loaded=0;
static int sdl_initialized=0;
static int sdl_audio_initialized=0;

static void QuitWrap(void);

int load_sdl_funcs(struct sdlfuncs *sdlf)
{
	dll_handle	sdl_dll;
	const char *libnames[]={"SDL2", "SDL", NULL};

	sdlf->gotfuncs=0;
	if((sdl_dll=xp_dlopen(libnames,RTLD_LAZY|RTLD_GLOBAL,SDL_PATCHLEVEL))==NULL)
		return(-1);

	if((sdlf->Init=xp_dlsym(sdl_dll, SDL_Init))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->Quit=xp_dlsym(sdl_dll, SDL_Quit))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	// SDL2: Rename from mutexP/mutexV to LockMutex/UnlockMutex
	if((sdlf->mutexP=xp_dlsym(sdl_dll, SDL_LockMutex))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->mutexV=xp_dlsym(sdl_dll, SDL_UnlockMutex))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->PeepEvents=xp_dlsym(sdl_dll, SDL_PeepEvents))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetCurrentVideoDriver=xp_dlsym(sdl_dll, SDL_GetCurrentVideoDriver))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SemWait=xp_dlsym(sdl_dll, SDL_SemWait))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SemWaitTimeout=xp_dlsym(sdl_dll, SDL_SemWaitTimeout))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SemPost=xp_dlsym(sdl_dll, SDL_SemPost))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->EventState=xp_dlsym(sdl_dll, SDL_EventState))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateRGBSurface=xp_dlsym(sdl_dll, SDL_CreateRGBSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateRGBSurfaceFrom=xp_dlsym(sdl_dll, SDL_CreateRGBSurfaceFrom))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->FillRect=xp_dlsym(sdl_dll, SDL_FillRect))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->BlitSurface=xp_dlsym(sdl_dll, SDL_UpperBlit))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->RenderPresent=xp_dlsym(sdl_dll, SDL_RenderPresent))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SDL_CreateSemaphore=xp_dlsym(sdl_dll, SDL_CreateSemaphore))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SDL_DestroySemaphore=xp_dlsym(sdl_dll, SDL_DestroySemaphore))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SDL_CreateMutex=xp_dlsym(sdl_dll, SDL_CreateMutex))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateThread=xp_dlsym(sdl_dll, SDL_CreateThread))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->WaitThread=xp_dlsym(sdl_dll, SDL_WaitThread))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->WaitEventTimeout=xp_dlsym(sdl_dll, SDL_WaitEventTimeout))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->PollEvent=xp_dlsym(sdl_dll, SDL_PollEvent))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateWindow=xp_dlsym(sdl_dll, SDL_CreateWindow))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateWindowAndRenderer=xp_dlsym(sdl_dll, SDL_CreateWindowAndRenderer))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateRenderer=xp_dlsym(sdl_dll, SDL_CreateRenderer))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->FreeSurface=xp_dlsym(sdl_dll, SDL_FreeSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SetWindowTitle=xp_dlsym(sdl_dll, SDL_SetWindowTitle))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetWindowSize=xp_dlsym(sdl_dll, SDL_GetWindowSize))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetWindowSurface=xp_dlsym(sdl_dll, SDL_GetWindowSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SetWindowIcon=xp_dlsym(sdl_dll, SDL_SetWindowIcon))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->ShowCursor=xp_dlsym(sdl_dll, SDL_ShowCursor))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->WasInit=xp_dlsym(sdl_dll, SDL_WasInit))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetWindowWMInfo=xp_dlsym(sdl_dll, SDL_GetWindowWMInfo))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetError=xp_dlsym(sdl_dll, SDL_GetError))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->InitSubSystem=xp_dlsym(sdl_dll, SDL_InitSubSystem))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->QuitSubSystem=xp_dlsym(sdl_dll, SDL_QuitSubSystem))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->OpenAudio=xp_dlsym(sdl_dll, SDL_OpenAudio))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CloseAudio=xp_dlsym(sdl_dll, SDL_CloseAudio))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->PauseAudio=xp_dlsym(sdl_dll, SDL_PauseAudio))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->LockAudio=xp_dlsym(sdl_dll, SDL_LockAudio))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->UnlockAudio=xp_dlsym(sdl_dll, SDL_UnlockAudio))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->GetAudioStatus=xp_dlsym(sdl_dll, SDL_GetAudioStatus))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->MapRGB=xp_dlsym(sdl_dll, SDL_MapRGB))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->LockSurface=xp_dlsym(sdl_dll, SDL_LockSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->UnlockSurface=xp_dlsym(sdl_dll, SDL_UnlockSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->ConvertSurface=xp_dlsym(sdl_dll, SDL_ConvertSurface))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->CreateTexture=xp_dlsym(sdl_dll, SDL_CreateTexture))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->UpdateTexture=xp_dlsym(sdl_dll, SDL_UpdateTexture))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->RenderClear=xp_dlsym(sdl_dll, SDL_RenderClear))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->RenderCopy=xp_dlsym(sdl_dll, SDL_RenderCopy))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}
	if((sdlf->SetHint=xp_dlsym(sdl_dll, SDL_SetHint))==NULL) {
		xp_dlclose(sdl_dll);
		return(-1);
	}

	sdlf->gotfuncs=1;
	sdl_funcs_loaded=1;
	return(0);
}

int init_sdl_video(void)
{
	char	*drivername;
	int		use_sdl_video=FALSE;
#ifdef _WIN32
	char		*driver_env=NULL;
#endif

	if(sdl_video_initialized)
		return(0);

	load_sdl_funcs(&sdl);

	if (!sdl.gotfuncs)
		return -1;

	use_sdl_video=TRUE;

	sdl.SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1" );
	sdl.SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1" );
#ifdef _WIN32
	/* Fail to windib (ie: No mouse attached) */
	if(sdl.Init(SDL_INIT_VIDEO)) {
		// SDL2: We can likely do better now...
		driver_env=getenv("SDL_VIDEODRIVER");
		if(driver_env==NULL || strcmp(driver_env,"windib")) {
			putenv("SDL_VIDEODRIVER=windib");
			WinExec(GetCommandLine(), SW_SHOWDEFAULT);
			return(0);
		}
		/* Sure ,we can't use video, but audio is still valid! */
		if(sdl.Init(0)==0)
			sdl_initialized=TRUE;
	}
	else {
		sdl_video_initialized=TRUE;
		sdl_initialized=TRUE;
	}
#else
	/*
	 * SDL2: Is the below comment still true for SDL2?
	 * On Linux, SDL doesn't properly detect availability of the
	 * framebuffer apparently.  This results in remote connections
	 * displaying on the local framebuffer... a definate no-no.
	 * This ugly hack attempts to prevent this... of course, remote X11
	 * connections must still be allowed.
	 */
	if((!use_sdl_video) || ((getenv("REMOTEHOST")!=NULL || getenv("SSH_CLIENT")!=NULL) && getenv("DISPLAY")==NULL)) {
		/* Sure ,we can't use video, but audio is still valid! */
		if(sdl.Init(0)==0)
			sdl_initialized=TRUE;
	}
	else {
		if(sdl.Init(SDL_INIT_VIDEO)==0) {
			sdl_initialized=TRUE;
			sdl_video_initialized=TRUE;
		}
		else {
			/* Sure ,we can't use video, but audio is still valid! */
			if(sdl.Init(0)==0)
				sdl_initialized=TRUE;
		}
	}
#endif
	if(sdl_video_initialized && (drivername = sdl.GetCurrentVideoDriver())!=NULL) {
		/* Unacceptable drivers */
		if((!strcmp(drivername, "caca")) || (!strcmp(drivername,"aalib")) || (!strcmp(drivername,"dummy"))) {
			sdl.QuitSubSystem(SDL_INIT_VIDEO);
			sdl_video_initialized=FALSE;
		}
		else {
			sdl_video_initialized=TRUE;
		}
	}

	if(sdl_video_initialized) {
		SetThreadName("SDL Main");
		atexit(QuitWrap);
		return 0;
	}

	return(-1);
}

int init_sdl_audio(void)
{
	if(!sdl_initialized)
		return(-1);
	if(sdl_audio_initialized)
		return(0);
	if(sdl.InitSubSystem(SDL_INIT_AUDIO)==0) {
		sdl_audio_initialized=TRUE;
		return(0);
	}
	return(-1);
}

void run_sdl_drawing_thread(int (*drawing_thread)(void *data))
{
	sdl.CreateThread(drawing_thread, NULL);
}

static void QuitWrap(void)
{
	if (sdl_initialized) {
		exit_sdl_con();
		if(sdl.Quit)
			sdl.Quit();
	}
}
